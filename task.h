#ifndef TASK_H
#define TASK_H

#include <QString>
#include <QStringList>
#include <QVector>

struct AnswerSlot {
    QString label;
    double expectedValue;
};

// Beschreibt, WIE eine Aufgabe als schriftliches Rechenverfahren dargestellt wird.
// operands.isEmpty() == true bedeutet "nicht anwendbar", dann bleibt die alte,
// einzeilige Darstellung (fuer Kopfrechnen-Aufgaben) aktiv.
struct WrittenCalculation {
    QStringList operands;      // z.B. {"47", "38"} fuer 47 + 38
    QString operatorSymbol;     // "+", "-", "×"
    int answerDigitCount = 0;    // wie viele Ziffern-Kaestchen fuer das Ergebnis
};

struct Task {
    QString ruleName;
    QString promptText;
    QVector<AnswerSlot> answers;
    bool autoAdvance = true;
    WrittenCalculation writtenCalculation;   // leer = normale Kopfrechnen-Darstellung
};

#endif