#include "addition_subtraction_generator.h"
#include <cstdlib>
#include <algorithm>
#include <QDebug>

static int maxNumberForLevel(DifficultyLevel level)
{
    return 20 + level * 20;
}

// Kopfrechnen-Variante: nutzt dieselbe Formel wie maxNumberForLevel(), aber der Level-Input
// wird ab Kl.6 (Level 31) gedeckelt -> danach ein Plateau statt weiterem linearen Wachstum.
// Ohne diesen Deckel wuerde der Zahlenraum bei Kl.10 (Level 71) auf 1440 pro Operand steigen -
// das ist nicht mehr "im Kopf loesbar" (siehe generator-bench: Kopfrechnen-Addition ging bis
// Ø 955,79 / Max 2680 hoch). Mit dem Deckel bleibt der Zahlenraum ab Kl.6 konstant bei dem Wert,
// den Kl.6 selbst schon hatte.
static int mentalMathMaxNumberForLevel(DifficultyLevel level)
{
    constexpr DifficultyLevel PlateauLevel = 31;   // = classToLevel(6)
    return maxNumberForLevel(std::min(level, PlateauLevel));
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
    int maxNumber = mentalMath ? mentalMathMaxNumberForLevel(level) : maxNumberForLevel(level) * 5;
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

    qDebug() << "[Addition] mentalMath:" << mentalMath << "| maxNumber:" << maxNumber << "|" << task.promptText;
    return task;
}

TaskFragment generateAdditionFragment(DifficultyLevel level, bool mentalMath)
{
    int maxNumber = mentalMath ? mentalMathMaxNumberForLevel(level) : maxNumberForLevel(level) * 5;
    int value = generateOperand(maxNumber, mentalMath);

    TaskFragment fragment;
    fragment.value = value;
    fragment.display = QString::number(value);
    fragment.precedence = Precedence::Atom;   // einzelne Zahl, nichts zu klammern
    return fragment;
}

Task generateSubtractionTask(DifficultyLevel level, bool mentalMath)
{
    int maxNumber = mentalMath ? mentalMathMaxNumberForLevel(level) : maxNumberForLevel(level) * 5;
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

    qDebug() << "[Subtraction] mentalMath:" << mentalMath << "| maxNumber:" << maxNumber << "|" << task.promptText;
    return task;
}

TaskFragment generateSubtractionFragment(DifficultyLevel level, bool mentalMath)
{
    // Fragment ist einfach ein Zahlenwert - gleiche Logik wie bei Addition
    return generateAdditionFragment(level, mentalMath);
}