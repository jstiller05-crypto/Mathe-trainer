#include "units_generator.h"
#include <cstdlib>
#include <QVector>
#include <QDebug>

struct UnitConversion {
    QString fromUnit;
    QString toUnit;
    double factor;   // Multiplikator von "from" nach "to"
};

Task generateUnitsTask(DifficultyLevel level)
{
    Q_UNUSED(level);   // TODO: eigene Level-Skalierung fuer Einheiten noch offen

    QVector<UnitConversion> conversions = {
        { "mm", "cm", 0.1 },
        { "cm", "m", 0.01 },
        { "g", "kg", 0.001 },
        { "kg", "t", 0.001 }
    };

    const UnitConversion &chosen = conversions[rand() % conversions.size()];
    int value = (rand() % 20 + 1) * 10;   // glatte Zahl fuer sauberes Ergebnis
    double result = value * chosen.factor;

    Task task;
    task.ruleName = "UnitConversion";
    task.promptText = QString("%1 %2 in %3 =").arg(value).arg(chosen.fromUnit).arg(chosen.toUnit);
    task.answers.append({ "", result });
    task.autoAdvance = true;

    // Jetzt wie alle anderen Aufgaben im Raster dargestellt statt in der alten
    // Label-Zeile (Punkt 4) - kein Operanden-Schema (nur EIN Wert + Ziel-Einheit),
    // deshalb SingleLine mit "expression". Ergebnisse sind oft Kommazahlen
    // (z.B. 30 cm in m = 0,3), daher freeformAnswer statt Einzel-Ziffern-Kaestchen.
    QString answerStr = QString::number(result);
    task.writtenCalculation.expression = task.promptText;
    task.writtenCalculation.answerDigitCount = answerStr.length();
    task.writtenCalculation.freeformAnswer = answerStr.contains('.');
    task.writtenCalculation.mode = WrittenCalculation::DisplayMode::SingleLine;

    qDebug() << "[Units] Standalone:" << task.promptText;
    return task;
}