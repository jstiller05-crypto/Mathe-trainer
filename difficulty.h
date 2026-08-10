#ifndef DIFFICULTY_H
#define DIFFICULTY_H

// Schwierigkeit ist einfach eine Zahl von 1 bis 100 -
// je höher, desto anspruchsvoller die Aufgaben.
using DifficultyLevel = int;

// Ein "namespace" bündelt zusammengehörige Konstanten unter einem gemeinsamen Namen,
// damit man z.B. "Preset::Beginner" schreibt statt nur "Beginner" (Verwechslungsgefahr vermeiden)
namespace Preset {
constexpr DifficultyLevel Beginner     = 1;    // ca. Klasse 3
constexpr DifficultyLevel Elementary   = 25;   // ca. Klasse 5
constexpr DifficultyLevel Intermediate = 50;   // ca. Klasse 7
constexpr DifficultyLevel Advanced     = 75;   // ca. Klasse 9
constexpr DifficultyLevel Expert       = 100;  // ca. Klasse 10+
}

#endif
