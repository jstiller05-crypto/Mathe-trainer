#ifndef NUMBER_FORMAT_H
#define NUMBER_FORMAT_H

#include <QString>

// Reine Formatierungs-Mechanik (wie fragment_algebra.h) - KEIN Zufall, KEINE Level-
// Logik. Rundet auf maxDecimals Nachkommastellen und formatiert ueber
// QLocale::system() (liefert auf einem deutschen System ein Komma statt Punkt) -
// ueberfluessige Nachkommanullen werden danach abgeschnitten (z.B. "42,00" -> "42",
// waehrend "2,35" so bleibt). Wird sowohl fuer Aufgabentexte (z.B. Dezimalzahlen-
// Generator) als auch fuer die "richtig waere ..."-Rueckmeldung (MainWindow)
// gebraucht - deshalb hier zentral definiert statt doppelt gehalten.
QString formatGermanDecimal(double value, int maxDecimals = 2);

#endif
