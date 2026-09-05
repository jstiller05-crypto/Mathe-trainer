#ifndef WRITTEN_GRID_WIDGET_H
#define WRITTEN_GRID_WIDGET_H

#include <QWidget>
#include <QVector>
#include <QLineEdit>
#include <QColor>
#include "task.h"

class WrittenGridWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WrittenGridWidget(QWidget *parent = nullptr);

    void showCalculation(const WrittenCalculation &calc);
    void showWorksheet(const QVector<Task> &tasks);
    void setNumberFont(const QString &family);
    void setInkColor(const QColor &color);

    QString currentAnswerText() const;
    void setInputEnabled(bool enabled);
    void showAnswerColor(bool correct);
    void showSolution(const QString &text, bool correct);
    void focusFirstDigit();
    void insertSymbolAtFocus(const QString &symbol);

signals:
    void answerSubmitted();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    WrittenCalculation calculation;
    QVector<Task> worksheetTasks;
    QVector<QLineEdit*> answerFields;
    QString numberFontFamily;
    QColor inkColor = Qt::white;   // Vorgabewert wie bisher, bis MainWindow setInkColor() aufruft
    QString solutionText;           // Rueckmeldetext (Punkt 4), leer = nichts anzeigen
    bool solutionCorrect = true;    // steuert Ink- vs. Fehlerfarbe fuer solutionText

    int totalDigitColumns = 0;
    int pageRows = 0;
    double squareSize = 0;
    double gridXOffset = 0;
    double gridYOffset = 0;

    void recomputeLayout();
    void layoutAnswerFields();
    void placeFreeformField(int startCol, int rowIndex, double cellWidth, double cellHeight);
};

#endif