#include "root_power_log_generator.h"
#include <cstdlib>
#include <cmath>
#include <QVector>
#include <QDebug>

// KOPFRECHENBARKEITS-KRITERIUM: kleine Basis (2-9), kleiner Exponent (2-3) -
// Ergebnisse bleiben ueberschaubar (max. 9^3 = 729)
static TaskFragment makePowerFragment()
{
    int base = rand() % 8 + 2;
    int exponent = rand() % 2 + 2;

    TaskFragment fragment;
    fragment.value = std::pow(base, exponent);
    fragment.display = QString("%1^%2").arg(base).arg(exponent);
    return fragment;
}

// KOPFRECHENBARKEITS-KRITERIUM: Operand ist IMMER eine ECHTE Quadratzahl
// (4, 9, 16, 25, ...) - eine Wurzel aus z.B. 10 waere im Kopf kaum machbar.
static TaskFragment makeRootFragment()
{
    int root = rand() % 10 + 2;
    int operand = root * root;

    TaskFragment fragment;
    fragment.value = root;
    fragment.display = QString("√%1").arg(operand);
    return fragment;
}

// KOPFRECHENBARKEITS-KRITERIUM: nur Zehnerpotenzen (10, 100, 1000, ...) -
// log(37) waere im Kopf nicht loesbar, log(100) = 2 dagegen schon.
static TaskFragment makeLogFragment()
{
    int exponent = rand() % 4 + 1;
    int operand = static_cast<int>(std::pow(10, exponent));

    TaskFragment fragment;
    fragment.value = exponent;
    fragment.display = QString("log(%1)").arg(operand);
    return fragment;
}

// Waehlt zufaellig eine der (je nach Level) verfuegbaren Fragment-Arten
static TaskFragment pickFragment(DifficultyLevel level, QString &ruleNameOut)
{
    QVector<int> available = { 0 };          // 0 = Potenz, immer verfuegbar
    if (level >= 55) available.append(1);      // 1 = Wurzel, ab Kl.9
    if (level >= 70) available.append(2);       // 2 = Logarithmus, ab Kl.10

    int choice = available[rand() % available.size()];

    if (choice == 0) { ruleNameOut = "Power"; return makePowerFragment(); }
    if (choice == 1) { ruleNameOut = "Root"; return makeRootFragment(); }
    ruleNameOut = "Log";
    return makeLogFragment();
}

Task generateRootPowerLogTask(DifficultyLevel level)
{
    QString ruleName;
    TaskFragment fragment = pickFragment(level, ruleName);

    Task task;
    task.ruleName = ruleName;
    task.promptText = fragment.display + " =";
    task.answers.append({ "", fragment.value });
    task.autoAdvance = true;

    qDebug() << "[RootPowerLog] Standalone:" << task.promptText << "=" << fragment.value;
    return task;
}

TaskFragment generateRootPowerLogFragment(DifficultyLevel level)
{
    QString unusedRuleName;
    return pickFragment(level, unusedRuleName);
}