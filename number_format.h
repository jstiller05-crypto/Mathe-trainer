#ifndef NUMBER_FORMAT_H
#define NUMBER_FORMAT_H

#include <QString>

// F03: rundet "halb von Null weg" (0.145 -> 0.15, -0.145 -> -0.15) und gleicht dabei
// den BINAEREN Rundungsfehler aus, den ein einfaches std::round(value*factor)/factor
// bei Zahlen wie 0.145 noch haette (siehe number_format.cpp fuer die Erklaerung, WARUM
// 0.145 als double ueberhaupt ein Problem ist). Oeffentlich (nicht static in der .cpp),
// weil Phase 2 (F01) denselben Rundungsbedarf fuer die Antwortpruefung braucht
// (session_controller.cpp) - eine zweite, abweichende Rundung dort waere ein
// Wartungsrisiko (zwei Stellen, die bei einer Korrektur beide angepasst werden
// muessten).
double roundHalfAwayFromZero(double value, int decimals);

// Reine Formatierungs-Mechanik (wie fragment_algebra.h) - KEIN Zufall, KEINE Level-
// Logik. Rundet auf maxDecimals Nachkommastellen (ueber roundHalfAwayFromZero(), s.o.)
// und formatiert ueber QLocale::system() (liefert auf einem deutschen System ein Komma
// statt Punkt) - ueberfluessige Nachkommanullen werden danach abgeschnitten (z.B.
// "42,00" -> "42", waehrend "2,35" so bleibt). Wird sowohl fuer Aufgabentexte (z.B.
// Dezimalzahlen-Generator) als auch fuer die "richtig waere ..."-Rueckmeldung
// (MainWindow) gebraucht - deshalb hier zentral definiert statt doppelt gehalten.
QString formatGermanDecimal(double value, int maxDecimals = 2);

// Q_ASSERT-Selbsttest fuer roundHalfAwayFromZero()/formatGermanDecimal() (F03/F04) -
// rein deterministisch, kein Zufall beteiligt. Von main.cpp UND benchmark_main.cpp
// einmal beim Start aufgerufen, dort jeweils eingerahmt von #ifndef QT_NO_DEBUG
// (analog zu runCombineSelfTest() in arithmetic_unit.h).
void runNumberFormatSelfTest();

#endif
