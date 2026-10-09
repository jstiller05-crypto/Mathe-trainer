#ifndef ROOT_POWER_LOG_GENERATOR_H
#define ROOT_POWER_LOG_GENERATOR_H

#include "task.h"
#include "task_fragment.h"
#include "difficulty.h"

// Kriterien fuer diese Generatoren - zentral HIER vermerkt, damit sofort
// klar ist, ab welchem Level was verfuegbar wird (statt verstreut im Code)
namespace RootPowerLogCriteria {
// F16: classToLevel(n) statt nackter Zahlen - folgen jetzt automatisch der zentralen
// Klasse->Level-Umrechnung (difficulty.h), statt bei einer Aenderung dort stillschweigend
// falsch zu werden.
inline constexpr DifficultyLevel PowerMinLevel = classToLevel(5);       // Kl.5 - Potenz (erstmal nur Quadrat)
inline constexpr DifficultyLevel PowerFullMinLevel = classToLevel(8);   // Kl.8 - ab hier auch groessere Exponenten
inline constexpr DifficultyLevel RootMinLevel = classToLevel(7);        // Kl.7 - Wurzel
inline constexpr DifficultyLevel LogMinLevel = classToLevel(10);        // Kl.10 - Logarithmus
}

Task generatePowerTask(DifficultyLevel level, bool mentalMath);
TaskFragment generatePowerFragment(DifficultyLevel level, bool mentalMath);

Task generateRootTask(DifficultyLevel level, bool mentalMath);
TaskFragment generateRootFragment(DifficultyLevel level, bool mentalMath);

Task generateLogTask(DifficultyLevel level, bool mentalMath);
TaskFragment generateLogFragment(DifficultyLevel level, bool mentalMath);

#endif