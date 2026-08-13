#include "task_view.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

TaskView::TaskView(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(24);
    mainLayout->setContentsMargins(60, 40, 60, 40);

    mainLayout->addStretch();

    taskLabel = new QLabel("Task", this);
    taskLabel->setObjectName("taskLabel");
    taskLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(taskLabel);

    QHBoxLayout *answerRow = new QHBoxLayout();
    answerRow->addStretch();

    answerEdit = new QLineEdit(this);
    answerEdit->setObjectName("answerEdit");
    answerEdit->setAlignment(Qt::AlignCenter);
    answerRow->addWidget(answerEdit);

    checkButton = new QPushButton("✓", this);
    checkButton->setObjectName("checkButton");
    checkButton->setMaximumWidth(44);
    answerRow->addWidget(checkButton);

    answerRow->addStretch();
    mainLayout->addLayout(answerRow);

    feedbackLabel = new QLabel("", this);
    feedbackLabel->setObjectName("feedbackLabel");
    feedbackLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(feedbackLabel);

    mainLayout->addStretch();

    connect(checkButton, &QPushButton::clicked, this, &TaskView::answerSubmitted);
    connect(answerEdit, &QLineEdit::returnPressed, this, &TaskView::answerSubmitted);
}

void TaskView::showQuestion(const QString &questionText)
{
    taskLabel->setText(questionText);
    answerEdit->clear();
    feedbackLabel->setText("");
}

void TaskView::showFeedback(const QString &feedbackText)
{
    feedbackLabel->setText(feedbackText);
}

void TaskView::setInputEnabled(bool enabled)
{
    checkButton->setEnabled(enabled);
    answerEdit->setEnabled(enabled);
}

void TaskView::focusAnswerField()
{
    answerEdit->setFocus();
}

QString TaskView::currentAnswerText() const
{
    return answerEdit->text();
}