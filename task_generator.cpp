// task_generator.cpp
#include "task_generator.h"
#include "arithmetic_unit.h"
#include <QDebug>

Task generateTask(DifficultyLevel level, const QVector<QPair<QString, QString>> &activeSelections, bool mentalMath)
{
    QStringList arithmeticSubs;
    for (const auto &selection : activeSelections) {
        if (selection.first == "Arithmetik") arithmeticSubs.append(selection.second);
    }

    if (!arithmeticSubs.isEmpty()) {
        return generateArithmeticTask(level, arithmeticSubs, mentalMath);
    }

    qWarning() << "[TaskGenerator] Keine unterstuetzte Kategorie aktiv - Fallback";
    return generateArithmeticTask(level, { "Addition & Subtraktion" }, mentalMath);
}