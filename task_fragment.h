#ifndef TASK_FRAGMENT_H
#define TASK_FRAGMENT_H

#include <QString>

// Ein "Puzzleteil" einer Aufgabe: ein Generator kann entweder eine KOMPLETTE
// Aufgabe erzeugen (Task, siehe task.h), oder nur ein Fragment - ein einzelner
// Wert, der SPAETER als Operand in einer anderen Aufgabe verwendet werden kann,
// OHNE dass seine urspruengliche Darstellung verloren geht.
//
// Beispiel: der Wurzel/Potenz-Generator erzeugt fuer "4 hoch 2" das Fragment
// { value = 16, display = "4^2" } - der WERT ist 16, aber ANGEZEIGT wird "4^2",
// nicht "16". So kann eine spaetere Aufgabe wie "4^2 + 8" entstehen, ohne dass
// die Potenz-Schreibweise verloren geht.
// Bindungsstaerke eines Fragments - wird beim Verketten zweier Fragmente gebraucht,
// um zu entscheiden, ob Klammern noetig sind (siehe fragment_algebra.h). Die Zahlen
// sind BEWUSST so vergeben, dass "bindet fester" eine GROESSERE Zahl ist - die
// Klammerregel vergleicht dann einfach f.precedence < opPrecedence.
//
// "enum class" (im Unterschied zu einem "nackten" enum) hat KEINE implizite
// Umwandlung nach int - genau das ist hier gewollt, damit man eine Precedence nicht
// aus Versehen mit einer beliebigen Zahl vermischt. Der Nebeneffekt: der eingebaute
// Operator< ist fuer enum class NICHT definiert, "a < b" mit zwei Precedence-Werten
// waere also ein Kompilierfehler. Ein Zahlenvergleich braucht deshalb immer den
// expliziten Umweg static_cast<int>(a) < static_cast<int>(b).
enum class Precedence { Atom = 3, Point = 2, Line = 1 };
// Atom  = einzelner Wert oder bereits geklammert: 7, √81, 4², (3 + 4)
// Point = äußerster Operator ist × oder ÷
// Line  = äußerster Operator ist + oder −

struct TaskFragment {
    double value;      // reiner Zahlenwert, fuer die Weiterverwendung als Operand
    QString display;    // wie es TATSAECHLICH angezeigt werden soll
    Precedence precedence = Precedence::Atom;   // s.o. - Standard: einzelner Wert
};

#endif