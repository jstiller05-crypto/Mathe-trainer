#ifndef WRITTEN_GRID_WIDGET_H
#define WRITTEN_GRID_WIDGET_H

#include <QWidget>
#include <QVector>
#include <QLineEdit>
#include <QColor>
#include "task.h"
#include "geometry_model.h"

class QPainter;   // Vorwaertsdeklaration reicht - <QPainter> wird nur in der .cpp gebraucht

class WrittenGridWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WrittenGridWidget(QWidget *parent = nullptr);

    void showCalculation(const WrittenCalculation &calc);
    void showWorksheet(const QVector<Task> &tasks);
    // Analog zu showCalculation()/showWorksheet(): dritter Anzeige-Modus, zeichnet ein
    // GeometryModel (siehe geometry_model.h) statt Rechen-Kaestchen. Die urspruenglich
    // eigenstaendige GeometryModelWidget-Klasse wurde bewusst NICHT als zweites
    // Widget behalten - die App soll genau EIN Frontend-Widget fuer alle Aufgaben-
    // Darstellungen haben, die Skalierungs-/Zeichenlogik ist stattdessen hier als
    // private Hilfsfunktionen eingezogen (siehe recomputeGeometryLayout() unten).
    void showModel(const GeometryModel &model);
    void setNumberFont(const QString &family);
    void setInkColor(const QColor &color);

    QString currentAnswerText() const;
    // Plural-Pendants fuer mehrere UNABHAENGIGE Antworten (z.B. Flaeche UND Umfang bei
    // einer Geometrie-Aufgabe) - currentAnswerText()/showAnswerColor() bleiben bestehen
    // und werden fuer den Rechen-Modus intern weiterhin genutzt (siehe .cpp), weil dort
    // ALLE answerFields zusammen EINE Antwort bilden. Im Geometrie-Modus liefert/erwartet
    // diese Variante dagegen einen Eintrag PRO GeometryLabel::answerIndex.
    QVector<QString> currentAnswerTexts() const;
    void setInputEnabled(bool enabled);
    void showAnswerColor(bool correct);
    void showAnswerColors(const QVector<bool> &correctness);
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
    // Bei Geometrie-Anzeige (siehe showModel()) das aktuelle Modell - leer (points.isEmpty())
    // bedeutet "kein Geometrie-Modus aktiv", analog dazu, wie totalDigitColumns==0 bzw.
    // worksheetTasks.isEmpty() die jeweils anderen Modi deaktiviert anzeigen.
    GeometryModel geometryModel;
    QVector<QLineEdit*> answerFields;   // je nach aktivem Modus: Ziffern-Kaestchen ODER Geometrie-Antwortfelder
    QString numberFontFamily;
    QColor inkColor = Qt::white;   // Vorgabewert wie bisher, bis MainWindow setInkColor() aufruft
    QString solutionText;           // Rueckmeldetext (Punkt 4), leer = nichts anzeigen
    bool solutionCorrect = true;    // steuert Ink- vs. Fehlerfarbe fuer solutionText

    int totalDigitColumns = 0;
    int pageRows = 0;
    double squareSize = 0;
    double gridXOffset = 0;
    double gridYOffset = 0;

    // Skalierung/Verschiebung NUR fuer den Geometrie-Modus - bildet die (nicht
    // massstabsgetreue, siehe geometry_model.h) Bounding-Box aus geometryModel
    // seitenverhaeltnistreu auf die Widget-Flaeche ab (uebernommen aus der urspruenglich
    // eigenstaendigen GeometryModelWidget-Klasse, siehe Kommentar bei showModel()).
    double geometryScale = 1.0;
    double geometryOffsetX = 0.0;
    double geometryOffsetY = 0.0;
    double geometryMinX = 0.0;
    double geometryMinY = 0.0;

    void recomputeLayout();
    void layoutAnswerFields();
    void placeFreeformField(int startCol, int rowIndex, double cellWidth, double cellHeight);

    void recomputeGeometryLayout();
    void layoutGeometryAnswerFields();
    void paintGeometryModel(QPainter &painter);
    QPointF mapModelToWidget(const QPointF &modelPoint) const;
};

#endif