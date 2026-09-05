#ifndef TASK_VIEW_H
#define TASK_VIEW_H

#include <QWidget>
#include <QPushButton>
#include <QVector>
#include <QColor>
#include "task.h"
#include "written_grid_widget.h"

// Zeigt IMMER genau eine Aufgabe im Karo-Raster (WrittenGridWidget) an - die frueher
// zusaetzlich vorhandene einzeilige Label-Darstellung (promptLabel/answerRowWidget)
// ist entfallen, seit auch Finanzen/Einheiten eine writtenCalculation.expression
// setzen und damit AUSNAHMSLOS jede Aufgabe ueber das Raster laeuft.
class TaskView : public QWidget
{
    Q_OBJECT

public:
    explicit TaskView(QWidget *parent = nullptr);

    void showTask(const Task &task);
    void showAnswerColors(const QVector<bool> &correctness);
    void showFeedbackText(const QString &text, bool correct);
    void setInputEnabled(bool enabled);
    void focusFirstField();
    void setContinueButtonVisible(bool visible);
    void setNumberFontFamily(const QString &family);
    void setInkColor(const QColor &color);
    void insertSymbolAtFocus(const QString &symbol);
    void showWorksheet(const QVector<Task> &tasks);

    QVector<QString> currentAnswerTexts() const;

signals:
    void answerSubmitted();
    void symbolMenuToggled();
    void skipRequested();
    void continueRequested();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QPushButton *checkButton;
    QPushButton *symbolMenuButton;
    QPushButton *skipButton;
    QPushButton *continueButton;

    WrittenGridWidget *writtenGrid;
};

#endif
