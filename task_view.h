#ifndef TASK_VIEW_H
#define TASK_VIEW_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVector>
#include "task.h"
#include "written_grid_widget.h"

class TaskView : public QWidget
{
    Q_OBJECT

public:
    explicit TaskView(QWidget *parent = nullptr);

    void showTask(const Task &task);
    void showAnswerColors(const QVector<bool> &correctness);
    void showFeedbackText(const QString &text);
    void setInputEnabled(bool enabled);
    void focusFirstField();
    void setContinueButtonVisible(bool visible);
    void setNumberFontFamily(const QString &family);
    void insertSymbolAtFocus(const QString &symbol);

    QVector<QString> currentAnswerTexts() const;

signals:
    void answerSubmitted();
    void symbolMenuToggled();
    void skipRequested();
    void continueRequested();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QLabel *promptLabel;
    QWidget *answerRowWidget;
    QVector<QLineEdit*> answerFields;
    QLabel *feedbackLabel;
    QPushButton *checkButton;
    QPushButton *symbolMenuButton;
    QPushButton *skipButton;
    QPushButton *continueButton;

    void rebuildAnswerFields(const QVector<AnswerSlot> &answerSlots);
    void focusNextField(QLineEdit *current);

    WrittenGridWidget *writtenGrid;
};

#endif