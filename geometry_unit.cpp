#include "geometry_unit.h"
#include "rectangle_generator.h"
#include "random_utils.h"
#include <QDebug>

// Waehlt die passende Speiche fuer eine Unterkategorie - aktuell nur "Rechteck",
// wird mit jedem neuen Geometrie-Generator (Dreieck, Kreis, Koerper, ...) erweitert,
// genau wie standaloneForSubcategory() in arithmetic_unit.cpp.
static Task standaloneForSubcategory(const QString &subcategory, DifficultyLevel level, bool smallNumbers)
{
    if (subcategory == "Rechteck") return generateRectangleTask(level, smallNumbers);

    qWarning() << "[GeometryUnit] Unbekannte Unterkategorie:" << subcategory << "- Fallback auf Rechteck";
    return generateRectangleTask(level, smallNumbers);
}

Task generateGeometryTask(DifficultyLevel level, const QStringList &activeSubcategories, TaskMode mode)
{
    // Wie in arithmetic_unit.cpp: smallNumbers steuert die Zahlengroesse (nur bei
    // Kopfrechnen klein/rund). Eine Verschmelzung mehrerer Formen zu einer Kette
    // (wie bei Arithmetik-Fragmenten) gibt es fuer Geometrie noch nicht - dafuer
    // braeuchte es mehrere KOMBINIERBARE Formen, aktuell existiert nur das Rechteck.
    bool smallNumbers = (mode == TaskMode::MentalMath);

    QStringList subcategories = activeSubcategories;
    if (subcategories.isEmpty()) {
        subcategories = geometryAvailableSubcategories(level);
        qDebug() << "[GeometryUnit] Keine Unterkategorie aktiv - nutze alle verfuegbaren:" << subcategories;
    }

    if (subcategories.isEmpty()) {
        // Noch keine einzige Unterkategorie fuer dieses Level verfuegbar (Level unter
        // RectangleCriteria::MinLevel) - in diesem fruehen Ausbaustadium mit nur EINEM
        // Generator faellt das trotzdem auf "Rechteck" zurueck, statt mit einer leeren
        // Liste abzustuerzen (randomInt(0, -1) waere ein ungueltiger Bereich).
        // Sobald es mehrere Formen gibt, sollte diese Stelle ueberdacht werden.
        qWarning() << "[GeometryUnit] Keine Unterkategorie fuer Level" << level << "verfuegbar - Fallback auf Rechteck";
        subcategories << "Rechteck";
    }

    qDebug() << "[GeometryUnit] Aktive Unterkategorien:" << subcategories << "| Modus:" << static_cast<int>(mode);

    QString chosen = subcategories[randomInt(0, subcategories.size() - 1)];
    qDebug() << "[GeometryUnit] Gewaehlte Form:" << chosen;

    Task task = standaloneForSubcategory(chosen, level, smallNumbers);

    qDebug() << "[GeometryUnit] FERTIGE AUFGABE:" << task.promptText
             << "| Antwortfelder:" << task.answers.size()
             << "| Modell-Punkte:" << (task.geometryModel ? task.geometryModel->points.size() : 0);

    return task;
}

QStringList geometryAvailableSubcategories(DifficultyLevel level)
{
    QStringList available;
    if (level >= RectangleCriteria::MinLevel) available << "Rechteck";

    qDebug() << "[GeometryUnit] Verfuegbare Unterkategorien bei Level" << level << ":" << available;
    return available;
}

// Anders als runCombineSelfTest() (arithmetic_unit.cpp) KEIN Determinismus-Test mit
// fest erwarteten Werten - die Geometrie-Generatoren wuerfeln echte Zufallswerte,
// "die richtige Antwort" ist also von Aufruf zu Aufruf verschieden. Stattdessen ein
// STRUKTURELLER Rauchtest: erzeugt ein paar Aufgaben und prueft, dass die Grunddaten
// in sich stimmig sind (Modell vorhanden, 4 Punkte/Kanten fuers Rechteck, mindestens
// ein Antwortfeld, nicht-leerer Aufgabentext). Die qDebug()-Ausgaben aus
// generateGeometryTask()/generateRectangleTask() machen dabei nebenbei jede einzelne
// erzeugte Aufgabe in der Konsole nachvollziehbar - genau das war hier gefordert.
void runGeometrySelfTest()
{
    for (int i = 0; i < 5; ++i) {
        Task task = generateGeometryTask(RectangleCriteria::MinLevel, { "Rechteck" }, TaskMode::MentalMath);

        Q_ASSERT(task.geometryModel.has_value());
        Q_ASSERT(task.geometryModel->points.size() == 4);
        Q_ASSERT(task.geometryModel->edges.size() == 4);
        Q_ASSERT(!task.answers.isEmpty());
        Q_ASSERT(!task.promptText.isEmpty());
    }

    qDebug() << "[GeometryUnit] Selbsttest erfolgreich (5 Rechteck-Aufgaben strukturell geprueft).";
}
