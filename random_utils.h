#ifndef RANDOM_UTILS_H
#define RANDOM_UTILS_H

#include <QtGlobal>

// Zentraler Zufalls-Helfer fuer ALLE Generatoren/Units - reine Mechanik (wie
// fragment_algebra.h/number_format.h), OHNE eigene Level- oder Aufgaben-Logik, nur
// an EINER Stelle definiert statt dass jede Datei selbst mit rand() hantiert.
//
// Ersetzt das bisherige rand()/RAND_MAX aus <cstdlib>, das zwei Probleme hatte:
// - F17: Windows definiert RAND_MAX nur als 32767 (nicht wie z.B. unter Linux als
//   deutlich groesserer Wert) - "rand() % N" fuer ein grosses N liefert dort also
//   nie Werte ueber 32767, egal wie gross N eigentlich sein sollte.
// - F23: "rand() / RAND_MAX" (als double gerechnet) kann exakt 1.0 ergeben, naemlich
//   genau dann, wenn rand() den Maximalwert RAND_MAX selbst liefert - ein angeblicher
//   Wert "im Bereich [0,1)" konnte also in Wahrheit genau 1 sein.
// QRandomGenerator (seit Qt 5.10) hat beide Probleme nicht: bounded() arbeitet
// unabhaengig von einer plattformspezifischen RAND_MAX-Konstante, und
// generateDouble() ist laut Qt-Dokumentation garantiert < 1.0.

// Ganzzahl im Bereich [min, max] - BEIDE Grenzen eingeschlossen. QRandomGenerator::
// bounded(lowest, highest) liefert Werte aus [lowest, highest) - die obere Grenze ist
// dort EXKLUSIV - deshalb wird hier intern "max + 1" als obere Schranke uebergeben,
// damit "max" selbst noch ein moegliches Ergebnis ist.
int randomInt(int min, int max);

// Gleitkommazahl im Bereich [0, 1) - 1 ist AUSGESCHLOSSEN (siehe F23-Erklaerung oben).
double randomUnitInterval();

// Zufaelliges Ja/Nein mit genau "percent" Prozent Wahrscheinlichkeit fuer true -
// Ersatz fuer die vielen "rand() % 2 == 0" (randomChance(50)) bzw. "roll < 70"
// (randomChance(70)) im bisherigen Code.
bool randomChance(int percent);

// Setzt den Startwert (Seed) des hier verwendeten Zufallsgenerators neu. Ohne diesen
// Aufruf ist der Generator zufaellig (sicher) geseedet, siehe random_utils.cpp -
// generator-bench kann hiermit trotzdem reproduzierbare Laeufe erzwingen (--seed, F27).
void setRandomSeed(quint32 seed);

#endif
