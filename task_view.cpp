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

    // Geometrie-Aufgaben (siehe task.h: Task::geometryModel) bekommen ihre eigene
    // Darstellung ueber WrittenGridWidget::showModel() statt der Rechen-Kaestchen -
    // die Weiche liegt bewusst hier (statt z.B. in SessionController), weil es reine
    // Anzeige-Logik ist und TaskView schon jetzt die einzige Stelle ist, die
    // writtenGrid kennt.
    if (task.geometryModel) {
        writtenGrid->showModel(*task.geometryModel);
    } else {
        writtenGrid->showCalculation(task.writtenCalculation);
    }
}

QVector<QString> TaskView::currentAnswerTexts() const
{
    // Direkt durchgereicht statt auf einen Einzelwert reduziert - WrittenGridWidget
    // liefert je nach Modus entweder einen 1-elementigen Vektor (Rechen-Modus) oder
    // mehrere unabhaengige Antworten (Geometrie-Modus, siehe dortiger Kommentar).
    return writtenGrid->currentAnswerTexts();
}

void TaskView::showWorksheet(const QVector<Task> &tasks)
{
    writtenGrid->setVisible(true);
    writtenGrid->showWorksheet(tasks);
}

void TaskView::showAnswerColors(const QVector<bool> &correctness)
{
    writtenGrid->showAnswerColors(correctness);
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

    // Nach einer falschen Antwort deaktiviert setInputEnabled(false) die Antwortfelder -
    // deaktivierte Widgets bekommen aber keine Tastenereignisse mehr, der Fokus landet
    // sonst undefiniert irgendwo. Der Weiter-Button haelt jetzt selbst den Fokus, damit
    // Enter (QPushButton loest bei Fokus+Enter automatisch clicked() aus) direkt
    // weiterschaltet - komplett ohne Maus bedienbar.
    if (visible) continueButton->setFocus();
}

void TaskView::focusFirstField()
{
    writtenGrid->focusFirstDigit();
}
