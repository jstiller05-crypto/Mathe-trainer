#ifndef GEOMETRY_UNIT_H
#define GEOMETRY_UNIT_H

#include "task.h"
#include "difficulty.h"
#include <QStringList>

// Nabe fuer die Top-Kategorie "Geometrie" - Aufbau bewusst analog zu
// arithmetic_unit.h/.cpp (siehe dort): nimmt Level, aktive Unterkategorien und
// Modus entgegen, waehlt die passende Speiche (aktuell nur "Rechteck") und reicht
// die fertige Aufgabe nur weiter, wie es das Sternsystem aus CLAUDE.md vorsieht.
Task generateGeometryTask(DifficultyLevel level, const QStringList &activeSubcategories, TaskMode mode);
QStringList geometryAvailableSubcategories(DifficultyLevel level);

// Struktureller Rauchtest (siehe geometry_unit.cpp fuer den Grund, warum kein
// Q_ASSERT-Determinismus-Test wie runCombineSelfTest() moeglich ist). Von main.cpp
// einmal beim Start aufgerufen, dort eingerahmt von #ifndef QT_NO_DEBUG (analog zu
// runCombineSelfTest()/runDivisibilitySelfTest()).
void runGeometrySelfTest();

#endif
