#ifndef DECIMAL_GENERATOR_H
#define DECIMAL_GENERATOR_H

#include "task.h"
#include "task_fragment.h"
#include "difficulty.h"

// Kriterium lokal hier dokumentiert (wie bei den anderen Generatoren).
namespace DecimalCriteria {
inline constexpr DifficultyLevel MinLevel = 21;  // Kl.5
}

// Rechnen mit Dezimalbruechen - MIT echter Komma-Anzeige im Aufgabentext selbst
// (nicht nur bei der Eingabe, die parseUserNumber in session_controller.cpp schon
// abdeckt), ueber formatGermanDecimal() (number_format.h).
Task generateDecimalTask(DifficultyLevel level, bool mentalMath);
TaskFragment generateDecimalFragment(DifficultyLevel level, bool mentalMath);

#endif
