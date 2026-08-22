#include "percent_mult_div_generator.h"
#include <cstdlib>
#include <QDebug>

static int maxFactorForLevel(DifficultyLevel level)
{
    return 9 + level / 2;
}

Task generateMultDivTask(DifficultyLevel level, bool mentalMath)
{
    // Kopfrechnen: normaler Bereich. Ausgeschaltet: deutlich groesser (Herausforderung)
    int maxFactor = mentalMath ? maxFactorForLevel(level) : maxFactorForLevel(level) * 3;
    bool isMultiplication = (rand() % 2 == 0);

    Task task;
    task.autoAdvance = mentalMath;   // bei ausgeschaltetem Kopfrechnen nicht automatisch weiterspringen

    if (isMultiplication) {
        int a = rand() % maxFactor + 1;
        int b = rand() % maxFactor + 1;
        task.ruleName = "Multiplication";
        task.promptText = QString("%1 × %2 =").arg(a).arg(b);
        task.answers.append({ "", static_cast<double>(a * b) });
        qDebug() << "[MultDiv] mentalMath:" << mentalMath << "|" << task.promptText;
    } else {
        // Divisor + Ergebnis zuerst waehlen, DANN Dividend berechnen -> Division geht immer glatt auf
        int divisor = rand() % 10 + 2;
        int quotient = rand() % maxFactor + 1;
        int dividend = divisor * quotient;
        task.ruleName = "Division";
        task.promptText = QString("%1 ÷ %2 =").arg(dividend).arg(divisor);
        task.answers.append({ "", static_cast<double>(quotient) });
        qDebug() << "[MultDiv] mentalMath:" << mentalMath << "|" << task.promptText;
    }

    return task;
}

Task generatePercentTask(DifficultyLevel level, bool mentalMath)
{
    Q_UNUSED(level);   // TODO: eigene Level-Skalierung fuer Prozentrechnung noch offen

    Task task;
    task.autoAdvance = mentalMath;

    if (mentalMath) {
        // "Saubere" Prozentsaetze und glatte Grundwerte
        int percentSteps[] = {5, 10, 20, 25, 50, 75};
        int percent = percentSteps[rand() % 6];
        int base = (rand() % 20 + 1) * 10;
        task.ruleName = "Percentage";
        task.promptText = QString("%1% von %2 =").arg(percent).arg(base);
        task.answers.append({ "", (percent * base) / 100.0 });
    } else {
        // Herausforderung: beliebiger Prozentsatz, beliebiger Grundwert
        int percent = rand() % 99 + 1;
        int base = rand() % 990 + 10;
        task.ruleName = "Percentage";
        task.promptText = QString("%1% von %2 =").arg(percent).arg(base);
        task.answers.append({ "", (percent * base) / 100.0 });
    }

    qDebug() << "[Percent] mentalMath:" << mentalMath << "|" << task.promptText;
    return task;
}

TaskFragment generateMultDivFragment(DifficultyLevel level, bool mentalMath)
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