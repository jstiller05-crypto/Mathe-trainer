#include "number_format.h"
#include <QLocale>
#include <QDebug>
#include <cmath>

// F03: "halb von Null weg" ist die in der Schule uebliche Rundungsregel (0,5 rundet
// IMMER weiter weg von 0, nie zur geraden Zahl hin wie beim sog. "Banker's Rounding").
// std::round() macht genau das schon richtig - das eigentliche Problem liegt tiefer:
// viele Dezimalbrueche lassen sich als IEEE-754-double gar nicht EXAKT darstellen,
// genauso wie sich 1/3 nicht exakt als endliche Dezimalzahl schreiben laesst. 0.145
// ist im Rechner in Wahrheit z.B. 0.144999999999999990... - std::round(0.145 * 100)
// rundet dieses interne 14.4999999... dann (korrekt fuer DIESEN Wert, aber nicht fuer
// die mathematisch gemeinte 14.5) auf 14 statt 15 ab.
// Das winzige Epsilon (1e-9 * factor) schiebt den Wert ein Stueck in die Richtung, in
// die er "mathematisch" eigentlich zeigen sollte, BEVOR std::round() greift - gross
// genug, um solche Binaer-Rundungsfehler (typischerweise kleiner als 1e-13 relativ
// zum Wert) auszugleichen, aber viel zu klein, um ein echtes 0.1449999 (das WIRKLICH
// zu 0.14 runden soll) zu verfaelschen.
// std::copysign(x, value) liefert |x| mit dem VORZEICHEN von "value" - bei negativen
// Werten wirkt das Epsilon dadurch weiter von 0 WEG (wie bei der Rundungsregel
// gewollt), nicht versehentlich zurueck zur 0.
double roundHalfAwayFromZero(double value, int decimals)
{
    double factor = std::pow(10.0, decimals);
    double rounded = std::round(value * factor + std::copysign(1e-9 * factor, value)) / factor;

    // F04 (Randfall): ein Wert wie -0.001 wuerde hier auf eine NEGATIVE Null (-0.0)
    // runden - rechnerisch identisch zu 0.0 (IEEE 754 definiert -0.0 == 0.0 als wahr),
    // die TEXT-Darstellung wuerde daraus aber ein verwirrendes "-0" machen. Die
    // Zuweisung des Literals 0.0 (nicht etwa "-rounded" oder aehnliches) ersetzt das
    // Vorzeichen-Bit unabhaengig davon, welche der beiden Nullen "rounded" gerade ist.
    if (rounded == 0.0) rounded = 0.0;

    return rounded;
}

QString formatGermanDecimal(double value, int maxDecimals)
{
    QLocale locale = QLocale::system();

    // F04: QLocale::toString() wuerde sonst abhaengig von der Systemsprache ein
    // Tausendertrennzeichen einfuegen (deutsch: "1.116,00", englisch: "1,116.00") -
    // nach dem Abschneiden der Nachkommanullen unten bliebe davon "1.116" bzw.
    // "1,116" stehen. parseUserNumber() (session_controller.cpp) erwartet aber
    // einzelne Zahlen OHNE Tausendertrennzeichen und wuerde "1.116" faelschlich als
    // 1,116 lesen (Punkt wird dort zu Komma-Ersatz) - Anzeige (hier) und Eingabe-
    // Parser muessen also garantiert dasselbe Format sprechen. QLocale::
    // OmitGroupSeparator ist ein QLocale::NumberOption - ein Flag-Enum, das sich wie
    // andere Qt-Flags per | kombinieren liesse, hier reicht aber dieses eine Flag.
    locale.setNumberOptions(QLocale::OmitGroupSeparator);

    // Erst SELBST runden (nicht nur ueber die Nachkommastellen-Anzahl von toString()
    // steuern) - so ist "rounded" der Wert, der tatsaechlich angezeigt wird, und
    // stimmt exakt mit dem ueberein, was danach an Text entsteht. roundHalfAwayFromZero()
    // (s.o.) gleicht dabei zusaetzlich den binaeren Rundungsfehler aus (F03).
    double rounded = roundHalfAwayFromZero(value, maxDecimals);

    // toString(double, 'f', maxDecimals) liefert IMMER genau maxDecimals
    // Nachkommastellen (z.B. "42,00") - die fuer eine ganze Zahl ueberfluessigen
    // Nullen (und ein dann ueberfluessiges Komma) werden danach manuell abgeschnitten.
    QString formatted = locale.toString(rounded, 'f', maxDecimals);

    QString decimalPoint = locale.decimalPoint();
    if (formatted.contains(decimalPoint)) {
        while (formatted.endsWith('0')) formatted.chop(1);
        if (formatted.endsWith(decimalPoint)) formatted.chop(1);
    }

    return formatted;
}

// Q_ASSERT siehe Kommentar bei runCombineSelfTest() in arithmetic_unit.cpp - hier fuer
// roundHalfAwayFromZero()/formatGermanDecimal() mit den aus F03/F04 bekannten
// Problemfaellen. Das erwartete Dezimaltrennzeichen wird bewusst vom SYSTEM abgefragt
// (QLocale::system()) statt fest ein Komma anzunehmen, damit der Test auch auf einem
// englischsprachigen Windows (Punkt statt Komma) nicht faelschlich fehlschlaegt.
void runNumberFormatSelfTest()
{
    QString decimalPoint = QLocale::system().decimalPoint();

    Q_ASSERT(formatGermanDecimal(0.145) == "0" + decimalPoint + "15");
    Q_ASSERT(formatGermanDecimal(1.005) == "1" + decimalPoint + "01");
    Q_ASSERT(formatGermanDecimal(2.675) == "2" + decimalPoint + "68");
    Q_ASSERT(formatGermanDecimal(1116.0) == "1116");   // KEIN Tausendertrennzeichen (F04)
    Q_ASSERT(formatGermanDecimal(-0.001) == "0");       // NICHT "-0" (F04-Randfall)

    qDebug() << "[NumberFormat] Selbsttest erfolgreich (5/5 Faelle, Dezimaltrennzeichen:" << decimalPoint << ").";
}
