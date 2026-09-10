# Mathe-Trainings-App

C++/Qt Desktop-App: tägliche, zufällig generierte Matheaufgaben zur Konzentrations- und Denkförderung. Ziel langfristig: professionelle Lernapp mit Schwierigkeitsgraden, Statistiken, geführtem Lernen, KI, Konten mit Sync – aber Schritt für Schritt, aktuell noch weit vor Release.

## Tech-Stack & Konventionen

- C++ (kein reines C++ mehr angestrebt – Qt wird für UI genutzt, weil einfacher als reines WinAPI)
- Qt für UI, CMake als Build-System
- Code (Variablen-/Funktionsnamen etc.) auf Englisch, Kommentare ausführlich, besonders an ungewöhnlichen Stellen
- qDebug() aktiv nutzen, um den Programmablauf während der Entwicklung nachvollziehbar zu machen
- Fehler beim Laden externer Ressourcen (Fonts, Icons etc.) immer abfangen/loggen, App darf dadurch nicht abstürzen
- Zielplattform aktuell: nur Windows 10. Linux und Mobile sind spätere Ziele, aber die Architektur soll das nicht verbauen
- Ich bin noch Lernender in C++ – bei Syntax, Befehlen, Funktionen bitte grundlegend erklären, nicht nur Code liefern

## Architektur: "Sternsystem" für Aufgaben-Generierung

Zentrale Idee: kein einzelner großer Generator, sondern viele kleine, unabhängige Generator-Dateien, die nur mit einer zentralen Stelle kommunizieren, nie direkt miteinander.

- `task_generator` = zentrale Nabe, reicht fertige Aufgaben nur an `task_view` weiter (spiegelt nur)
- Pro Basis-Kategorie eine "Unit" (aktuell nur `arithmetic_unit` existiert): sammelt Ergebnisse der Einzel-Generatoren, kann sie verschmelzen (z. B. Potenz-Ergebnis als Operand in eine Addition einsetzen), prüft danach nochmal auf Plausibilität
- Jeder Einzel-Generator (z. B. `addition_subtraction_generator`, `root_power_log_generator`, `percent_mult_div_generator`, `finance_generator`, `units_generator`) legt seine eigene Level→Faktor-Umrechnung selbst fest (kein zentrales einheitliches Mapping)
- Gemeinsame Pipeline pro Einzel-Generator: Länge (wie viele Operatoren verkettet) → Operator(en) wählen (Wahrscheinlichkeit abhängig vom Faktor) → Aufgabe zusammenbauen → Prüfen ob sie passt
- Weitere geplante Top-Kategorien (noch nicht gebaut): Trigonometrie, Geometrie, Algebra, Stochastik, Analysis (Analysis erst für Level jenseits Kl.10/Studenten-Niveau)

## Schwierigkeits-System

- Skala 1–100 statt fester Stufen; Faktor fließt in Aufgaben-Generierung ein
- Jede Klassenstufe (3–10) liegt auf genau einem festen Level, Zwischenwerte bleiben frei/leer aber über Regler erreichbar (Kl.10 = Level 75)
- Level-Kriterien pro Aufgaben-Art werden direkt in der jeweiligen Generator-Datei dokumentiert (nicht zentral verstreut)
- Bekannte Kriterien: Potenz ab Kl.5 (erst nur Exponent 2), Wurzel Kl.7–8, Logarithmus ab Kl.10
- "Kopfrechnen"-Modus ist ein globaler, kategorieübergreifender Schalter (nicht pro Kategorie): eingeschaltet = im Kopf lösbar (z. B. Wurzel aus 9 statt 10, max. 3 Verschmelzungen, meist 0–1), ausgeschaltet = Herausforderungs-Modus mit längeren/komplexeren Ketten
- Lehrplan-Details je Klassenstufe: siehe Chat-Historie bzw. auf Nachfrage – bei Bedarf kann ich das nachliefern

## Aktueller Stand & nächster Fokus

- Grundgerüst mit MainWindow, SessionController, Sidebar, TaskView, SymbolMenu, SettingsView, WrittenGridWidget existiert bereits
- Kategorie "Arithmetik" wird aktuell gebaut: Addition & Subtraktion, Prozent/Mult/Div, Wurzel/Potenz/Log, Finanzrechnung, Einheiten – alle sollen denselben Pipeline-Ablauf nutzen, nur mit unterschiedlichen Regel-Sets
- Priorität aktuell: Aufgaben-Generator sauber und leistungsfähig aufbauen (nicht nur zufällig, sondern regelbasiert korrekt eingestuft) – höchste Priorität vor UI-Feinschliff
- Reihenfolge generell: erst Rechenlogik fertig, dann UI, dann Richtung Release
- v1.0-Ziel: nur die Aufgaben-Funktion, aber von Anfang an so stabil/erweiterbar gebaut, dass später Features ergänzt werden können

## UI-Konzept (später dran, aber schon entschieden)

- Modernes Design, Akzentfarbe, System-Theme übernehmen, kleine Fade-Animationen
- Sidebar links, Overlay-Verhalten (schwebt über Inhalt, verschiebt nicht), klappt bei Klick auf ca. 1/5 Fensterbreite aus, 300ms Animation, 1s Verzögerung beim Einklappen
- Sidebar zeigt Aufgaben-Kategorien (nicht mehr Klassenstufen – die wandern in die Einstellungen), Unterkategorien mehrfach auswählbar
- Korrekte mathematische Zeichen (× statt *, echte Potenz-/Wurzelschreibweise), verkürzte Schreibweisen erst ab Kl.10
- Antwortfelder: primär Zahlen, TAB/Enter-Navigation zwischen mehreren Feldern, Farb-Feedback rot/grün

## Wichtig für die Zusammenarbeit

- Ich will aktiv mitlernen, nicht nur fertigen Code bekommen – bitte kurze Erklärungen zu Syntax/Konzepten mitliefern
- Viele Code-Kommentare, besonders an ungewöhnlichen Stellen
- Bei Architektur-Fragen: die Sternsystem-Logik (siehe oben) ist bewusst so gewählt (Zuverlässigkeit, kein Verlorengehen einzelner Aufgaben-Arten) – bitte nicht ungefragt zu einem monolithischen Generator umbauen
