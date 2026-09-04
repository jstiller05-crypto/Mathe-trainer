#include "arithmetic_unit.h"
#include "addition_subtraction_generator.h"
#include "percent_mult_div_generator.h"
#include "root_power_log_generator.h"
#include "finance_generator.h"
#include "units_generator.h"
#include <cstdlib>
#include <algorithm>
#include <QDebug>

static TaskFragment fragmentForSubcategory(const QString &subcategory, DifficultyLevel level, bool smallNumbers)
{
    if (subcategory == "Addition") return generateAdditionFragment(level, smallNumbers);
    if (subcategory == "Subtraktion") return generateSubtractionFragment(level, smallNumbers);
    if (subcategory == "Multiplikation") return generateMultiplicationFragment(level, smallNumbers);
    if (subcategory == "Division") return generateDivisionFragment(level, smallNumbers);
    if (subcategory == "Potenz") return generatePowerFragment(level, smallNumbers);
    if (subcategory == "Wurzel") return generateRootFragment(level, smallNumbers);
    if (subcategory == "Logarithmus") return generateLogFragment(level, smallNumbers);

    qWarning() << "[ArithmeticUnit] Unterkategorie" << subcategory << "unterstuetzt keine Fragmente - Fallback";
    return generateAdditionFragment(level, smallNumbers);
}

static bool supportsFragment(const QString &subcategory)
{
    return subcategory != "Finanzen & Einheiten" && subcategory != "Prozentrechnung";
}

static Task standaloneForSubcategory(const QString &subcategory, DifficultyLevel level, bool smallNumbers)
{
    if (subcategory == "Addition") return generateAdditionTask(level, smallNumbers);
    if (subcategory == "Subtraktion") return generateSubtractionTask(level, smallNumbers);
    if (subcategory == "Multiplikation") return generateMultiplicationTask(level, smallNumbers);
    if (subcategory == "Division") return generateDivisionTask(level, smallNumbers);
    if (subcategory == "Prozentrechnung") return generatePercentTask(level, smallNumbers);
    if (subcategory == "Potenz") return generatePowerTask(level, smallNumbers);
    if (subcategory == "Wurzel") return generateRootTask(level, smallNumbers);
    if (subcategory == "Logarithmus") return generateLogTask(level, smallNumbers);

    if (subcategory == "Finanzen & Einheiten") {
        bool useFinance = (rand() % 2 == 0);
        return useFinance ? generateFinanceTask(level) : generateUnitsTask(level);
    }

    qWarning() << "[ArithmeticUnit] Unbekannte Unterkategorie:" << subcategory << "- Fallback";
    return generateAdditionTask(level, smallNumbers);
}

// preferShortChains = true -> meistens nur 1 Operator (Kopfrechnen ODER Taschenrechner-Modus).
// preferShortChains = false -> laengere Verkettungen bevorzugt (nur "Schwere Aufgabe"-Modus).
static int pickChainLength(bool preferShortChains, int activeFragmentCapableCount)
{
    int maxPossible = std::min(activeFragmentCapableCount, preferShortChains ? 3 : 4);
    if (maxPossible <= 1) return 1;

    int roll = rand() % 100;

    if (preferShortChains) {
        if (roll < 70) return 1;
        if (roll < 95) return std::min(2, maxPossible);
        return maxPossible;
    } else {
        if (roll < 20) return 1;
        if (roll < 55) return std::min(2, maxPossible);
        return maxPossible;
    }
}

Task generateArithmeticTask(DifficultyLevel level, const QStringList &activeSubcategories, TaskMode mode)
{
    // Zwei unabhaengige Achsen, aus dem gewaehlten Modus abgeleitet:
    // - smallNumbers steuert die Zahlengroesse in den Einzel-Generatoren (nur bei Kopfrechnen klein/rund)
    // - shortChain steuert die Kettenlaenge (nur bei "Schwere Aufgabe" duerfen es mehr Glieder werden)
    bool smallNumbers = (mode == TaskMode::MentalMath);
    bool shortChain = (mode != TaskMode::Hard);

    if (activeSubcategories.isEmpty()) {
        qWarning() << "[ArithmeticUnit] Keine Unterkategorie aktiv - Fallback auf Addition";
        return generateAdditionTask(level, smallNumbers);
    }

    qDebug() << "[ArithmeticUnit] Aktive Unterkategorien:" << activeSubcategories << "| Modus:" << static_cast<int>(mode);

    QStringList fragmentCapable;
    for (const QString &sub : activeSubcategories) {
        if (supportsFragment(sub)) fragmentCapable.append(sub);
    }

    int chainLength = pickChainLength(shortChain, fragmentCapable.size());
    qDebug() << "[ArithmeticUnit] Kettenlaenge gewaehlt:" << chainLength;

    if (chainLength <= 1) {
        QString chosen = activeSubcategories[rand() % activeSubcategories.size()];
        qDebug() << "[ArithmeticUnit] Keine Verschmelzung, gewaehlt:" << chosen;
        return standaloneForSubcategory(chosen, level, smallNumbers);
    }

    QString firstSub = fragmentCapable[rand() % fragmentCapable.size()];
    TaskFragment chain = fragmentForSubcategory(firstSub, level, smallNumbers);
    qDebug() << "[ArithmeticUnit] Kette gestartet mit" << firstSub << ":" << chain.display << "=" << chain.value;

    for (int i = 1; i < chainLength; ++i) {
        QString nextSub = fragmentCapable[rand() % fragmentCapable.size()];
        TaskFragment nextFragment = fragmentForSubcategory(nextSub, level, smallNumbers);
        chain = combineFragments(chain, nextFragment, smallNumbers);
        qDebug() << "[ArithmeticUnit] Glied" << (i + 1) << "(" << nextSub << "):" << chain.display << "=" << chain.value;
    }

    Task task;
    task.ruleName = "ArithmeticChain";
    task.promptText = chain.display + " =";
    task.answers.append({ "", chain.value });
    task.autoAdvance = smallNumbers;   // nur reines Kopfrechnen springt automatisch weiter

    // Verkettete Aufgaben haben kein festes Operanden-Schema (die Kette kann Potenz-,
    // Wurzel- oder Log-Fragmente enthalten) - deshalb immer SingleLine mit dem fertigen
    // Ausdruck als "expression", statt der (hier nicht anwendbaren) operands/operator-Felder.
    QString answerStr = QString::number(chain.value);
    task.writtenCalculation.expression = task.promptText;
    task.writtenCalculation.answerDigitCount = answerStr.length();
    task.writtenCalculation.freeformAnswer = answerStr.contains('.') || answerStr.contains('-');
    task.writtenCalculation.mode = WrittenCalculation::DisplayMode::SingleLine;

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