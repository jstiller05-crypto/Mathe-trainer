#ifndef ARITHMETIC_UNIT_H
#define ARITHMETIC_UNIT_H

#include "task.h"
#include "difficulty.h"
#include <QStringList>

Task generateArithmeticTask(DifficultyLevel level, const QStringList &activeSubcategories, TaskMode mode);
QStringList arithmeticAvailableSubcategories(DifficultyLevel level);

// Selbsttest fuer combineWithOperator()/wrapIfNeeded() (fragment_algebra.h) - rein
// deterministisch, kein Zufall beteiligt. Von main.cpp einmal beim Start aufgerufen,
// dort eingerahmt von #ifndef QT_NO_DEBUG (siehe dortiger Kommentar).
void runCombineSelfTest();

#endif