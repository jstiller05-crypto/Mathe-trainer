#include "task_view.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

TaskView::TaskView(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(24);
    mainLayout->setContentsMargins(60, 40, 60, 40);
    // Schiebt die Button-Reihe ganz nach unten - das Raster selbst liegt NICHT im
    // Layout, sondern als absolut positioniertes Widget dahinter (siehe resizeEvent()).
    mainLayout->addStretch();

    writtenGrid = new WrittenGridWidget(this);
    writtenGrid->setVisible(false);
    writtenGrid->lower();
    connect(writtenGrid, &WrittenGridWidget::answerSubmitted, this, &TaskView::answerSubmitted);

    // --- Button-Reihe ganz unten, horizontal zentriert ---
    QHBoxLayout *bottomRow = new QHBoxLayout();
    bottomRow->addStretch();

    checkButton = new QPushButton("✓", this);
    checkButton->setObjectName("checkButton");
    checkButton->setMaximumWidth(44);
    bottomRow->addWidget(checkButton);
    connect(checkButton, &QPushButton::clicked, this, &TaskView::answerSubmitted);

    symbolMenuButton = new QPushButton("∑", this);
    symbolMenuButton->setObjectName("symbolMenuButton");
    bottomRow->addWidget(symbolMenuButton);
    connect(symbolMenuButton, &QPushButton::clicked, this, &TaskView::symbolMenuToggled);

    skipButton = new QPushButton("⏭", this);
    skipButton->setObjectName("skipButton");
    bottomRow->addWidget(skipButton);
    connect(skipButton, &QPushButton::clicked, this, &TaskView::skipRequested);

    continueButton = new QPushButton(tr("Weiter"), this);
    continueButton->setObjectName("continueButton");
    continueButton->setVisible(false);   // standardmäßig unsichtbar, nur bei autoAdvance=false gezeigt
    bottomRow->addWidget(continueButton);
    connect(continueButton, &QPushButton::clicked, this, &TaskView::continueRequested);

    bottomRow->addStretch();
    mainLayout->addLayout(bottomRow);   // KEIN addStretch() mehr danach!
}

void TaskView::setNumberFontFamily(const QString &family)
{
    writtenGrid->setNumberFont(family);
}

void TaskView::setInkColor(const QColor &color)
{
    writtenGrid->setInkColor(color);
}

void TaskView::insertSymbolAtFocus(const QString &symbol)
{
    writtenGrid->insertSymbolAtFocus(symbol);
}

void TaskView::showTask(const Task &task)
{
    writtenGrid->setVisible(true);
    writtenGrid->showCalculation(task.writtenCalculation);
}

QVector<QString> TaskView::currentAnswerTexts() const
{
    return { writtenGrid->currentAnswerText() };
}

void TaskView::showWorksheet(const QVector<Task> &tasks)
{
    writtenGrid->setVisible(true);
    writtenGrid->showWorksheet(tasks);
}

void TaskView::showAnswerColors(const QVector<bool> &correctness)
{
    writtenGrid->showAnswerColor(!correctness.isEmpty() && correctness.first());
}

void TaskView::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (writtenGrid) {
        // Gleicher linker Rand wie bei mainLayout (siehe Konstruktor, setContentsMargins(60, ...)) -
        // die Sidebar ist ein raised Overlay (kein Layout-Element) und liegt IMMER (auch
        // eingeklappt) ueber dem linken Rand von TaskView. Ohne diesen Rand zentriert das
        // Raster ueber die VOLLE Breite und Aufgaben/Antwortfelder landen teils dahinter -
        // dort sind sie unsichtbar UND nicht klickbar, weil die Sidebar die Klicks abfaengt.
        QRect gridArea = rect().adjusted(60, 0, 0, 0);
        writtenGrid->setGeometry(gridArea);
    }
}

void TaskView::showFeedbackText(const QString &text, bool correct)
{
    // feedbackLabel ist entfallen (siehe task_view.h-Kommentar) - der Rueckmeldetext
    // geht jetzt immer ans Raster (WrittenGridWidget::showSolution()), das ihn in
    // paintEvent() unterhalb der Aufgabe zeichnet.
    writtenGrid->showSolution(text, correct);
}

void TaskView::setInputEnabled(bool enabled)
{
    checkButton->setEnabled(enabled);
    writtenGrid->setInputEnabled(enabled);
}

void TaskView::setContinueButtonVisible(bool visible)
{
    continueButton->setVisible(visible);
}

void TaskView::focusFirstField()
{
    writtenGrid->focusFirstDigit();
}
