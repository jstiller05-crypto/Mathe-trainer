#include "rectangle_generator.h"
#include "geometry_model.h"
#include <cstdlib>
#include <QDebug>

// Eigene, bewusst kleine Umrechnung (wie jeder Generator, siehe CLAUDE.md) -
// orientiert an maxFactorForLevel() in percent_mult_div_generator.cpp (Rechteck-
// Flaeche ist im Kern eine Multiplikation, deshalb aehnliche Groessenordnung).
// Seitenlaengen bleiben bewusst ein- bis niedrig-zweistellig, damit Flaeche UND
// Umfang fuer Kl.4-5 noch im Kopf bzw. auf Papier loesbar sind.
static int maxSideForLevel(DifficultyLevel level, bool mentalMath)
{
    int base = 4 + level / 4;
    return mentalMath ? base : base + 4;
}

// WICHTIG: writtenCalculation (die Karo-Raster-Darstellung, siehe task.h) bleibt bei
// Geometrie-Aufgaben aktuell komplett LEER. Das Zeichnen einer Geometrie-Aufgabe
// (ueber GeometryModel statt eines Text-Ausdrucks) ist laut Aufgabenstellung ein
// SPAETERER Schritt - TaskView wuerde mit einem leeren writtenCalculation aktuell
// einfach nichts anzeigen (kein Crash, siehe WrittenGridWidget::recomputeLayout()),
// das reicht fuer diesen Zwischenstand, der nur ueber qDebug()/Konsole geprueft wird.

// Variante A: Laenge und Breite sind GEGEBEN (feste Labels an den entsprechenden
// Kanten), GESUCHT sind Flaeche UND Umfang (zwei Antwortfelder).
static Task buildLengthWidthGivenTask(int length, int width, bool mentalMath)
{
    int area = length * width;
    int perimeter = 2 * (length + width);

    Task task;
    task.ruleName = "RectangleAreaPerimeter";
    task.promptText = QString("Rechteck: Länge %1 cm, Breite %2 cm - Fläche und Umfang?").arg(length).arg(width);
    task.answers.append({ "Fläche", static_cast<double>(area) });
    task.answers.append({ "Umfang", static_cast<double>(perimeter) });
    task.autoAdvance = mentalMath;

    // GeometryModel: 4 Eckpunkte eines Rechtecks in einem neutralen, NICHT
    // massstabsgetreuen Koordinatensystem (siehe geometry_model.h - egal wie schmal
    // oder breit das ECHTE Rechteck ist, hier steht immer dasselbe handliche 4x2-
    // Layout). Die echten Masse (length/width) stehen ausschliesslich in den Labels.
    GeometryModel model;
    model.points = { QPointF(0, 0), QPointF(4, 0), QPointF(4, 2), QPointF(0, 2) };
    model.edges = {
        { 0, 1 },   // untere Kante - Laenge
        { 1, 2 },   // rechte Kante - Breite
        { 2, 3 },   // obere Kante (Laenge, unbeschriftet - Gegenkante ist ja schon markiert)
        { 3, 0 }    // linke Kante (Breite, unbeschriftet)
    };
    model.labels = {
        { QPointF(2, 0), QString("%1 cm").arg(length), -1 },   // Mitte der unteren Kante, fester Text
        { QPointF(4, 1), QString("%1 cm").arg(width), -1 },     // Mitte der rechten Kante, fester Text
    };
    task.geometryModel = model;

    qDebug() << "[Rectangle] Variante 'Laenge+Breite gegeben':" << length << "x" << width
             << "cm | Flaeche:" << area << "cm² | Umfang:" << perimeter << "cm"
             << "| Modell-Punkte:" << model.points.size() << "| Kanten:" << model.edges.size()
             << "| Labels:" << model.labels.size();

    return task;
}

// Variante B: EINE Seite plus die Flaeche sind gegeben, GESUCHT ist die andere Seite.
static Task buildSideAndAreaGivenTask(int knownSide, int area, int soughtSide, bool mentalMath)
{
    Task task;
    task.ruleName = "RectangleMissingSide";
    task.promptText = QString("Rechteck: eine Seite %1 cm, Fläche %2 cm² - andere Seite?").arg(knownSide).arg(area);
    task.answers.append({ "gesuchte Seite", static_cast<double>(soughtSide) });
    task.autoAdvance = mentalMath;

    GeometryModel model;
    model.points = { QPointF(0, 0), QPointF(4, 0), QPointF(4, 2), QPointF(0, 2) };
    model.edges = {
        { 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 0 }
    };
    // Die bekannte Seite bekommt einen festen Text, die GESUCHTE Seite bekommt
    // stattdessen einen Verweis auf task.answers[0] (answerIndex = 0) statt eines
    // festen Textes - so weiss die (spaetere) Zeichnung, WELCHE Kante als "gesucht"
    // markiert werden muss, ohne den Text selbst zu kennen.
    model.labels = {
        { QPointF(2, 0), QString("%1 cm").arg(knownSide), -1 },
        { QPointF(4, 1), QString(), 0 },
    };
    task.geometryModel = model;

    qDebug() << "[Rectangle] Variante 'Seite+Flaeche gegeben': bekannte Seite" << knownSide
             << "cm | Flaeche:" << area << "cm² | gesuchte Seite:" << soughtSide << "cm"
             << "| Modell-Punkte:" << model.points.size() << "| Kanten:" << model.edges.size()
             << "| Labels:" << model.labels.size();

    return task;
}

Task generateRectangleTask(DifficultyLevel level, bool mentalMath)
{
    int maxSide = maxSideForLevel(level, mentalMath);
    bool lengthWidthVariant = (rand() % 2 == 0);

    qDebug() << "[Rectangle] Level:" << level << "| mentalMath:" << mentalMath
             << "| Variante:" << (lengthWidthVariant ? "Laenge+Breite gegeben" : "Seite+Flaeche gegeben")
             << "| maxSide:" << maxSide;

    if (lengthWidthVariant) {
        // Mindestens 2 statt 1 - ein Rechteck mit Seitenlaenge 1 wirkt beim spaeteren
        // Zeichnen eher wie ein Strich als wie eine echte Flaeche.
        int length = rand() % maxSide + 2;
        int width = rand() % maxSide + 2;
        return buildLengthWidthGivenTask(length, width, mentalMath);
    }

    int knownSide = rand() % maxSide + 2;
    int soughtSide = rand() % maxSide + 2;
    int area = knownSide * soughtSide;
    return buildSideAndAreaGivenTask(knownSide, area, soughtSide, mentalMath);
}
