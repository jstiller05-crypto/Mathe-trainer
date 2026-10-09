#include "addition_subtraction_generator.h"
#include "negative_number_generator.h"
#include "random_utils.h"
#include <algorithm>
#include <QDebug>

static int maxNumberForLevel(DifficultyLevel level)
{
    return 20 + level * 20;
}

// Kopfrechnen-Variante: nutzt dieselbe Formel wie maxNumberForLevel(), aber der Level-Input
// wird ab Kl.6 gedeckelt -> danach ein Plateau statt weiterem linearen Wachstum. Ohne diesen
// Deckel wuerde der Zahlenraum bei Kl.10 auf ein Mehrfaches steigen - das ist nicht mehr
// "im Kopf loesbar" (siehe generator-bench: Kopfrechnen-Addition ging bis Ø 955,79 / Max 2680
// hoch). Mit dem Deckel bleibt der Zahlenraum ab Kl.6 konstant bei dem Wert, den Kl.6 selbst
// schon hatte. F16: classToLevel(6) statt der frueheren nackten Zahl 31, damit diese Schwelle
// automatisch der zentralen Klasse->Level-Umrechnung (difficulty.h) folgt.
static int mentalMathMaxNumberForLevel(DifficultyLevel level)
{
    DifficultyLevel plateauLevel = classToLevel(6);
    return maxNumberForLevel(std::min(level, plateauLevel));
}

// F16: nutzt jetzt NegativeNumberCriteria::MinLevel (negative_number_generator.h) statt
// einer eigenen Kopie derselben Schwelle - "ab hier sind negative Ergebnisse erlaubt" soll
// nur noch an EINER Stelle definiert sein (siehe auch kNegativeResultsMinLevel in
// arithmetic_unit.cpp, das jetzt ebenfalls dorthin verweist).
static bool negativeResultsAllowed(DifficultyLevel level)
{
    return level >= NegativeNumberCriteria::MinLevel;
}

// F17/F23: generateMentalMathFriendlyNumber() nutzte vorher rand()/RAND_MAX direkt - unter
// Windows ist RAND_MAX nur 32767 (F17), und der Quotient konnte genau 1.0 ergeben, wodurch
// rawValue einen Schritt ueber maxValue landen konnte (F23). randomUnitInterval() (siehe
// random_utils.h) behebt beides: QRandomGenerator ist nicht an RAND_MAX gebunden und liefert
// garantiert einen Wert < 1.0.
static int generateMentalMathFriendlyNumber(int maxValue)
{
    double randomFraction = randomUnitInterval();
    double biasedFraction = randomFraction * randomFraction;
    int rawValue = static_cast<int>(biasedFraction * maxValue) + 1;

    if (rawValue >= 1000) return (rawValue / 10) * 10;
    if (rawValue >= 100)  return (rawValue / 5) * 5;
    return rawValue;
}

static int generateOperand(int maxValue, bool mentalMath)
{
    return mentalMath ? generateMentalMathFriendlyNumber(maxValue) : randomInt(1, maxValue);
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
    int second = mentalMath ? generateMentalMathFriendlyNumber(upperBound) : randomInt(0, upperBound);

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
    // Ein Ziffern-Kaestchen fasst nur 1 Zeichen - bei negativem Ergebnis (ab NegativeNumberCriteria::MinLevel erlaubt)
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