#include "percent_mult_div_generator.h"
#include <cstdlib>
#include <QDebug>

static int maxFactorForLevel(DifficultyLevel level)
{
    return 9 + level / 2;
}

Task generateMultiplicationTask(DifficultyLevel level, bool mentalMath)
{
    int maxFactor = mentalMath ? maxFactorForLevel(level) : maxFactorForLevel(level) * 3;
    int a = rand() % maxFactor + 1;
    int b = rand() % maxFactor + 1;

    Task task;
    task.ruleName = "Multiplication";
    task.promptText = QString("%1 × %2 =").arg(a).arg(b);
    task.answers.append({ "", static_cast<double>(a * b) });
    task.autoAdvance = mentalMath;

    task.writtenCalculation.operands = { QString::number(a), QString::number(b) };
    task.writtenCalculation.operatorSymbol = "×";
    task.writtenCalculation.expression = task.promptText;
    task.writtenCalculation.answerDigitCount = QString::number(a * b).length();
    task.writtenCalculation.freeformAnswer = false;   // Produkt zweier positiver Zahlen ist immer eine positive Ganzzahl
    task.writtenCalculation.mode = mentalMath ? WrittenCalculation::DisplayMode::SingleLine : WrittenCalculation::DisplayMode::Stacked;

    qDebug() << "[Multiplication] mentalMath:" << mentalMath << "|" << task.promptText;
    return task;
}

TaskFragment generateMultiplicationFragment(DifficultyLevel level, bool mentalMath)
{
    Q_UNUSED(level);
    int maxFactor = mentalMath ? 10 : 25;
    int a = rand() % maxFactor + 1;
    int b = rand() % maxFactor + 1;

    TaskFragment fragment;
    fragment.value = a * b;
    fragment.display = QString("%1 × %2").arg(a).arg(b);
    return fragment;
}

Task generateDivisionTask(DifficultyLevel level, bool mentalMath)
{
    int maxFactor = mentalMath ? maxFactorForLevel(level) : maxFactorForLevel(level) * 3;
    int divisor = rand() % 10 + 2;
    int quotient = rand() % maxFactor + 1;
    int dividend = divisor * quotient;

    Task task;
    task.ruleName = "Division";
    task.promptText = QString("%1 ÷ %2 =").arg(dividend).arg(divisor);
    task.answers.append({ "", static_cast<double>(quotient) });
    task.autoAdvance = mentalMath;

    task.writtenCalculation.operands = { QString::number(dividend), QString::number(divisor) };
    task.writtenCalculation.operatorSymbol = "÷";
    task.writtenCalculation.expression = task.promptText;
    task.writtenCalculation.answerDigitCount = QString::number(quotient).length();
    task.writtenCalculation.freeformAnswer = false;   // per Konstruktion geht die Division immer glatt auf (dividend = divisor * quotient)
    task.writtenCalculation.mode = mentalMath ? WrittenCalculation::DisplayMode::SingleLine : WrittenCalculation::DisplayMode::Stacked;

    qDebug() << "[Division] mentalMath:" << mentalMath << "|" << task.promptText;
    return task;
}

TaskFragment generateDivisionFragment(DifficultyLevel level, bool mentalMath)
{
    Q_UNUSED(level);
    int maxFactor = mentalMath ? 10 : 20;
    int divisor = rand() % 8 + 2;
    int quotient = rand() % maxFactor + 1;
    int dividend = divisor * quotient;

    TaskFragment fragment;
    fragment.value = quotient;
    fragment.display = QString("%1 ÷ %2").arg(dividend).arg(divisor);
    return fragment;
}

Task generatePercentTask(DifficultyLevel level, bool mentalMath)
{
    Q_UNUSED(level);

    int percent;
    int base;
    if (mentalMath) {
        int percentSteps[] = {5, 10, 20, 25, 50, 75};
        percent = percentSteps[rand() % 6];
        base = (rand() % 20 + 1) * 10;
    } else {
        percent = rand() % 99 + 1;
        base = rand() % 990 + 10;
    }
    double result = (percent * base) / 100.0;

    Task task;
    task.ruleName = "Percentage";
    task.promptText = QString("%1% von %2 =").arg(percent).arg(base);
    task.answers.append({ "", result });
    task.autoAdvance = mentalMath;

    // Prozentrechnung hat kein Operanden-Schema fuer das Stacked-Raster (kein
    // schriftliches Verfahren dafuer) - deshalb immer SingleLine.
    QString answerStr = QString::number(result);
    task.writtenCalculation.expression = task.promptText;
    task.writtenCalculation.answerDigitCount = answerStr.length();
    task.writtenCalculation.freeformAnswer = answerStr.contains('.');   // z.B. 5% von 47 = 2.35
    task.writtenCalculation.mode = WrittenCalculation::DisplayMode::SingleLine;

    qDebug() << "[Percent] mentalMath:" << mentalMath << "|" << task.promptText;
    return task;
}