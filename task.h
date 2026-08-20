#ifndef TASK_H
#define TASK_H

#include <QString>
#include <QVector>
#include "topic.h"

// Ein einzelnes Antwortfeld. Bei einfachen Kopfrechenaufgaben gibt's nur eins
// (label bleibt leer), bei komplexeren Aufgaben (z.B. Dreieck) mehrere gleichzeitig.
struct AnswerSlot {
    QString label;          // Beschriftung über dem Feld, z.B. "Hypotenuse c" - leer bei einfachen Aufgaben
    double expectedValue;    // die erwartete, richtige Lösung für GENAU dieses Feld
};

struct Task {
    QString ruleName;
    QString promptText;
    QVector<AnswerSlot> answers;
    bool autoAdvance = true;
};

#endif // TASK_H
