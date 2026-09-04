#include "task_view.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QApplication>

TaskView::TaskView(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(24);
    mainLayout->setContentsMargins(60, 40, 60, 40);
    mainLayout->addStretch();

    promptLabel = new QLabel("", this);
    promptLabel->setObjectName("taskLabel");
    promptLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(promptLabel);

    answerRowWidget = new QWidget(this);
    new QHBoxLayout(answerRowWidget);   // Layout wird erzeugt, aber vorerst leer - Felder kommen dynamisch dazu
    answerRowWidget->layout()->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(answerRowWidget);

    // task_view.cpp - Konstruktor, ERSETZE den Teil ab "checkButton = ..." bis zum Ende

    feedbackLabel = new QLabel("", this);
    feedbackLabel->setObjectName("feedbackLabel");
    feedbackLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(feedbackLabel);

    writtenGrid = new WrittenGridWidget(this);
    writtenGrid->setVisible(false);
    writtenGrid->lower();
    connect(writtenGrid, &WrittenGridWidget::answerSubmitted, this, &TaskView::answerSubmitted);

    mainLayout->addStretch();   // EINMAL hier - schiebt alles Folgende ganz nach unten

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

    // task_view.cpp - in der Button-Reihe (bottomRow) ergänzen, neben checkButton/symbolMenuButton

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

void TaskView::insertSymbolAtFocus(const QString &symbol)
{
    QLineEdit *focused = qobject_cast<QLineEdit*>(QApplication::focusWidget());

    if (focused && answerFields.contains(focused)) {
        focused->insert(symbol);
    } else if (!answerFields.isEmpty()) {
        answerFields.first()->insert(symbol);   // Fallback, falls kein Feld fokussiert ist
    }
}

void TaskView::showTask(const Task &task)
{
    bool isWritten = !task.writtenCalculation.operands.isEmpty();

    promptLabel->setVisible(!isWritten);
    answerRowWidget->setVisible(!isWritten);
    writtenGrid->setVisible(isWritten);

    if (isWritten) {
        writtenGrid->showCalculation(task.writtenCalculation);
    } else {
        promptLabel->setText(task.promptText);
        rebuildAnswerFields(task.answers);
    }

    feedbackLabel->setText("");
}

void TaskView::rebuildAnswerFields(const QVector<AnswerSlot> &answerSlots)   // <- umbenannt
{
    for (QLineEdit *field : answerFields) {
        field->deleteLater();
    }
    answerFields.clear();

    QHBoxLayout *layout = qobject_cast<QHBoxLayout*>(answerRowWidget->layout());

    for (const AnswerSlot &slot : answerSlots) {   // <- hier auch anpassen
        QVBoxLayout *column = new QVBoxLayout();

        if (!slot.label.isEmpty()) {
            QLabel *labelWidget = new QLabel(slot.label, answerRowWidget);
            labelWidget->setAlignment(Qt::AlignCenter);
            column->addWidget(labelWidget);
        }

        QLineEdit *field = new QLineEdit(answerRowWidget);
        field->setObjectName("answerEdit");
        field->setAlignment(Qt::AlignCenter);
        field->installEventFilter(this);
        column->addWidget(field);

        layout->addLayout(column);
        answerFields.append(field);
    }
}

QVector<QString> TaskView::currentAnswerTexts() const
{
    if (writtenGrid->isVisible()) {
        return { writtenGrid->currentAnswerText() };
    }
    QVector<QString> texts;
    for (QLineEdit *field : answerFields) texts.append(field->text());
    return texts;
}

void TaskView::showWorksheet(const QVector<WrittenCalculation> &calculations)
{
    promptLabel->setVisible(false);
    answerRowWidget->setVisible(false);
    writtenGrid->setVisible(true);
    writtenGrid->showWorksheet(calculations);
}

void TaskView::showAnswerColors(const QVector<bool> &correctness)
{
    if (writtenGrid->isVisible()) {
        writtenGrid->showAnswerColor(!correctness.isEmpty() && correctness.first());
        return;
    }
    for (int i = 0; i < answerFields.size() && i < correctness.size(); ++i) {
        QString color = correctness[i] ? "#2ecc71" : "#e74c3c";
        answerFields[i]->setStyleSheet(QString("border: 2px solid %1;").arg(color));
    }
}

void TaskView::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (writtenGrid) {
        writtenGrid->setGeometry(rect());   // füllt IMMER die komplette TaskView-Flaeche
    }
}

void TaskView::showFeedbackText(const QString &text)
{
    feedbackLabel->setText(text);
}

void TaskView::setInputEnabled(bool enabled)
{
    checkButton->setEnabled(enabled);
    writtenGrid->setInputEnabled(enabled);
    for (QLineEdit *field : answerFields) field->setEnabled(enabled);
}

void TaskView::setContinueButtonVisible(bool visible)
{
    continueButton->setVisible(visible);
}

void TaskView::focusFirstField()
{
    if (writtenGrid->isVisible()) {
        writtenGrid->focusFirstDigit();
    } else if (!answerFields.isEmpty()) {
        answerFields.first()->setFocus();
    }
}

void TaskView::focusNextField(QLineEdit *current)
{
    int index = answerFields.indexOf(current);
    if (index == -1) return;

    if (index + 1 < answerFields.size()) {
        answerFields[index + 1]->setFocus();   // zum nächsten Feld springen
    } else {
        emit answerSubmitted();   // letztes Feld -> Aufgabe wird abgeschickt
    }
}

bool TaskView::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        auto *keyEvent = static_cast<QKeyEvent*>(event);
        auto *field = qobject_cast<QLineEdit*>(watched);

        if (field && (keyEvent->key() == Qt::Key_Tab || keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)) {
            focusNextField(field);
            return true;   // Standard-Tab-Verhalten (Fokus-Reihenfolge des OS) unterdrücken, wir steuern selbst
        }
    }
    return QWidget::eventFilter(watched, event);
}