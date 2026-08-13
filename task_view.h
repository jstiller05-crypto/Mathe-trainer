#ifndef TASK_VIEW_H
#define TASK_VIEW_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

// Zeigt eine Aufgabe an und lässt den Nutzer eine Antwort eingeben.
// Weiß NICHTS über SessionController, Difficulty oder wie eine Aufgabe entsteht.
class TaskView : public QWidget
{
    Q_OBJECT

public:
    explicit TaskView(QWidget *parent = nullptr);

    void showQuestion(const QString &questionText);
    void showFeedback(const QString &feedbackText);
    void setInputEnabled(bool enabled);
    void focusAnswerField();
    QString currentAnswerText() const;

signals:
    void answerSubmitted();   // Nutzer hat Enter gedrückt oder auf Check geklickt

private:
    QLabel *taskLabel;
    QLineEdit *answerEdit;
    QPushButton *checkButton;
    QLabel *feedbackLabel;
};

#endif
