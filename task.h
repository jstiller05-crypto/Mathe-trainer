#ifndef TASK_H
#define TASK_H

#include <QString>
#include <QStringList>
#include <QVector>

struct AnswerSlot {
    QString label;
    double expectedValue;
};

// Beschreibt, WIE eine Aufgabe im Karo-Raster (WrittenGridWidget) dargestellt wird.
// Aktuell setzt JEDER Generator eine expression (TaskView zeigt deshalb ausnahmslos
// jede Aufgabe im Raster an - die fruehere einzeilige Label-Darstellung ist entfallen).
// expression.isEmpty() bleibt trotzdem als Absicherung bestehen, falls eine kuenftige
// Kategorie (z.B. Trigonometrie/Geometrie) mal eine reine Wortaufgabe ohne Raster-
// taugliches Format braucht - siehe den Fallback auf promptText im Aufgabenblatt-
// Zweig von WrittenGridWidget::paintEvent().
struct WrittenCalculation {
    QStringList operands;      // NUR fuer Stacked-Modus (schriftliches Rechnen mit EINEM Operator), z.B. {"47", "38"}
    QString operatorSymbol;     // "+", "-", "×", "÷" - nur fuer Stacked-Modus
    QString expression;          // fertiger Ausdruck INKLUSIVE abschliessendem "=" fuer SingleLine, z.B. "47 + 38 =" oder "4^2 + 8 ="
                                  // (auch fuer verkettete Aufgaben und Potenz/Wurzel/Log geeignet, da hier kein festes
                                  // Operanden-Schema existiert - siehe TaskFragment-Kommentar in task_fragment.h)
    int answerDigitCount = 0;    // Zeichenanzahl der erwarteten Antwort (Ziffern, ggf. ein '.' oder '-')
    bool freeformAnswer = false; // true = EIN zusammenhaengendes Eingabefeld statt einzelner Ziffern-Kaestchen
                                  // (noetig bei Kommazahlen oder negativen Ergebnissen - ein Kaestchen fasst nur 1 Zeichen)
    enum class DisplayMode { Stacked, SingleLine };
    DisplayMode mode = DisplayMode::Stacked;
};

struct Task {
    QString ruleName;
    QString promptText;
    QVector<AnswerSlot> answers;
    bool autoAdvance = true;
    WrittenCalculation writtenCalculation;   // wird von TaskView immer ueber das Raster angezeigt
};

#endif