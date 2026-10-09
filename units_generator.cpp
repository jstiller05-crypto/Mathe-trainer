#include "units_generator.h"
#include "random_utils.h"
#include <algorithm>
#include <QVector>
#include <QDebug>

struct UnitConversion {
    QString fromUnit;
    QString toUnit;
    double factor;          // Multiplikator von "from" nach "to"
    int valueStep;          // Schrittweite fuer den "from"-Wert, so gewaehlt, dass factor*valueStep
                             // immer ein glattes Vielfaches ergibt (keine periodischen Dezimalzahlen)
    DifficultyLevel minLevel;   // ab welchem Level diese Umrechnung ueberhaupt vorkommt
};

// TODO (erledigt): vorher gab es nur 4 feste Umrechnungspaare und einen festen Wertebereich
// (10-200 in 10er-Schritten) UNABHAENGIG von level/mentalMath - generator-bench zeigte dadurch
// eine Duplikate-Rate von 84-95% ueber ALLE Level/Modi hinweg. Jetzt: mehr Paare, gestaffelt
// nach Level freigeschaltet (mehr Vielfalt in hoeheren Klassen), und ein level-/modusabhaengiger
// Wertebereich.
static const QVector<UnitConversion> &allConversions()
{
    static const QVector<UnitConversion> conversions = {
        // Laenge
        { "mm", "cm", 0.1,       10, 1 },
        { "cm", "mm", 10.0,       1, 1 },
        { "cm", "m",  0.01,      10, 1 },
        { "m",  "cm", 100.0,      1, 11 },   // ab Kl.4
        { "m",  "km", 0.001,    100, 21 },   // ab Kl.5
        { "km", "m",  1000.0,     1, 21 },
        // Gewicht
        { "g",  "kg", 0.001,     10, 1 },
        { "kg", "g",  1000.0,     1, 1 },
        { "kg", "t",  0.001,     10, 11 },
        { "mg", "g",  0.001,     10, 21 },
        // Zeit (Schrittweite 15 -> Ergebnis immer ein glattes Vielfaches von 0.25)
        { "min", "h",  1.0 / 60.0, 15, 11 },   // ab Kl.4
        { "s",   "min",1.0 / 60.0, 15, 21 },   // ab Kl.5
        // Volumen
        { "ml", "l",  0.001,     10, 21 },
    };
    return conversions;
}

Task generateUnitsTask(DifficultyLevel level, bool mentalMath)
{
    // Nur die fuer das aktuelle Level freigeschalteten Umrechnungspaare anbieten - so
    // bekommen hoehere Klassenstufen mehr Vielfalt (mehr Paare), ohne dass ein Drittklaessler
    // schon mit m<->km oder Zeiteinheiten konfrontiert wird.
    QVector<UnitConversion> available;
    for (const UnitConversion &conversion : allConversions()) {
        if (level >= conversion.minLevel) available.append(conversion);
    }

    const UnitConversion &chosen = available[randomInt(0, available.size() - 1)];

    // Wertebereich waechst mit dem Level, im Kopfrechnen-Modus vorsichtiger als im
    // Taschenrechner-/Schwere-Aufgabe-Modus (dort duerfen auch groessere Werte vorkommen,
    // weil das Ergebnis nicht mehr im Kopf entstehen muss). Gedeckelt (min(...)), analog zum
    // Kopfrechnen-Deckel bei Addition/Subtraktion, damit es im mentalMath-Fall nicht doch
    // unbegrenzt waechst.
    int maxSteps = mentalMath ? std::min(20 + level / 3, 60) : (20 + level);
    int value = randomInt(1, maxSteps) * chosen.valueStep;
    double result = value * chosen.factor;

    Task task;
    task.ruleName = "UnitConversion";
    task.promptText = QString("%1 %2 in %3 =").arg(value).arg(chosen.fromUnit).arg(chosen.toUnit);
    task.answers.append({ "", result });
    task.autoAdvance = mentalMath;

    // Kein Operanden-Schema (nur EIN Wert + Ziel-Einheit), deshalb SingleLine mit
    // "expression". Ergebnisse sind oft Kommazahlen (z.B. 30 cm in m = 0,3), daher
    // freeformAnswer statt Einzel-Ziffern-Kaestchen.
    QString answerStr = QString::number(result);
    task.writtenCalculation.expression = task.promptText;
    task.writtenCalculation.answerDigitCount = answerStr.length();
    task.writtenCalculation.freeformAnswer = answerStr.contains('.');
    task.writtenCalculation.mode = WrittenCalculation::DisplayMode::SingleLine;

    qDebug() << "[Units] mentalMath:" << mentalMath << "| verfuegbare Paare:" << available.size()
              << "| maxSteps:" << maxSteps << "|" << task.promptText;
    return task;
}
