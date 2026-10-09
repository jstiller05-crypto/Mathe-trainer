#ifndef DIVISIBILITY_GENERATOR_H
#define DIVISIBILITY_GENERATOR_H

#include "task.h"
#include "task_fragment.h"
#include "difficulty.h"

// Kriterium lokal hier dokumentiert (wie bei den anderen Generatoren).
namespace DivisibilityCriteria {
inline constexpr DifficultyLevel MinLevel = classToLevel(5);  // F16: classToLevel(5) statt der nackten Zahl 21
}

// ggT und kgV - im Projekt die ersten beiden Rechenoperationen, die einen
// ALGORITHMUS statt einer einfachen Formel brauchen (siehe computeGcd() im .cpp).
Task generateDivisibilityTask(DifficultyLevel level, bool mentalMath);
TaskFragment generateDivisibilityFragment(DifficultyLevel level, bool mentalMath);

// Selbsttest fuer computeGcd()/computeLcm() (die beiden sind static/intern im .cpp,
// deshalb dieser Umweg) - rein deterministisch. Von main.cpp einmal beim Start
// aufgerufen, dort eingerahmt von #ifndef QT_NO_DEBUG (analog zu runCombineSelfTest()
// in arithmetic_unit.h).
void runDivisibilitySelfTest();

#endif
