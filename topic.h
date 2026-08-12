#ifndef TOPIC_H
#define TOPIC_H

// Nur die Themen, die wir mit unserer aktuellen Zahlen-Architektur
// sinnvoll umsetzen können. Weitere Themen (Geometrie, Textaufgaben, ...)
// stehen auf der Agenda, bis das System dafür erweitert ist.
enum class Topic {
    BasicArithmetic,     // Kl.3-4: + - im wachsenden Zahlenraum
    OrderOfOperations,    // Kl.5: Punkt vor Strich, Klammern
    NegativeNumbers,      // Kl.6
    Percentage,            // Kl.7: Prozentrechnung
    Interest,               // Kl.7: Zinsrechnung
    Powers,                  // Kl.8-9: Potenzen, Potenzgesetze
    Roots,                    // Kl.9: Quadratwurzeln
    UnitConversion             // Kl.3-4: mm-km, g-kg-t, Uhrzeiten (eigenständig, alle Stufen)
};

#endif