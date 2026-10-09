#ifndef DIFFICULTY_H
#define DIFFICULTY_H

// Schwierigkeit ist einfach eine Zahl von 1 bis 100 -
// je höher, desto anspruchsvoller die Aufgaben.
using DifficultyLevel = int;

// Klassenstufe (3-10) -> Level-Skala. An dieser einen Stelle definiert, damit sowohl
// MainWindow (Settings-Seite) als auch der Generator-Pruefstand (generator-bench)
// dieselbe Umrechnung nutzen, statt sie zweimal zu pflegen. "inline" ist hier noetig,
// weil diese Definition (nicht nur eine Deklaration) in mehreren .cpp-Dateien ueber
// dieses Header landen kann - ohne "inline" wuerde der Linker das als mehrfach
// definiertes Symbol ablehnen. "constexpr" zusaetzlich, damit z.B.
// RectangleCriteria::MinLevel = classToLevel(4) als Konstante zur COMPILEZEIT
// berechnet werden kann (wie RootPowerLogCriteria::PowerMinLevel es fruehrer mit einer
// nackten Zahl tat - das ist jetzt F16 Teil 2, siehe unten).
//
// F16/Variante A: Kl.10 soll auf Level 75 liegen (siehe CLAUDE.md), nicht auf 71 wie
// bei der vorherigen reinen 10er-Schrittweite "(c-3)*10+1". 74 Level-Punkte (von 1 bis
// 75) lassen sich aber nicht ganzzahlig durch die 7 Klassenschritte (Kl.3->Kl.10)
// teilen (74/7 ist keine Ganzzahl) - deshalb wechselt die Schrittweite zwischen 11 und
// 10: "stepsFromClass3 / 2" (Ganzzahl-Division, rundet ab) zieht von den eigentlich
// 11 Punkten pro Schritt bei jedem ZWEITEN Schritt genau 1 Punkt wieder ab. Ergebnis:
// Kl.3=1, Kl.4=12, Kl.5=22, Kl.6=33, Kl.7=43, Kl.8=54, Kl.9=64, Kl.10=75 - eine lokale
// Variable in einer constexpr-Funktion ist seit C++14 erlaubt (einfache Rechnung ohne
// Schleifen/Seiteneffekte reicht hier aus, wie im Kommentar oben schon beschrieben).
inline constexpr DifficultyLevel classToLevel(int schoolClass)
{
    int stepsFromClass3 = schoolClass - 3;
    return 1 + 11 * stepsFromClass3 - stepsFromClass3 / 2;
}

// Ein "namespace" bündelt zusammengehörige Konstanten unter einem gemeinsamen Namen,
// damit man z.B. "Preset::Beginner" schreibt statt nur "Beginner" (Verwechslungsgefahr vermeiden).
// Vorher standen hier eigene, von classToLevel() UNABHAENGIGE Werte (1/25/50/75/100) mit
// Klassen-Kommentaren, die beim Nachrechnen gar nicht zur damaligen Formel passten
// (F16: "Elementary=25 hieß dort 'Kl.5', tatsächlich 21" usw.). Jetzt direkt ueber
// classToLevel() ausgedrueckt, damit Konstante und Kommentar nie wieder auseinanderlaufen
// koennen - nur Preset::Beginner wird aktuell im Code tatsaechlich verwendet
// (SessionController-Standardwert), der Rest bleibt vorbereitete Dokumentation.
namespace Preset {
constexpr DifficultyLevel Beginner     = classToLevel(3);    // Kl.3, Level 1
constexpr DifficultyLevel Elementary   = classToLevel(5);    // Kl.5, Level 22
constexpr DifficultyLevel Intermediate = classToLevel(7);    // Kl.7, Level 43
constexpr DifficultyLevel Advanced     = classToLevel(9);    // Kl.9, Level 64
constexpr DifficultyLevel Expert       = classToLevel(10);   // Kl.10, Level 75 - das obere Ende des aktuell mit Inhalt gefuellten Lehrplans (die volle Skala ginge bis 100)
}

// Der Modus ist UNABHAENGIG vom Level (Klassenstufe) - er bestimmt, WELCHE ART
// von Aufgabe innerhalb des aktuellen Levels erzeugt wird. In der Sidebar sind
// die drei Werte ueber sich gegenseitig ausschliessende Buttons waehlbar.
enum class TaskMode {
    MentalMath,   // "Kopfrechnen": kleine, rundungsfreundliche Zahlen, kurze Aufgaben, schnelles Weiterspringen
    Calculator,   // "mit Taschenrechner": groessere/unrundere Zahlen, die man kaum noch im Kopf loest
    Hard          // "Schwere Aufgabe": laengere Ketten (mehr verkettete Operatoren), mehr Zeit zum Loesen
};

#endif
