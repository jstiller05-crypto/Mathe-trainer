#include "addition_subtraction_generator.h"
#include <cstdlib>
#include <QDebug>

// Eigene, LOKALE Level->Zahlenbereich-Umrechnung - NUR fuer diesen Generator.
// Andere Generatoren duerfen ihre EIGENE Formel benutzen (siehe Roadmap-Entscheidung).
// Level 1 -> Zahlen bis ~40, Level 100 -> Zahlen bis ~2020.
static int maxNumberForLevel(DifficultyLevel level)
{
    return 20 + level * 20;
}

// Ab wann duerfen Ergebnisse negativ werden? Laut Lehrplan ab Kl.6 (~Level 31).
static bool negativeResultsAllowed(DifficultyLevel level)
{
    return level >= 31;
}

Task generateAdditionSubtractionTask(DifficultyLevel level)
{
    int maxNumber = maxNumberForLevel(level);
    int first = rand() % maxNumber + 1;
    bool isAddition = (rand() % 2 == 0);

    int second;
    if (isAddition) {
        second = rand() % maxNumber + 1;
    } else {
        // Solange negative Ergebnisse noch nicht erlaubt sind: zweiter Wert <= erster Wert
        second = negativeResultsAllowed(level) ? (rand() % maxNumber + 1) : (rand() % (first + 1));
    }

    double result = isAddition ? (first + second) : (first - second);

    Task task;
    task.ruleName = "AdditionSubtraction";
    task.promptText = QString("%1 %2 %3 =").arg(first).arg(isAddition ? "+" : "-").arg(second);
    task.answers.append({ "", result });
    task.autoAdvance = true;

    qDebug() << "[AdditionSubtraction] Standalone:" << task.promptText << "=" << result;
    return task;
}

TaskFragment generateAdditionSubtractionFragment(DifficultyLevel level)
{
    int maxNumber = maxNumberForLevel(level);
    int value = rand() % maxNumber + 1;

    TaskFragment fragment;
    fragment.value = value;
    fragment.display = QString::number(value);
    return fragment;
}

Task combineWithAdditionSubtraction(const TaskFragment &first, DifficultyLevel level)
{
    int maxNumber = maxNumberForLevel(level);
    int second = rand() % maxNumber + 1;
    bool isAddition = (rand() % 2 == 0);

    double result = isAddition ? (first.value + second) : (first.value - second);

    Task task;
    task.ruleName = "AdditionSubtraction (kombiniert)";
    task.promptText = QString("%1 %2 %3 =").arg(first.display).arg(isAddition ? "+" : "-").arg(second);
    task.answers.append({ "", result });
    task.autoAdvance = true;

    qDebug() << "[AdditionSubtraction] Kombiniert:" << task.promptText << "=" << result;
    return task;
}