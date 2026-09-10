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

// Der Modus ist UNABHAENGIG vom Level (Klassenstufe) - er bestimmt, WELCHE ART
// von Aufgabe innerhalb des aktuellen Levels erzeugt wird. In der Sidebar sind
// die drei Werte ueber sich gegenseitig ausschliessende Buttons waehlbar.
enum class TaskMode {
    MentalMath,   // "Kopfrechnen": kleine, rundungsfreundliche Zahlen, kurze Aufgaben, schnelles Weiterspringen
    Calculator,   // "mit Taschenrechner": groessere/unrundere Zahlen, die man kaum noch im Kopf loest
    Hard          // "Schwere Aufgabe": laengere Ketten (mehr verkettete Operatoren), mehr Zeit zum Loesen
};

// Klassenstufe (3-10) -> Level-Skala. An dieser einen Stelle definiert, damit sowohl
// MainWindow (Settings-Seite) als auch der Generator-Pruefstand (generator-bench)
// dieselbe Umrechnung nutzen, statt sie zweimal zu pflegen. "inline" ist hier noetig,
// weil diese Definition (nicht nur eine Deklaration) in mehreren .cpp-Dateien ueber
// dieses Header landen kann - ohne "inline" wuerde der Linker das als mehrfach
// definiertes Symbol ablehnen. "constexpr" zusaetzlich, damit z.B.
// RectangleCriteria::MinLevel = classToLevel(4) als Konstante zur COMPILEZEIT
// berechnet werden kann (wie RootPowerLogCriteria::PowerMinLevel es mit einer
// nackten Zahl tut) - der Funktionskoerper ist simpel genug (eine Rechnung, kein
// Zustand), um das ohne weiteres zu erlauben.
inline constexpr DifficultyLevel classToLevel(int schoolClass) { return (schoolClass - 3) * 10 + 1; }

#endif
