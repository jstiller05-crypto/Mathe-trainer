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

    void showCalculation(const WrittenCalculation &calc);
    QString currentAnswerText() const;
    void setInputEnabled(bool enabled);
    void showAnswerColor(bool correct);
    void focusFirstDigit();

signals:
    void answerSubmitted();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    WrittenCalculation calculation;
    QVector<QLineEdit*> answerFields;

    int totalDigitColumns = 0;
    int lineRowIndex = 0;
    int totalDataRows = 0;

    // Alles hier ist relativ zur AKTUELLEN Widget-Groesse berechnet - keine festen Pixelwerte
    double cellWidth = 0;
    double cellHeight = 0;   // IMMER cellWidth * 2 - so bleibt das "Karo-Verhaeltnis" (Ziffer 2x so hoch wie breit) erhalten
    double gridXOffset = 0;
    double gridYOffset = 0;
    QVector<double> rowYOffsets;

    void recomputeLayout();
    void layoutAnswerFields();
};

#endif