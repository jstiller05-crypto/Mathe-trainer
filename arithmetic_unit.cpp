#include "arithmetic_unit.h"
#include "addition_subtraction_generator.h"
#include "percent_mult_div_generator.h"
#include "root_power_log_generator.h"
#include "finance_generator.h"
#include "units_generator.h"
#include <cstdlib>
#include <algorithm>
#include <QDebug>

static TaskFragment fragmentForSubcategory(const QString &subcategory, DifficultyLevel level, bool mentalMath)
{
    if (subcategory == "Addition & Subtraktion") return generateAdditionSubtractionFragment(level, mentalMath);
    if (subcategory == "Wurzel/Potenz/Logarithmus") return generateRootPowerLogFragment(level, mentalMath);
    if (subcategory == "Bruch-/Prozentrechnung") return generateMultDivFragment(level, mentalMath);

    qWarning() << "[ArithmeticUnit] Unterkategorie" << subcategory << "unterstuetzt keine Fragmente - Fallback";
    return generateAdditionSubtractionFragment(level, mentalMath);
}

static bool supportsFragment(const QString &subcategory)
{
    return subcategory != "Finanzen & Einheiten";
}

static Task standaloneForSubcategory(const QString &subcategory, DifficultyLevel level, bool mentalMath)
{
    if (subcategory == "Addition & Subtraktion") return generateAdditionSubtractionTask(level, mentalMath);
    if (subcategory == "Wurzel/Potenz/Logarithmus") return generateRootPowerLogTask(level, mentalMath);

    if (subcategory == "Bruch-/Prozentrechnung") {
        bool usePercent = (rand() % 2 == 0);
        return usePercent ? generatePercentTask(level, mentalMath) : generateMultDivTask(level, mentalMath);
    }

    if (subcategory == "Finanzen & Einheiten") {
        bool useFinance = (rand() % 2 == 0);
        return useFinance ? generateFinanceTask(level) : generateUnitsTask(level);
    }

    qWarning() << "[ArithmeticUnit] Unbekannte Unterkategorie:" << subcategory << "- Fallback";
    return generateAdditionSubtractionTask(level, mentalMath);
}

static int pickChainLength(bool mentalMath, int activeFragmentCapableCount)
{
    int maxPossible = std::min(activeFragmentCapableCount, mentalMath ? 3 : 4);
    if (maxPossible <= 1) return 1;

    int roll = rand() % 100;

    if (mentalMath) {
        if (roll < 70) return 1;
        if (roll < 95) return std::min(2, maxPossible);
        return maxPossible;
    } else {
        if (roll < 20) return 1;
        if (roll < 55) return std::min(2, maxPossible);
        return maxPossible;
    }
}

Task generateArithmeticTask(DifficultyLevel level, const QStringList &activeSubcategories, bool mentalMath)
{
    if (activeSubcategories.isEmpty()) {
        qWarning() << "[ArithmeticUnit] Keine Unterkategorie aktiv - Fallback auf Addition & Subtraktion";
        return generateAdditionSubtractionTask(level, mentalMath);
    }

    qDebug() << "[ArithmeticUnit] Aktive Unterkategorien:" << activeSubcategories << "| mentalMath:" << mentalMath;

    QStringList fragmentCapable;
    for (const QString &sub : activeSubcategories) {
        if (supportsFragment(sub)) fragmentCapable.append(sub);
    }

    int chainLength = pickChainLength(mentalMath, fragmentCapable.size());
    qDebug() << "[ArithmeticUnit] Kettenlaenge gewaehlt:" << chainLength;

    if (chainLength <= 1) {
        QString chosen = activeSubcategories[rand() % activeSubcategories.size()];
        qDebug() << "[ArithmeticUnit] Keine Verschmelzung, gewaehlt:" << chosen;
        return standaloneForSubcategory(chosen, level, mentalMath);
    }

    QString firstSub = fragmentCapable[rand() % fragmentCapable.size()];
    TaskFragment chain = fragmentForSubcategory(firstSub, level, mentalMath);
    qDebug() << "[ArithmeticUnit] Kette gestartet mit" << firstSub << ":" << chain.display << "=" << chain.value;

    for (int i = 1; i < chainLength; ++i) {
        QString nextSub = fragmentCapable[rand() % fragmentCapable.size()];
        TaskFragment nextFragment = fragmentForSubcategory(nextSub, level, mentalMath);
        chain = combineFragments(chain, nextFragment, mentalMath);
        qDebug() << "[ArithmeticUnit] Glied" << (i + 1) << "(" << nextSub << "):" << chain.display << "=" << chain.value;
    }

    Task task;
    task.ruleName = "ArithmeticChain";
    task.promptText = chain.display + " =";
    task.answers.append({ "", chain.value });
    task.autoAdvance = mentalMath;

    qDebug() << "[ArithmeticUnit] FERTIGE KETTE:" << task.promptText << "| Loesung:" << chain.value;
    return task;
}