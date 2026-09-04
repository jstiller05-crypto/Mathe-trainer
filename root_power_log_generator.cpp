#include "root_power_log_generator.h"
#include <cstdlib>
#include <cmath>
#include <QDebug>

static TaskFragment buildPowerFragment(DifficultyLevel level, bool mentalMath)
{
    bool fullRangeUnlocked = level >= RootPowerLogCriteria::PowerFullMinLevel;

    int base = mentalMath ? (rand() % 8 + 2) : (rand() % 12 + 2);
    int exponent = fullRangeUnlocked ? (mentalMath ? (rand() % 2 + 2) : (rand() % 3 + 2)) : 2;

    TaskFragment fragment;
    fragment.value = std::pow(base, exponent);
    fragment.display = QString("%1^%2").arg(base).arg(exponent);
    return fragment;
}

Task generatePowerTask(DifficultyLevel level, bool mentalMath)
{
    TaskFragment fragment = buildPowerFragment(level, mentalMath);

    Task task;
    task.ruleName = "Power";
    task.promptText = fragment.display + " =";
    task.answers.append({ "", fragment.value });
    task.autoAdvance = mentalMath;

    // Potenz hat kein Operanden-Schema fuers Stacked-Raster - immer SingleLine.
    QString answerStr = QString::number(fragment.value);
    task.writtenCalculation.expression = task.promptText;
    task.writtenCalculation.answerDigitCount = answerStr.length();
    task.writtenCalculation.freeformAnswer = answerStr.contains('.');
    task.writtenCalculation.mode = WrittenCalculation::DisplayMode::SingleLine;

    qDebug() << "[Power] Level:" << level << "| mentalMath:" << mentalMath << "|" << task.promptText;
    return task;
}

TaskFragment generatePowerFragment(DifficultyLevel level, bool mentalMath)
{
    return buildPowerFragment(level, mentalMath);
}

static TaskFragment buildRootFragment(DifficultyLevel level, bool mentalMath)
{
    Q_UNUSED(level);
    TaskFragment fragment;

    if (mentalMath) {
        int root = rand() % 10 + 2;
        int operand = root * root;
        fragment.value = root;
        fragment.display = QString("√%1").arg(operand);
    } else {
        int operand = rand() % 90 + 10;
        fragment.value = std::round(std::sqrt(operand) * 100.0) / 100.0;
        fragment.display = QString("√%1").arg(operand);
    }

    return fragment;
}

Task generateRootTask(DifficultyLevel level, bool mentalMath)
{
    TaskFragment fragment = buildRootFragment(level, mentalMath);

    Task task;
    task.ruleName = "Root";
    task.promptText = fragment.display + " =";
    task.answers.append({ "", fragment.value });
    task.autoAdvance = mentalMath;

    // Wurzel hat kein Operanden-Schema fuers Stacked-Raster - immer SingleLine.
    // Im Nicht-Kopfrechnen-Fall ist das Ergebnis auf 2 Nachkommastellen gerundet
    // (z.B. 6.86) - dafuer gibt es dann EIN zusammenhaengendes Eingabefeld.
    QString answerStr = QString::number(fragment.value);
    task.writtenCalculation.expression = task.promptText;
    task.writtenCalculation.answerDigitCount = answerStr.length();
    task.writtenCalculation.freeformAnswer = answerStr.contains('.');
    task.writtenCalculation.mode = WrittenCalculation::DisplayMode::SingleLine;

    qDebug() << "[Root] Level:" << level << "| mentalMath:" << mentalMath << "|" << task.promptText;
    return task;
}

TaskFragment generateRootFragment(DifficultyLevel level, bool mentalMath)
{
    return buildRootFragment(level, mentalMath);
}

static TaskFragment buildLogFragment(DifficultyLevel level, bool mentalMath)
{
    Q_UNUSED(level);
    TaskFragment fragment;

    if (mentalMath) {
        int exponent = rand() % 4 + 1;
        int operand = static_cast<int>(std::pow(10, exponent));
        fragment.value = exponent;
        fragment.display = QString("log(%1)").arg(operand);
    } else {
        int operand = rand() % 9900 + 100;
        fragment.value = std::round(std::log10(operand) * 100.0) / 100.0;
        fragment.display = QString("log(%1)").arg(operand);
    }

    return fragment;
}

Task generateLogTask(DifficultyLevel level, bool mentalMath)
{
    TaskFragment fragment = buildLogFragment(level, mentalMath);

    Task task;
    task.ruleName = "Log";
    task.promptText = fragment.display + " =";
    task.answers.append({ "", fragment.value });
    task.autoAdvance = mentalMath;

    // Logarithmus hat kein Operanden-Schema fuers Stacked-Raster - immer SingleLine.
    QString answerStr = QString::number(fragment.value);
    task.writtenCalculation.expression = task.promptText;
    task.writtenCalculation.answerDigitCount = answerStr.length();
    task.writtenCalculation.freeformAnswer = answerStr.contains('.');
    task.writtenCalculation.mode = WrittenCalculation::DisplayMode::SingleLine;

    qDebug() << "[Log] Level:" << level << "| mentalMath:" << mentalMath << "|" << task.promptText;
    return task;
}

TaskFragment generateLogFragment(DifficultyLevel level, bool mentalMath)
{
    return buildLogFragment(level, mentalMath);
}