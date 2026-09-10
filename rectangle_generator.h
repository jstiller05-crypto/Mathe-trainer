#ifndef RECTANGLE_GENERATOR_H
#define RECTANGLE_GENERATOR_H

#include "task.h"
#include "difficulty.h"

// Kriterium lokal hier dokumentiert (wie bei den anderen Generatoren, z.B.
// RootPowerLogCriteria in root_power_log_generator.h).
namespace RectangleCriteria {
// Flaeche/Umfang eines Rechtecks passt inhaltlich zu Kl.4-5 - classToLevel(4) statt
// einer nackten Zahl, damit der Zusammenhang zur Klassenstufe beim Lesen sofort
// klar ist.
inline constexpr DifficultyLevel MinLevel = classToLevel(4);   // Kl.4
}

// Erste Speiche der Geometrie-Unit (siehe geometry_unit.h) - erzeugt eine
// Rechteck-Aufgabe UND das dazugehoerige GeometryModel (siehe geometry_model.h).
Task generateRectangleTask(DifficultyLevel level, bool mentalMath);

#endif
