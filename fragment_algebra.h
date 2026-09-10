#ifndef FRAGMENT_ALGEBRA_H
#define FRAGMENT_ALGEBRA_H

#include "task_fragment.h"
#include <QString>

// Reine Mechanik rund um das Verketten zweier TaskFragments mit einem Operator -
// KEIN Zufall, KEINE Level-Logik (das bleibt Sache der Generatoren bzw. der Unit).
// Liegt bewusst NEBEN task_fragment.h und NICHT in arithmetic_unit.h/.cpp, damit
// auch zukuenftige Speichen (z.B. term_generator) diese Funktionen direkt nutzen
// koennen, OHNE von der Unit abzuhaengen - eine Speiche, die die Unit importiert,
// waere im Sternsystem (siehe CLAUDE.md) eine Abhaengigkeit in die falsche Richtung:
// die Unit kennt ihre Speichen, niemals umgekehrt.

// "+" / "-" binden schwaecher (Line), "×" / "÷" staerker (Point).
Precedence precedenceOfOperator(const QString &op);

// Liefert f.display, ggf. in Klammern - genau dann, wenn f als Operand des Operators
// mit opPrecedence sonst mehrdeutig oder schlicht falsch waere:
//   - f.precedence < opPrecedence (ein schwaecher bindender Ausdruck wird Operand
//     eines staerker bindenden Operators, z.B. "3 + 4" als Operand von "×")
//   - ZUSAETZLICH, wenn f RECHTS von einem − oder ÷ steht UND genauso stark bindet
//     wie der Operator selbst - − und ÷ sind nicht assoziativ ("12 − (3 + 2)" darf
//     nicht zu "12 − 3 + 2" werden, und "20 ÷ (2 × 5)" nicht zu "20 ÷ 2 × 5"), ein
//     Gleichstand ist hier also schon fuer sich genommen ein Grund zu klammern -
//     anders als bei + und ×, wo Gleichstand rechts unproblematisch ist.
//   - ZUSAETZLICH, wenn f.display mit "-" beginnt (eine rohe, noch nicht geklammerte
//     negative Zahl) - unabhaengig von Precedence UND Seite, sonst waere "3 + -4"
//     bzw. "-4 + 3" das Ergebnis statt "3 + (-4)" bzw. "(-4) + 3".
QString wrapIfNeeded(const TaskFragment &f, Precedence opPrecedence,
                      bool isRightOperand, bool opIsMinusOrDivide);

// Baut aus zwei Fragmenten und einem Operator ("+", "-", "×", "÷") ein neues Fragment:
// display verwendet wrapIfNeeded() fuer beide Seiten, value wird tatsaechlich
// ausgerechnet, precedence des Ergebnisses ist precedenceOfOperator(op). Absichtlich
// deterministisch (kein rand() hier) - das macht die Funktion in Schritt 5 testbar.
TaskFragment combineWithOperator(const TaskFragment &a, const TaskFragment &b, const QString &op);

#endif
