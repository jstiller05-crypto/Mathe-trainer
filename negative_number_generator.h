#ifndef NEGATIVE_NUMBER_GENERATOR_H
#define NEGATIVE_NUMBER_GENERATOR_H

#include "task.h"
#include "task_fragment.h"
#include "difficulty.h"

// Kriterium lokal hier dokumentiert (wie bei den anderen Generatoren).
namespace NegativeNumberCriteria {
// Gleiche Schwelle wie negativeResultsAllowed() in addition_subtraction_generator.cpp
// und kNegativeResultsMinLevel in arithmetic_unit.cpp - ab hier duerfen Ergebnisse
// ueberhaupt negativ sein, deshalb macht eine EIGENE Uebung fuer Vorzeichenregeln
// erst ab demselben Level Sinn.
inline constexpr DifficultyLevel MinLevel = classToLevel(6);   // F16: classToLevel(6) statt der nackten Zahl 31
}

// Eigene Uebung fuer Vorzeichenregeln (Kl.6/7) - nicht nur "Ergebnis darf negativ
// sein" (das deckt addition_subtraction_generator ab Level 31 schon ab), sondern
// gezielt Addition/Subtraktion/Multiplikation/Division MIT negativen Operanden.
Task generateNegativeNumberTask(DifficultyLevel level, bool mentalMath);
TaskFragment generateNegativeNumberFragment(DifficultyLevel level, bool mentalMath);

#endif
