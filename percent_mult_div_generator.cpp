#include "percent_mult_div_generator.h"
#include "random_utils.h"
#include <QDebug>
#include <QVector>

static int maxFactorForLevel(DifficultyLevel level)
{
    return 9 + level / 2;
}

// Kopfrechnen-Obergrenze fuer Multiplikations-Faktoren: bewusst FEST (nicht level-abhaengig),
// damit "Kopfrechnen" auch bei hohen Klassenstufen einstellige bis niedrige zweistellige
// Faktoren bleibt. Vorher nutzte generateMultiplicationTask() im mentalMath-Fall
// maxFactorForLevel(level), das bis auf 44 (Level 71/Kl.10) waechst - Ergebnis waren
// Aufgaben wie "38 × 41 =" als angebliche Kopfrechenaufgabe (siehe generator-bench:
// Ø 514,55 / Max 1806 bei Kl.10). generateMultiplicationFragment() hatte diesen Deckel
// (10) schon immer - jetzt teilen sich beide Funktionen denselben Wert statt inkonsistent
// zu sein.
static int mentalMathMultiplicationMaxFactor()
{
    return 10;
}

Task generateMultiplicationTask(DifficultyLevel level, bool mentalMath)
{
    int maxFactor = mentalMath ? mentalMathMultiplicationMaxFactor() : maxFactorForLevel(level) * 3;
    int a = randomInt(1, maxFactor);
    int b = randomInt(1, maxFactor);

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

    qDebug() << "[Multiplication] mentalMath:" << mentalMath << "| maxFactor:" << maxFactor << "|" << task.promptText;
    return task;
}

TaskFragment generateMultiplicationFragment(DifficultyLevel level, bool mentalMath)
{
    Q_UNUSED(level);
    int maxFactor = mentalMath ? mentalMathMultiplicationMaxFactor() : 25;
    int a = randomInt(1, maxFactor);
    int b = randomInt(1, maxFactor);

    TaskFragment fragment;
    fragment.value = a * b;
    fragment.display = QString("%1 × %2").arg(a).arg(b);
    fragment.precedence = Precedence::Point;   // aeusserster Operator ist ×
    return fragment;
}

Task generateDivisionTask(DifficultyLevel level, bool mentalMath)
{
    int maxFactor = mentalMath ? maxFactorForLevel(level) : maxFactorForLevel(level) * 3;
    int divisor = randomInt(2, 11);
    int quotient = randomInt(1, maxFactor);
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
    int divisor = randomInt(2, 9);
    int quotient = randomInt(1, maxFactor);
    int dividend = divisor * quotient;

    TaskFragment fragment;
    fragment.value = quotient;
    fragment.display = QString("%1 ÷ %2").arg(dividend).arg(divisor);
    fragment.precedence = Precedence::Point;   // aeusserster Operator ist ÷
    return fragment;
}

Task generatePercentTask(DifficultyLevel level, bool mentalMath)
{
    int percent;
    int base;
    if (mentalMath) {
        // Kopfrechnen-Prozentsaetze werden mit steigendem Level "unrunder" - so bekommt ein
        // Zehntklaessler nicht mehr exakt dieselben Aufgaben wie ein Drittklaessler (siehe
        // generator-bench: vorher war die Duplikate-Rate bei Kl.3 UND Kl.10 identisch 82%,
        // weil level hier komplett ignoriert wurde).
        QVector<int> percentSteps = { 5, 10, 20, 25, 50, 75 };
        // F16: classToLevel(6)/classToLevel(8) statt der frueheren nackten Zahlen 31/51.
        if (level >= classToLevel(6)) percentSteps += QVector<int>{ 15, 30, 40, 60, 80, 90 };
        if (level >= classToLevel(8)) percentSteps += QVector<int>{ 12, 35, 45, 65, 85 };
        percent = percentSteps[randomInt(0, percentSteps.size() - 1)];

        // Grundwert waechst mit dem Level (bleibt durch den festen Faktor 10 weiterhin
        // eine "runde" Zahl fuers Kopfrechnen), analog zu maxFactorForLevel() oben.
        int maxBaseSteps = 20 + level / 2;
        base = randomInt(1, maxBaseSteps) * 10;
    } else {
        percent = randomInt(1, 99);
        base = randomInt(10, 999);
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

    qDebug() << "[Percent] mentalMath:" << mentalMath << "| level:" << level << "| moeglicheProzentsaetze:"
              << (mentalMath ? "level-abhaengig" : "1-99") << "|" << task.promptText;
    return task;
}