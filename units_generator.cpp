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

    Task task;
    task.ruleName = "UnitConversion";
    task.promptText = QString("%1 %2 = ? %3").arg(value).arg(chosen.fromUnit).arg(chosen.toUnit);
    task.answers.append({ "", value * chosen.factor });
    task.autoAdvance = true;

    qDebug() << "[Units] Standalone:" << task.promptText;
    return task;
}