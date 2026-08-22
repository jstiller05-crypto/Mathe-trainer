#include "addition_subtraction_generator.h"
#include <cstdlib>
#include <QDebug>

static int maxNumberForLevel(DifficultyLevel level)
{
    return 20 + level * 20;
}

static bool negativeResultsAllowed(DifficultyLevel level)
{
    return level >= 31;
}

// Erzeugt eine Zahl bis maxValue - bevorzugt KLEINE Werte, laesst groessere
// aber gelegentlich zu. Zusaetzlich: je mehr Stellen die Zahl hat, desto
// "runder" wird sie DIREKT bei der Erzeugung - keine nachtraegliche Pruefung.
static int generateMentalMathFriendlyNumber(int maxValue)
{
    double randomFraction = static_cast<double>(rand()) / RAND_MAX;
    double biasedFraction = randomFraction * randomFraction;
    int rawValue = static_cast<int>(biasedFraction * maxValue) + 1;

    if (rawValue >= 1000) return (rawValue / 10) * 10;
    if (rawValue >= 100)  return (rawValue / 5) * 5;
    return rawValue;
}

Task generateAdditionSubtractionTask(DifficultyLevel level, bool mentalMath)
{
    int maxNumber = mentalMath ? maxNumberForLevel(level) : maxNumberForLevel(level) * 5;

    int first = mentalMath ? generateMentalMathFriendlyNumber(maxNumber) : (rand() % maxNumber + 1);
    bool isAddition = (rand() % 2 == 0);

    int second;
    if (isAddition) {
        second = mentalMath ? generateMentalMathFriendlyNumber(maxNumber) : (rand() % maxNumber + 1);
    } else {
        int upperBound = negativeResultsAllowed(level) ? maxNumber : first;
        second = mentalMath ? generateMentalMathFriendlyNumber(upperBound) : (rand() % (upperBound + 1));
    }

    double result = isAddition ? (first + second) : (first - second);

    Task task;
    task.ruleName = "AdditionSubtraction";
    task.promptText = QString("%1 %2 %3 =").arg(first).arg(isAddition ? "+" : "-").arg(second);
    task.answers.append({ "", result });
    task.autoAdvance = mentalMath;

    qDebug() << "[AdditionSubtraction] mentalMath:" << mentalMath << "|" << task.promptText;
    return task;
}

TaskFragment generateAdditionSubtractionFragment(DifficultyLevel level, bool mentalMath)
{
    int maxNumber = mentalMath ? maxNumberForLevel(level) : maxNumberForLevel(level) * 5;
    int value = mentalMath ? generateMentalMathFriendlyNumber(maxNumber) : (rand() % maxNumber + 1);

    TaskFragment fragment;
    fragment.value = value;
    fragment.display = QString::number(value);
    return fragment;
}

Task combineWithAdditionSubtraction(const TaskFragment &first, DifficultyLevel level, bool mentalMath)
{
    int maxNumber = mentalMath ? maxNumberForLevel(level) : maxNumberForLevel(level) * 5;
    int second = mentalMath ? generateMentalMathFriendlyNumber(maxNumber) : (rand() % maxNumber + 1);
    bool isAddition = (rand() % 2 == 0);

    double result = isAddition ? (first.value + second) : (first.value - second);

    Task task;
    task.ruleName = "AdditionSubtraction (kombiniert)";
    task.promptText = QString("%1 %2 %3 =").arg(first.display).arg(isAddition ? "+" : "-").arg(second);
    task.answers.append({ "", result });
    task.autoAdvance = mentalMath;

    qDebug() << "[AdditionSubtraction] Kombiniert, mentalMath:" << mentalMath << "|" << task.promptText;
    return task;
}

TaskFragment combineFragments(const TaskFragment &a, const TaskFragment &b, bool mentalMath)
{
    Q_UNUSED(mentalMath);
    bool isAddition = (rand() % 2 == 0);

    TaskFragment result;
    result.value = isAddition ? (a.value + b.value) : (a.value - b.value);
    result.display = QString("%1 %2 %3").arg(a.display).arg(isAddition ? "+" : "-").arg(b.display);

    qDebug() << "[AdditionSubtraction] Fragmente kombiniert:" << result.display << "=" << result.value;
    return result;
}