#include "random_utils.h"
#include <QRandomGenerator>

// EIGENE Instanz statt bei jedem Aufruf direkt QRandomGenerator::global() zu nutzen -
// nur so kann setRandomSeed() (fuer reproduzierbare generator-bench-Laeufe, F27) gezielt
// NUR diesen einen Generator umstellen, ohne den globalen Generator zu beeinflussen, den
// Qt moeglicherweise auch intern an anderen Stellen verwendet.
// "static" (Dateisichtbarkeit, nicht zu verwechseln mit einer statischen Klassen-
// Methode) beschraenkt diese Variable auf random_utils.cpp - von aussen ist sie nur
// ueber die Funktionen unten erreichbar, nie direkt.
// QRandomGenerator::securelySeeded() liefert einen mit echtem Betriebssystem-Zufall
// (nicht nur der Uhrzeit wie das bisherige std::srand(time(nullptr))) initialisierten
// Generator - jeder App-Start bekommt dadurch eine andere Aufgabenfolge, ohne dass wir
// selbst einen Startwert aussuchen muessen.
static QRandomGenerator engine = QRandomGenerator::securelySeeded();

int randomInt(int min, int max)
{
    return engine.bounded(min, max + 1);
}

double randomUnitInterval()
{
    return engine.generateDouble();
}

bool randomChance(int percent)
{
    return randomInt(1, 100) <= percent;
}

void setRandomSeed(quint32 seed)
{
    engine.seed(seed);
}
