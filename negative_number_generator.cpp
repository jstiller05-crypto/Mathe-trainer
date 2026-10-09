#include "negative_number_generator.h"
#include "fragment_algebra.h"
#include "random_utils.h"
#include <QDebug>

// Eigene, bewusst einfache Umrechnung (wie jeder Generator).
static int maxOperandForLevel(DifficultyLevel level, bool mentalMath)
{
    int base = 5 + level / 5;
    return mentalMath ? base : base * 2;
}

// Bewusst KEINE Sonderbehandlung fuer negative Werte hier - QString::number(-7)
// liefert ganz normal "-7" (mit Minuszeichen), noch OHNE Klammern. Die Klammerung
// uebernimmt combineWithOperator()/wrapIfNeeded() automatisch (siehe
// fragment_algebra.cpp: "Anzeige beginnt mit '-'") - genau dieser Fall hat die
// Klammer-Luecke dort ueberhaupt erst aufgedeckt, weil bisher kein Generator ein
// Fragment mit fuehrendem Minuszeichen erzeugt hat.
static TaskFragment makeNumberFragment(int value)
{
    TaskFragment fragment;
    fragment.value = value;
    fragment.display = QString::number(value);
    fragment.precedence = Precedence::Atom;
    return fragment;
}

static Task buildTaskFromResult(const TaskFragment &result, bool mentalMath)
{
    Task task;
    task.ruleName = "NegativeNumber";
    task.promptText = result.display + " =";
    task.answers.append({ "", result.value });
    task.autoAdvance = mentalMath;

    QString answerStr = QString::number(result.value);
    task.writtenCalculation.expression = task.promptText;
    task.writtenCalculation.answerDigitCount = answerStr.length();
    task.writtenCalculation.freeformAnswer = answerStr.contains('.') || answerStr.contains('-');
    task.writtenCalculation.mode = WrittenCalculation::DisplayMode::SingleLine;

    qDebug() << "[NegativeNumber] mentalMath:" << mentalMath << "|" << task.promptText;
    return task;
}

Task generateNegativeNumberTask(DifficultyLevel level, bool mentalMath)
{
    int maxOperand = maxOperandForLevel(level, mentalMath);
    int form = randomInt(0, 3);

    TaskFragment result;

    switch (form) {
    case 0: {
        // Addition, mindestens ein Operand negativ: "(-3) + 5 ="
        int a = -randomInt(1, maxOperand);
        int b = randomInt(1, maxOperand);
        result = combineWithOperator(makeNumberFragment(a), makeNumberFragment(b), "+");
        break;
    }
    case 1: {
        // Subtraktion, negativer zweiter Operand - die Minus-Minus-Falle: "4 - (-7) ="
        int a = randomInt(1, maxOperand);
        int b = -randomInt(1, maxOperand);
        result = combineWithOperator(makeNumberFragment(a), makeNumberFragment(b), "-");
        break;
    }
    case 2: {
        // Multiplikation, beide Operanden negativ -> positives Ergebnis: "(-2) × (-6) ="
        int a = -randomInt(1, maxOperand);
        int b = -randomInt(1, maxOperand);
        result = combineWithOperator(makeNumberFragment(a), makeNumberFragment(b), "×");
        break;
    }
    default: {
        // Division, geht IMMER glatt auf (wie bei den bestehenden Divisions-
        // Generatoren: Divisor und Quotient zuerst waehlen, der Dividend ergibt sich
        // daraus) - hier zusaetzlich der Dividend negativ: "(-12) ÷ 3 ="
        int divisor = randomInt(1, maxOperand);
        int quotient = randomInt(1, maxOperand);
        int dividend = -(divisor * quotient);
        result = combineWithOperator(makeNumberFragment(dividend), makeNumberFragment(divisor), "÷");
        break;
    }
    }

    return buildTaskFromResult(result, mentalMath);
}

TaskFragment generateNegativeNumberFragment(DifficultyLevel level, bool mentalMath)
{
    int maxOperand = maxOperandForLevel(level, mentalMath);
    int value = -randomInt(1, maxOperand);

    // Anders als makeNumberFragment() oben wird HIER bewusst schon selbst geklammert:
    // dieses Fragment wird als fertiger Baustein exportiert (siehe
    // arithmetic_unit::fragmentForSubcategory()) und soll unabhaengig davon, WIE es
    // spaeter kombiniert wird, sofort korrekt aussehen.
    TaskFragment fragment;
    fragment.value = value;
    fragment.display = QString("(%1)").arg(value);
    fragment.precedence = Precedence::Atom;
    return fragment;
}
