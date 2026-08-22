#include "root_power_log_generator.h"
#include <cstdlib>
#include <cmath>
#include <QVector>
#include <QDebug>

// mentalMath=true: kleine Basis (2-9), kleiner Exponent (2-3) - immer im Kopf machbar
// mentalMath=false: groessere Basis/Exponent erlaubt - Herausforderung
static TaskFragment makePowerFragment(bool mentalMath)
{
    int base = mentalMath ? (rand() % 8 + 2) : (rand() % 12 + 2);
    int exponent = mentalMath ? (rand() % 2 + 2) : (rand() % 3 + 2);

    TaskFragment fragment;
    fragment.value = std::pow(base, exponent);
    fragment.display = QString("%1^%2").arg(base).arg(exponent);
    return fragment;
}

// mentalMath=true: IMMER eine echte Quadratzahl (4,9,16,...) - im Kopf loesbar
// mentalMath=false: beliebiger Operand erlaubt, Ergebnis wird gerundet
static TaskFragment makeRootFragment(bool mentalMath)
{
    TaskFragment fragment;

    if (mentalMath) {
        int root = rand() % 10 + 2;
        int operand = root * root;
        fragment.value = root;
        fragment.display = QString("√%1").arg(operand);
    } else {
        int operand = rand() % 90 + 10;   // NICHT zwingend eine Quadratzahl
        fragment.value = std::round(std::sqrt(operand) * 100.0) / 100.0;   // auf 2 Nachkommastellen runden
        fragment.display = QString("√%1").arg(operand);
    }

    return fragment;
}

// mentalMath=true: nur Zehnerpotenzen (10,100,1000,...) - im Kopf loesbar
// mentalMath=false: beliebige Basis-10-Zahl, Ergebnis gerundet
static TaskFragment makeLogFragment(bool mentalMath)
{
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

static TaskFragment pickFragment(DifficultyLevel level, bool mentalMath, QString &ruleNameOut)
{
    QVector<int> available = { 0 };
    if (level >= 55) available.append(1);
    if (level >= 70) available.append(2);

    int choice = available[rand() % available.size()];

    if (choice == 0) { ruleNameOut = "Power"; return makePowerFragment(mentalMath); }
    if (choice == 1) { ruleNameOut = "Root"; return makeRootFragment(mentalMath); }
    ruleNameOut = "Log";
    return makeLogFragment(mentalMath);
}

Task generateRootPowerLogTask(DifficultyLevel level, bool mentalMath)
{
    QString ruleName;
    TaskFragment fragment = pickFragment(level, mentalMath, ruleName);

    Task task;
    task.ruleName = ruleName;
    task.promptText = fragment.display + " =";
    task.answers.append({ "", fragment.value });
    task.autoAdvance = mentalMath;

    qDebug() << "[RootPowerLog] mentalMath:" << mentalMath << "|" << task.promptText << "=" << fragment.value;
    return task;
}

TaskFragment generateRootPowerLogFragment(DifficultyLevel level, bool mentalMath)
{
    QString unusedRuleName;
    return pickFragment(level, mentalMath, unusedRuleName);
}