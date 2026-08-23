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
    if (subcategory == "Addition") return generateAdditionFragment(level, mentalMath);
    if (subcategory == "Subtraktion") return generateSubtractionFragment(level, mentalMath);
    if (subcategory == "Multiplikation") return generateMultiplicationFragment(level, mentalMath);
    if (subcategory == "Division") return generateDivisionFragment(level, mentalMath);
    if (subcategory == "Potenz") return generatePowerFragment(level, mentalMath);
    if (subcategory == "Wurzel") return generateRootFragment(level, mentalMath);
    if (subcategory == "Logarithmus") return generateLogFragment(level, mentalMath);

    qWarning() << "[ArithmeticUnit] Unterkategorie" << subcategory << "unterstuetzt keine Fragmente - Fallback";
    return generateAdditionFragment(level, mentalMath);
}

static bool supportsFragment(const QString &subcategory)
{
    return subcategory != "Finanzen & Einheiten" && subcategory != "Prozentrechnung";
}

static Task standaloneForSubcategory(const QString &subcategory, DifficultyLevel level, bool mentalMath)
{
    if (subcategory == "Addition") return generateAdditionTask(level, mentalMath);
    if (subcategory == "Subtraktion") return generateSubtractionTask(level, mentalMath);
    if (subcategory == "Multiplikation") return generateMultiplicationTask(level, mentalMath);
    if (subcategory == "Division") return generateDivisionTask(level, mentalMath);
    if (subcategory == "Prozentrechnung") return generatePercentTask(level, mentalMath);
    if (subcategory == "Potenz") return generatePowerTask(level, mentalMath);
    if (subcategory == "Wurzel") return generateRootTask(level, mentalMath);
    if (subcategory == "Logarithmus") return generateLogTask(level, mentalMath);

    if (subcategory == "Finanzen & Einheiten") {
        bool useFinance = (rand() % 2 == 0);
        return useFinance ? generateFinanceTask(level) : generateUnitsTask(level);
    }

    qWarning() << "[ArithmeticUnit] Unbekannte Unterkategorie:" << subcategory << "- Fallback";
    return generateAdditionTask(level, mentalMath);
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
        qWarning() << "[ArithmeticUnit] Keine Unterkategorie aktiv - Fallback auf Addition";
        return generateAdditionTask(level, mentalMath);
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

QStringList arithmeticAvailableSubcategories(DifficultyLevel level)
{
    QStringList available;
    available << "Addition" << "Subtraktion" << "Multiplikation" << "Division" << "Prozentrechnung" << "Finanzen & Einheiten";

    if (level >= RootPowerLogCriteria::PowerMinLevel) available << "Potenz";
    if (level >= RootPowerLogCriteria::RootMinLevel) available << "Wurzel";
    if (level >= RootPowerLogCriteria::LogMinLevel) available << "Logarithmus";

    qDebug() << "[ArithmeticUnit] Verfuegbare Unterkategorien bei Level" << level << ":" << available;
    return available;
}