#ifndef WRITTEN_GRID_WIDGET_H
#define WRITTEN_GRID_WIDGET_H

#include <QWidget>
#include <QVector>
#include <QLineEdit>
#include "task.h"

class WrittenGridWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WrittenGridWidget(QWidget *parent = nullptr);

    QString currentAnswerText() const;

    void showCalculation(const WrittenCalculation &calc);
    void setInputEnabled(bool enabled);
    void showAnswerColor(bool correct);
    void focusFirstDigit();
    void setNumberFont(const QString &family);

signals:
    void answerSubmitted();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    WrittenCalculation calculation;
    QVector<QLineEdit*> answerFields;
    QString numberFontFamily;

    int totalDigitColumns = 0;
    int pageRows = 0;
    double squareSize = 0;
    double gridXOffset = 0;
    double gridYOffset = 0;

    void recomputeLayout();
    void layoutAnswerFields();
};

#endif