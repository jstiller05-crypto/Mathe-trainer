#ifndef ARITHMETIC_UNIT_H
#define ARITHMETIC_UNIT_H

#include "task.h"
#include "difficulty.h"
#include <QStringList>

// F27: Diagnose-Infos zu EINEM generateArithmeticTask()-Aufruf - zeigt, wie viele
// Versuche die interne Wiederholschleife gebraucht hat, aus welchen Gruenden
// verworfene Versuche abgelehnt wurden, und ob am Ende der Notausgang (Aufgabe nach
// maxAttempts trotz Bedenken genommen) gegriffen hat. Frueher war das alles nur per
// qDebug() in der Konsole sichtbar - generator-bench (siehe benchmark_main.cpp) kann
// es damit jetzt auch STATISTISCH auswerten (Spalten "Versuche Ø"/"Abgelehnt%"/
// "Notausgang"). Im normalen App-Betrieb interessiert das niemanden, deshalb unten ein
// OPTIONALER Ausgabeparameter (Zeiger mit nullptr als Default) statt eines
// Pflicht-Rueckgabewerts, den jeder Aufrufer mitschleppen muesste.
struct ArithmeticGenerationInfo {
    int attempts = 0;             // wie viele Erzeugungsversuche insgesamt gebraucht wurden (1 = sofort plausibel)
    bool fallbackUsed = false;    // true = Notausgang gegriffen (siehe generateArithmeticTask())
    QStringList rejectReasons;    // ein Eintrag pro verworfenem Versuch, von findImplausibilityReason() geliefert
};

Task generateArithmeticTask(DifficultyLevel level, const QStringList &activeSubcategories, TaskMode mode,
                             ArithmeticGenerationInfo *info = nullptr);
QStringList arithmeticAvailableSubcategories(DifficultyLevel level);

// Selbsttest fuer combineWithOperator()/wrapIfNeeded() (fragment_algebra.h) - rein
// deterministisch, kein Zufall beteiligt. Von main.cpp einmal beim Start aufgerufen,
// dort eingerahmt von #ifndef QT_NO_DEBUG (siehe dortiger Kommentar).
void runCombineSelfTest();

#endif