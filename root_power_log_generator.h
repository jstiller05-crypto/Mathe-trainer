#ifndef ROOT_POWER_LOG_GENERATOR_H
#define ROOT_POWER_LOG_GENERATOR_H

#include "task.h"
#include "task_fragment.h"
#include "difficulty.h"

// Kriterien fuer diese Generatoren - zentral HIER vermerkt, damit sofort
// klar ist, ab welchem Level was verfuegbar wird (statt verstreut im Code)
namespace RootPowerLogCriteria {
inline constexpr DifficultyLevel PowerMinLevel = 21;       // Kl.5 - Potenz (erstmal nur Quadrat)
inline constexpr DifficultyLevel PowerFullMinLevel = 51;    // Kl.8 - ab hier auch groessere Exponenten
inline constexpr DifficultyLevel RootMinLevel = 41;          // Kl.7 - Wurzel
inline constexpr DifficultyLevel LogMinLevel = 71;            // Kl.10 - Logarithmus
}

Task generatePowerTask(DifficultyLevel level, bool mentalMath);
TaskFragment generatePowerFragment(DifficultyLevel level, bool mentalMath);

Task generateRootTask(DifficultyLevel level, bool mentalMath);
TaskFragment generateRootFragment(DifficultyLevel level, bool mentalMath);

Task generateLogTask(DifficultyLevel level, bool mentalMath);
TaskFragment generateLogFragment(DifficultyLevel level, bool mentalMath);

#endif