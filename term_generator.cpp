#include "term_generator.h"
#include "fragment_algebra.h"
#include "random_utils.h"
#include <QDebug>

// Eigene, bewusst einfache Umrechnung (wie jeder Generator, siehe CLAUDE.md) - waechst
// langsamer als bei reiner Addition, weil ein Term immer MEHRERE Operationen kombiniert
// und dadurch schon bei kleineren Operanden schnell unhandlich gross wird.
static int maxOperandForLevel(DifficultyLevel level, bool mentalMath)
{
    int base = 8 + level / 4;
    return mentalMath ? base : base * 2;
}

static TaskFragment makeNumberFragment(int value)
{
    TaskFragment fragment;
    fragment.value = value;
    fragment.display = QString::number(value);
    fragment.precedence = Precedence::Atom;
    return fragment;
}

static Task buildTaskFromTerm(const TaskFragment &term, bool mentalMath)
{
    Task task;
    task.ruleName = "Term";
    task.promptText = term.display + " =";
    task.answers.append({ "", term.value });
    task.autoAdvance = mentalMath;

    // Wie die anderen SingleLine-Generatoren: kein Operanden-Schema fuers Stacked-
    // Raster (mehrere unterschiedliche Operatoren + Klammern passen dort nicht rein).
    QString answerStr = QString::number(term.value);
    task.writtenCalculation.expression = task.promptText;
    task.writtenCalculation.answerDigitCount = answerStr.length();
    task.writtenCalculation.freeformAnswer = answerStr.contains('.') || answerStr.contains('-');
    task.writtenCalculation.mode = WrittenCalculation::DisplayMode::SingleLine;

    qDebug() << "[Term] mentalMath:" << mentalMath << "|" << task.promptText;
    return task;
}

TaskFragment generateTermFragment(DifficultyLevel level, bool mentalMath)
{
    int maxOperand = maxOperandForLevel(level, mentalMath);
    int a = randomInt(1, maxOperand);
    int b = randomInt(1, maxOperand);
    QString op = randomChance(50) ? "+" : "-";

    TaskFragment inner = combineWithOperator(makeNumberFragment(a), makeNumberFragment(b), op);

    TaskFragment result;
    result.value = inner.value;
    result.display = QString("(%1)").arg(inner.display);
    result.precedence = Precedence::Atom;   // geklammert = wieder ein Atom fuer die Regel (siehe fragment_algebra.h)
    return result;
}

Task generateTermTask(DifficultyLevel level, bool mentalMath)
{
    int maxOperand = maxOperandForLevel(level, mentalMath);
    bool divisionAllowed = (level >= TermCriteria::DivisionMinLevel);
    bool nestedAllowed = (level >= TermCriteria::NestedMinLevel);

    QString pointOp = (divisionAllowed && randomChance(50)) ? "÷" : "×";
    QString lineOp = randomChance(50) ? "+" : "-";
    // Reihenfolge der Verknuepfung und Seite zufaellig waehlen - zusammen mit
    // combineWithOperator() (Schritt 2), das die Klammern automatisch nach
    // Bindungsstaerke setzt, ergeben sich daraus alle Grundformen aus der Aufgabe:
    // "2 × (3 + 4)", "20 - 3 × 4", "(12 + 8) ÷ 5", "3 × 4 - 7" ...
    bool pointPairFirst = randomChance(50);
    bool innerOnLeft = randomChance(50);

    TaskFragment fragA;
    TaskFragment fragB;
    TaskFragment fragC;

    if (pointPairFirst) {
        // Das Punkt-Paar (× oder ÷) wird zuerst verknuepft. Bei Division vom
        // Ergebnis her gedacht (Divisor + Quotient waehlen), damit sie garantiert
        // glatt aufgeht, statt hinterher zu pruefen und neu zu wuerfeln.
        if (pointOp == "÷") {
            int divisor = randomInt(2, 10);
            int quotient = randomInt(1, maxOperand);
            fragA = makeNumberFragment(divisor * quotient);
            fragB = makeNumberFragment(divisor);
        } else {
            fragA = makeNumberFragment(randomInt(1, maxOperand));
            // Genestete Klammer (z.B. "3 × (4 + 2) − 7") nur bei × moeglich - ihr
            // Wert waere als Divisor/Dividend sonst nicht kontrollierbar.
            fragB = (nestedAllowed && randomChance(50)) ? generateTermFragment(level, mentalMath)
                                                          : makeNumberFragment(randomInt(1, maxOperand));
        }
        fragC = makeNumberFragment(randomInt(1, maxOperand));

        TaskFragment inner = combineWithOperator(fragA, fragB, pointOp);
        TaskFragment term = innerOnLeft ? combineWithOperator(inner, fragC, lineOp)
                                          : combineWithOperator(fragC, inner, lineOp);
        return buildTaskFromTerm(term, mentalMath);
    }

    // Das Strich-Paar (+ oder -) wird zuerst verknuepft.
    if (pointOp == "÷") {
        // Sonderfall "(a + b) ÷ c": Divisor c und Quotient q zuerst waehlen, dann
        // a + b = c*q so in a und b splitten, dass die Summe exakt c*q ergibt - die
        // Division geht dadurch auch in dieser Reihenfolge garantiert glatt auf
        // (deshalb hier ausnahmsweise IMMER "+" statt des gewuerfelten lineOp).
        int divisor = randomInt(2, 10);
        int quotient = randomInt(1, maxOperand);
        int sum = divisor * quotient;
        int a = randomInt(1, sum - 1);
        fragA = makeNumberFragment(a);
        fragB = makeNumberFragment(sum - a);
        fragC = makeNumberFragment(divisor);

        TaskFragment inner = combineWithOperator(fragA, fragB, "+");
        TaskFragment term = innerOnLeft ? combineWithOperator(inner, fragC, pointOp)
                                          : combineWithOperator(fragC, inner, pointOp);
        return buildTaskFromTerm(term, mentalMath);
    }

    fragA = makeNumberFragment(randomInt(1, maxOperand));
    fragB = (nestedAllowed && randomChance(50)) ? generateTermFragment(level, mentalMath)
                                                  : makeNumberFragment(randomInt(1, maxOperand));
    fragC = makeNumberFragment(randomInt(1, maxOperand));

    TaskFragment inner = combineWithOperator(fragA, fragB, lineOp);
    TaskFragment term = innerOnLeft ? combineWithOperator(inner, fragC, pointOp)
                                      : combineWithOperator(fragC, inner, pointOp);
    return buildTaskFromTerm(term, mentalMath);
}
