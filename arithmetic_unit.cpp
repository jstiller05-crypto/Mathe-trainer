#include "arithmetic_unit.h"
#include "addition_subtraction_generator.h"
#include "percent_mult_div_generator.h"
#include "root_power_log_generator.h"
#include "units_generator.h"
#include "term_generator.h"
#include "negative_number_generator.h"
#include "divisibility_generator.h"
#include "decimal_generator.h"
#include "fragment_algebra.h"
#include <cstdlib>
#include <cmath>
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
    if (subcategory == "Klammern & Terme") return generateTermFragment(level, smallNumbers);
    if (subcategory == "Negative Zahlen") return generateNegativeNumberFragment(level, smallNumbers);
    if (subcategory == "Teilbarkeit (ggT/kgV)") return generateDivisibilityFragment(level, smallNumbers);
    if (subcategory == "Dezimalzahlen") return generateDecimalFragment(level, smallNumbers);

    qWarning() << "[ArithmeticUnit] Unterkategorie" << subcategory << "unterstuetzt keine Fragmente - Fallback";
    return generateAdditionFragment(level, smallNumbers);
}

static bool supportsFragment(const QString &subcategory)
{
    return subcategory != "Größen & Einheiten" && subcategory != "Prozentrechnung";
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

    if (subcategory == "Größen & Einheiten") {
        return generateUnitsTask(level, smallNumbers);
    }
    if (subcategory == "Klammern & Terme") return generateTermTask(level, smallNumbers);
    if (subcategory == "Negative Zahlen") return generateNegativeNumberTask(level, smallNumbers);
    if (subcategory == "Teilbarkeit (ggT/kgV)") return generateDivisibilityTask(level, smallNumbers);
    if (subcategory == "Dezimalzahlen") return generateDecimalTask(level, smallNumbers);

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

// Operator-Mischung je Level - je hoeher das Level, desto haeufiger × und ÷.
// Die Klammern werden NICHT direkt gesteuert - sie entstehen von selbst aus dieser
// Mischung ueber die Regel in combineWithOperator()/wrapIfNeeded() (fragment_algebra):
// trifft × oder ÷ auf ein schwaecher bindendes Fragment (z.B. "3 + 4"), oder − / ÷
// auf ein gleich stark bindendes RECHTES Fragment, wird automatisch geklammert.
static QString pickOperatorForLevel(DifficultyLevel level)
{
    int roll = rand() % 100;

    if (level < 21) {
        // + 45%, - 45%, × 10%, ÷ 0%
        if (roll < 45) return "+";
        if (roll < 90) return "-";
        return "×";
    } else if (level < 41) {
        // + 30%, - 30%, × 25%, ÷ 15%
        if (roll < 30) return "+";
        if (roll < 60) return "-";
        if (roll < 85) return "×";
        return "÷";
    } else {
        // + 25%, - 25%, × 25%, ÷ 25%
        if (roll < 25) return "+";
        if (roll < 50) return "-";
        if (roll < 75) return "×";
        return "÷";
    }
}

static bool divisionIsExact(double dividend, double divisor)
{
    if (qFuzzyIsNull(divisor)) return false;
    return qFuzzyIsNull(std::fmod(dividend, divisor));
}

// Verkettet zwei Fragmente zu einem neuen - Unit-Logik (kennt Level und Modus),
// deshalb hier statt in einer Speiche (fruehere Version lag in
// addition_subtraction_generator.cpp und kannte nur + und -).
static TaskFragment combineFragments(const TaskFragment &a, const TaskFragment &b, DifficultyLevel level, bool mentalMath)
{
    Q_UNUSED(mentalMath);   // aktuell kein Einfluss auf die Operator-Wahl - Platz fuer spaeter

    // a) Operator nach Level wuerfeln (s.o.).
    QString op = pickOperatorForLevel(level);

    // b) Seitentausch: die Kette waechst weiter unten in generateArithmeticTask()
    // immer als LINKER Operand (chain = combineFragments(chain, next)). Ohne Tausch
    // koennten Klammern dadurch strukturell nur links entstehen - Faelle wie
    // "12 - (3 + 2)" oder "20 ÷ (2 × 5)" (Klammer RECHTS) waeren sonst unerreichbar.
    bool swapSides = (rand() % 2 == 0);
    const TaskFragment &left = swapSides ? b : a;
    const TaskFragment &right = swapSides ? a : b;

    // c) Division muss aufgehen (Divisor != 0, kein Rest) - sonst einen anderen
    // Operator wuerfeln. Nicht endlos versuchen (koennte bei unguenstigen Werten nie
    // aufgehen) - nach ein paar Anlaeufen auf "+" zurueckfallen, das geht immer.
    const int maxDivisionAttempts = 5;
    int attempts = 0;
    while (op == "÷" && !divisionIsExact(left.value, right.value) && attempts < maxDivisionAttempts) {
        op = pickOperatorForLevel(level);
        ++attempts;
    }
    if (op == "÷" && !divisionIsExact(left.value, right.value)) {
        op = "+";
    }

    // d) Die eigentliche Verkettung ist reine Mechanik aus Schritt 2 (fragment_algebra).
    TaskFragment result = combineWithOperator(left, right, op);
    qDebug() << "[ArithmeticUnit] Fragmente kombiniert:" << result.display << "=" << result.value
             << "| Operator:" << op
             << "| Praezedenz links:" << static_cast<int>(left.precedence)
             << "rechts:" << static_cast<int>(right.precedence);

    return result;
}

// Ein einzelner Erzeugungsversuch - kann eine unplausible Aufgabe liefern (siehe
// isPlausible() weiter unten), deshalb static/intern: generateArithmeticTask() (ganz
// unten, die einzige oeffentliche Funktion) wiederholt diesen Versuch bei Bedarf.
static Task generateArithmeticTaskOnce(DifficultyLevel level, const QStringList &activeSubcategories, TaskMode mode)
{
    // Zwei unabhaengige Achsen, aus dem gewaehlten Modus abgeleitet:
    // - smallNumbers steuert die Zahlengroesse in den Einzel-Generatoren (nur bei Kopfrechnen klein/rund)
    // - shortChain steuert die Kettenlaenge (nur bei "Schwere Aufgabe" duerfen es mehr Glieder werden)
    bool smallNumbers = (mode == TaskMode::MentalMath);
    bool shortChain = (mode != TaskMode::Hard);

    // Leere Auswahl bedeutet "alle fuer das aktuelle Level verfuegbaren Typen" statt
    // eines stillen Rueckfalls auf Addition - z.B. wenn in der Sidebar bewusst alle
    // Haekchen entfernt wurden (das ist jetzt erlaubt, siehe SidebarMenu).
    QStringList subcategories = activeSubcategories;
    if (subcategories.isEmpty()) {
        subcategories = arithmeticAvailableSubcategories(level);
        qDebug() << "[ArithmeticUnit] Keine Unterkategorie aktiv - nutze alle verfuegbaren:" << subcategories;
    }

    qDebug() << "[ArithmeticUnit] Aktive Unterkategorien:" << subcategories << "| Modus:" << static_cast<int>(mode);

    QStringList fragmentCapable;
    for (const QString &sub : subcategories) {
        if (supportsFragment(sub)) fragmentCapable.append(sub);
    }

    int chainLength = pickChainLength(shortChain, fragmentCapable.size());
    qDebug() << "[ArithmeticUnit] Kettenlaenge gewaehlt:" << chainLength;

    if (chainLength <= 1) {
        QString chosen = subcategories[rand() % subcategories.size()];
        qDebug() << "[ArithmeticUnit] Keine Verschmelzung, gewaehlt:" << chosen;
        return standaloneForSubcategory(chosen, level, smallNumbers);
    }

    QString firstSub = fragmentCapable[rand() % fragmentCapable.size()];
    TaskFragment chain = fragmentForSubcategory(firstSub, level, smallNumbers);
    qDebug() << "[ArithmeticUnit] Kette gestartet mit" << firstSub << ":" << chain.display << "=" << chain.value;

    for (int i = 1; i < chainLength; ++i) {
        QString nextSub = fragmentCapable[rand() % fragmentCapable.size()];
        TaskFragment nextFragment = fragmentForSubcategory(nextSub, level, smallNumbers);
        chain = combineFragments(chain, nextFragment, level, smallNumbers);
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

// Gleiche Schwelle wie negativeResultsAllowed() in addition_subtraction_generator.cpp
// (dort nicht exportiert) - als benannte Konstante statt einer nackten 31, damit der
// Zusammenhang beim Lesen sofort klar ist.
static constexpr DifficultyLevel kNegativeResultsMinLevel = 31;

// Grobe Obergrenze fuer den BETRAG des Ergebnisses, unabhaengig davon, wie grosszuegig
// die einzelnen Generatoren ihre eigenen Zahlenraeume waehlen - waechst mit dem Level.
static double resultMagnitudeCeilingForLevel(DifficultyLevel level)
{
    return 200.0 + level * 50.0;
}

// Ausdruck + Antwort duerfen zusammen nicht mehr als so viele Zeichen ergeben, sonst
// wird die Aufgabe im Karo-Raster (WrittenGridWidget, PAGE_COLUMNS = 32) trotz des
// Schrumpf-Mechanismus fuer lange Ketten kaum noch lesbar.
static constexpr int kMaxExpressionAndAnswerLength = 44;

// Prueft eine FERTIGE Aufgabe nochmal auf Plausibilitaet, wie es die CLAUDE.md fuer
// die Unit vorsieht - wichtig geworden, seit Terme (Klammern & Terme) mehrere
// Operatoren und damit staerker schwankende Ergebnisse liefern koennen. static/intern,
// da nur generateArithmeticTask() (s.u.) sie braucht.
static bool isPlausible(const Task &task, DifficultyLevel level, TaskMode mode)
{
    if (task.answers.isEmpty()) return false;
    double value = task.answers.first().expectedValue;

    if (value < 0 && level < kNegativeResultsMinLevel) {
        qDebug() << "[ArithmeticUnit] Unplausibel: negatives Ergebnis" << value << "unter Level" << kNegativeResultsMinLevel;
        return false;
    }

    if (std::abs(value) > resultMagnitudeCeilingForLevel(level)) {
        qDebug() << "[ArithmeticUnit] Unplausibel: Ergebnis" << value << "ueber der Obergrenze fuer Level" << level;
        return false;
    }

    // Kleine Toleranz statt exaktem Vergleich - wie schon in checkAnswers()
    // (session_controller.cpp), wichtig wegen Rundungsfehlern bei Gleitkommazahlen.
    // Ausnahme "Decimal": diese Aufgabenart ist per Definition nicht ganzzahlig -
    // ohne die Ausnahme wuerde jede einzelne Dezimalzahlen-Aufgabe im Kopfrechnen-
    // Modus alle 20 Versuche der Wiederholschleife unten verbrauchen, bevor sie
    // trotzdem (mit Warnung) durchkommt.
    bool inherentlyNonInteger = (task.ruleName == "Decimal");
    if (mode == TaskMode::MentalMath && !inherentlyNonInteger && std::abs(value - std::round(value)) > 0.001) {
        qDebug() << "[ArithmeticUnit] Unplausibel: Kopfrechnen-Ergebnis" << value << "ist keine Ganzzahl";
        return false;
    }

    int totalLength = task.writtenCalculation.expression.length() + task.writtenCalculation.answerDigitCount;
    if (totalLength > kMaxExpressionAndAnswerLength) {
        qDebug() << "[ArithmeticUnit] Unplausibel: Aufgabe zu lang fuers Raster (" << totalLength << "Zeichen):" << task.promptText;
        return false;
    }

    return true;
}

Task generateArithmeticTask(DifficultyLevel level, const QStringList &activeSubcategories, TaskMode mode)
{
    // Erzeugen, pruefen, bei Fehlschlag neu wuerfeln - MAXIMAL 20 Versuche. Ohne
    // Obergrenze waere eine Endlosschleife moeglich, sobald eine Level-Regel
    // unerfuellbar ist (z.B. ein ungluecklich zusammenspielendes Generator-Set) -
    // nach 20 Versuchen wird die letzte Aufgabe trotz Bedenken genommen und
    // deutlich sichtbar gewarnt, statt die App haengen zu lassen.
    const int maxAttempts = 20;
    Task task;

    for (int attempt = 1; attempt <= maxAttempts; ++attempt) {
        task = generateArithmeticTaskOnce(level, activeSubcategories, mode);

        if (isPlausible(task, level, mode)) {
            return task;
        }

        qDebug() << "[ArithmeticUnit] Versuch" << attempt << "von" << maxAttempts << "unplausibel - neu wuerfeln.";
    }

    qWarning() << "[ArithmeticUnit] Keine plausible Aufgabe nach" << maxAttempts << "Versuchen - nehme die letzte:" << task.promptText;
    return task;
}

QStringList arithmeticAvailableSubcategories(DifficultyLevel level)
{
    QStringList available;
    available << "Addition" << "Subtraktion" << "Multiplikation" << "Division" << "Prozentrechnung" << "Größen & Einheiten";

    if (level >= RootPowerLogCriteria::PowerMinLevel) available << "Potenz";
    if (level >= RootPowerLogCriteria::RootMinLevel) available << "Wurzel";
    if (level >= RootPowerLogCriteria::LogMinLevel) available << "Logarithmus";
    if (level >= TermCriteria::ParenthesesMinLevel) available << "Klammern & Terme";
    if (level >= NegativeNumberCriteria::MinLevel) available << "Negative Zahlen";
    if (level >= DivisibilityCriteria::MinLevel) available << "Teilbarkeit (ggT/kgV)";
    if (level >= DecimalCriteria::MinLevel) available << "Dezimalzahlen";

    qDebug() << "[ArithmeticUnit] Verfuegbare Unterkategorien bei Level" << level << ":" << available;
    return available;
}

// Q_ASSERT ist ein Qt-Makro: prueft eine Bedingung, und wenn sie falsch ist, bricht
// es im Debug-Build mit einer Meldung (inkl. Datei+Zeile) ab - im Release-Build
// (QT_NO_DEBUG definiert) wird das Makro komplett wegkompiliert, kostet dort also
// nichts. combineWithOperator() ist rein deterministisch (kein rand() darin), die
// Testfaelle sind deshalb feste Werte statt zufaellig erzeugter Fragmente.
void runCombineSelfTest()
{
    auto makeFragment = [](double value, const QString &display, Precedence precedence) {
        TaskFragment fragment;
        fragment.value = value;
        fragment.display = display;
        fragment.precedence = precedence;
        return fragment;
    };

    TaskFragment result;

    result = combineWithOperator(makeFragment(7, "3 + 4", Precedence::Line), makeFragment(2, "2", Precedence::Atom), "×");
    Q_ASSERT(result.display == "(3 + 4) × 2");
    Q_ASSERT(qFuzzyCompare(result.value, 14.0));

    result = combineWithOperator(makeFragment(12, "3 × 4", Precedence::Point), makeFragment(2, "2", Precedence::Atom), "+");
    Q_ASSERT(result.display == "3 × 4 + 2");
    Q_ASSERT(qFuzzyCompare(result.value, 14.0));

    result = combineWithOperator(makeFragment(12, "12", Precedence::Atom), makeFragment(5, "3 + 2", Precedence::Line), "-");
    Q_ASSERT(result.display == "12 - (3 + 2)");
    Q_ASSERT(qFuzzyCompare(result.value, 7.0));

    result = combineWithOperator(makeFragment(12, "12", Precedence::Atom), makeFragment(6, "3 × 2", Precedence::Point), "-");
    Q_ASSERT(result.display == "12 - 3 × 2");
    Q_ASSERT(qFuzzyCompare(result.value, 6.0));

    result = combineWithOperator(makeFragment(20, "20", Precedence::Atom), makeFragment(10, "2 × 5", Precedence::Point), "÷");
    Q_ASSERT(result.display == "20 ÷ (2 × 5)");
    Q_ASSERT(qFuzzyCompare(result.value, 2.0));

    result = combineWithOperator(makeFragment(4, "8 ÷ 2", Precedence::Point), makeFragment(2, "2", Precedence::Atom), "÷");
    Q_ASSERT(result.display == "8 ÷ 2 ÷ 2");
    Q_ASSERT(qFuzzyCompare(result.value, 2.0));

    result = combineWithOperator(makeFragment(9, "√81", Precedence::Atom), makeFragment(4, "4", Precedence::Atom), "+");
    Q_ASSERT(result.display == "√81 + 4");
    Q_ASSERT(qFuzzyCompare(result.value, 13.0));

    // Neue Klammerregel (siehe wrapIfNeeded() in fragment_algebra.cpp): eine rohe
    // negative Zahl ("-4", precedence Atom wie jede andere Zahl) muss trotzdem
    // geklammert werden, sonst waere "3 + -4" statt "3 + (-4)" das Ergebnis.
    result = combineWithOperator(makeFragment(3, "3", Precedence::Atom), makeFragment(-4, "-4", Precedence::Atom), "+");
    Q_ASSERT(result.display == "3 + (-4)");
    Q_ASSERT(qFuzzyCompare(result.value, -1.0));

    qDebug() << "[ArithmeticUnit] Selbsttest fuer combineWithOperator() erfolgreich (8/8 Faelle).";
}