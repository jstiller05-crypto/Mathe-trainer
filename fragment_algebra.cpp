#include "fragment_algebra.h"

Precedence precedenceOfOperator(const QString &op)
{
    return (op == "×" || op == "÷") ? Precedence::Point : Precedence::Line;
}

QString wrapIfNeeded(const TaskFragment &f, Precedence opPrecedence,
                      bool isRightOperand, bool opIsMinusOrDivide)
{
    bool needsParens = static_cast<int>(f.precedence) < static_cast<int>(opPrecedence);

    if (!needsParens && isRightOperand && opIsMinusOrDivide && f.precedence == opPrecedence) {
        needsParens = true;
    }

    // Ein Fragment, dessen ANZEIGE mit einem Minuszeichen beginnt (z.B. "-4", eine
    // rohe negative Zahl, noch nicht geklammert), braucht IMMER Klammern - egal ob
    // es links oder rechts vom Operator steht und unabhaengig von der Precedence
    // (eine blosse Zahl hat wie jede andere Precedence::Atom, faellt also durch die
    // Pruefung oben durch). Ohne diese Regel wuerde z.B. "3 + -4" statt "3 + (-4)"
    // entstehen. WICHTIG: das prueft nur den ANZEIGETEXT (f.display), nicht
    // f.value < 0 - ein bereits geklammertes negatives ERGEBNIS wie "(3 - 8)" beginnt
    // mit "(", nicht mit "-", und wuerde bei einer Pruefung auf f.value < 0
    // faelschlich ein zweites Mal geklammert.
    if (!needsParens && f.display.startsWith('-')) {
        needsParens = true;
    }

    return needsParens ? QString("(%1)").arg(f.display) : f.display;
}

TaskFragment combineWithOperator(const TaskFragment &a, const TaskFragment &b, const QString &op)
{
    Precedence opPrecedence = precedenceOfOperator(op);
    bool opIsMinusOrDivide = (op == "-" || op == "÷");

    QString leftDisplay = wrapIfNeeded(a, opPrecedence, false, opIsMinusOrDivide);
    QString rightDisplay = wrapIfNeeded(b, opPrecedence, true, opIsMinusOrDivide);

    TaskFragment result;
    result.display = QString("%1 %2 %3").arg(leftDisplay, op, rightDisplay);
    result.precedence = opPrecedence;

    if (op == "+")      result.value = a.value + b.value;
    else if (op == "-") result.value = a.value - b.value;
    else if (op == "×") result.value = a.value * b.value;
    else if (op == "÷") result.value = a.value / b.value;
    else                result.value = a.value + b.value;   // unbekannter Operator sollte nie vorkommen - sicherer Fallback

    return result;
}
