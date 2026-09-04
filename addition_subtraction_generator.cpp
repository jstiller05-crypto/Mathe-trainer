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

static int generateMentalMathFriendlyNumber(int maxValue)
{
    double randomFraction = static_cast<double>(rand()) / RAND_MAX;
    double biasedFraction = randomFraction * randomFraction;
    int rawValue = static_cast<int>(biasedFraction * maxValue) + 1;

    if (rawValue >= 1000) return (rawValue / 10) * 10;
    if (rawValue >= 100)  return (rawValue / 5) * 5;
    return rawValue;
}

static int generateOperand(int maxValue, bool mentalMath)
{
    return mentalMath ? generateMentalMathFriendlyNumber(maxValue) : (rand() % maxValue + 1);
}

Task generateAdditionTask(DifficultyLevel level, bool mentalMath)
{
    int maxNumber = mentalMath ? maxNumberForLevel(level) : maxNumberForLevel(level) * 5;
    int first = generateOperand(maxNumber, mentalMath);
    int second = generateOperand(maxNumber, mentalMath);

    Task task;
    task.ruleName = "Addition";
    task.promptText = QString("%1 + %2 =").arg(first).arg(second);
    task.answers.append({ "", static_cast<double>(first + second) });
    task.autoAdvance = mentalMath;

    QString answerStr = QString::number(first + second);
    task.writtenCalculation.operands = { QString::number(first), QString::number(second) };
    task.writtenCalculation.operatorSymbol = "+";
    task.writtenCalculation.expression = task.promptText;
    task.writtenCalculation.answerDigitCount = answerStr.length();
    task.writtenCalculation.freeformAnswer = false;   // Summe zweier positiver Zahlen ist immer eine positive Ganzzahl
    task.writtenCalculation.mode = mentalMath ? WrittenCalculation::DisplayMode::SingleLine : WrittenCalculation::DisplayMode::Stacked;

    qDebug() << "[Addition] mentalMath:" << mentalMath << "|" << task.promptText;
    return task;
}

TaskFragment generateAdditionFragment(DifficultyLevel level, bool mentalMath)
{
    int maxNumber = mentalMath ? maxNumberForLevel(level) : maxNumberForLevel(level) * 5;
    int value = generateOperand(maxNumber, mentalMath);

    TaskFragment fragment;
    fragment.value = value;
    fragment.display = QString::number(value);
    return fragment;
}

Task generateSubtractionTask(DifficultyLevel level, bool mentalMath)
{
    int maxNumber = mentalMath ? maxNumberForLevel(level) : maxNumberForLevel(level) * 5;
    int first = generateOperand(maxNumber, mentalMath);

    int upperBound = negativeResultsAllowed(level) ? maxNumber : first;
    int second = mentalMath ? generateMentalMathFriendlyNumber(upperBound) : (rand() % (upperBound + 1));

    int result = first - second;

    Task task;
    task.ruleName = "Subtraction";
    task.promptText = QString("%1 - %2 =").arg(first).arg(second);
    task.answers.append({ "", static_cast<double>(result) });
    task.autoAdvance = mentalMath;

    QString answerStr = QString::number(result);
    task.writtenCalculation.operands = { QString::number(first), QString::number(second) };
    task.writtenCalculation.operatorSymbol = "-";
    task.writtenCalculation.expression = task.promptText;
    task.writtenCalculation.answerDigitCount = answerStr.length();
    // Ein Ziffern-Kaestchen fasst nur 1 Zeichen - bei negativem Ergebnis (ab Level 31 erlaubt)
    // gibt es deshalb EIN zusammenhaengendes Feld statt einzelner Kaestchen fuers Vorzeichen.
    task.writtenCalculation.freeformAnswer = answerStr.contains('-');
    task.writtenCalculation.mode = mentalMath ? WrittenCalculation::DisplayMode::SingleLine : WrittenCalculation::DisplayMode::Stacked;

    qDebug() << "[Subtraction] mentalMath:" << mentalMath << "|" << task.promptText;
    return task;
}

TaskFragment generateSubtractionFragment(DifficultyLevel level, bool mentalMath)
{
    // Fragment ist einfach ein Zahlenwert - gleiche Logik wie bei Addition
    return generateAdditionFragment(level, mentalMath);
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