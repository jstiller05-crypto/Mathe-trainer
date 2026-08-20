#ifndef TASK_FRAGMENT_H
#define TASK_FRAGMENT_H

#include <QString>

// Ein "Puzzleteil" einer Aufgabe: ein Generator kann entweder eine KOMPLETTE
// Aufgabe erzeugen (Task, siehe task.h), oder nur ein Fragment - ein einzelner
// Wert, der SPAETER als Operand in einer anderen Aufgabe verwendet werden kann,
// OHNE dass seine urspruengliche Darstellung verloren geht.
//
// Beispiel: der Wurzel/Potenz-Generator erzeugt fuer "4 hoch 2" das Fragment
// { value = 16, display = "4²" } - der WERT ist 16, aber ANGEZEIGT wird "4²",
// nicht "16". So kann eine spaetere Aufgabe wie "4² + 8" entstehen, ohne dass
// die Potenz-Schreibweise verloren geht.
struct TaskFragment {
    double value;      // reiner Zahlenwert, fuer die Weiterverwendung als Operand
    QString display;    // wie es TATSAECHLICH angezeigt werden soll
};

#endif