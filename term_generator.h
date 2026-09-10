#ifndef TERM_GENERATOR_H
#define TERM_GENERATOR_H

#include "task.h"
#include "task_fragment.h"
#include "difficulty.h"

// Kriterien fuer diesen Generator - lokal hier dokumentiert (wie bei
// root_power_log_generator.h), statt zentral verstreut.
namespace TermCriteria {
inline constexpr DifficultyLevel MixedOperatorsMinLevel = 21;  // Kl.5 - Terme mit gemischten Ebenen (Punkt+Strich) ueberhaupt erst ab hier
inline constexpr DifficultyLevel ParenthesesMinLevel    = 21;  // Kl.5 - Unterkategorie "Klammern & Terme" wird ab hier in der Sidebar freigeschaltet
inline constexpr DifficultyLevel DivisionMinLevel       = 21;  // Kl.5 - ÷ als Punkt-Operator im Term erlaubt
inline constexpr DifficultyLevel NestedMinLevel         = 51;  // Kl.8 - Terme mit geschachtelter Klammer, z.B. "3 × (4 + 2) − 7"
}

// Eigenstaendige Speiche: erzeugt ihre Operanden selbst, keine Abhaengigkeit zu
// anderen Generatoren. Nutzt fragment_algebra.h fuer die Klammer-/Verkettungsmechanik.
Task generateTermTask(DifficultyLevel level, bool mentalMath);

// Liefert einen FERTIG GEKLAMMERTEN Teilausdruck, z.B. { value 7, display "(3 + 4)",
// precedence Atom }. Ein geklammerter Ausdruck ist fuer die Regel in fragment_algebra
// wieder ein Atom - deshalb passt dieser Generator als ganz normale Speiche in
// arithmetic_unit::fragmentForSubcategory().
TaskFragment generateTermFragment(DifficultyLevel level, bool mentalMath);

#endif
