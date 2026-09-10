#include "number_format.h"
#include <QLocale>
#include <cmath>

QString formatGermanDecimal(double value, int maxDecimals)
{
    QLocale locale = QLocale::system();

    // Erst SELBST runden (nicht nur ueber die Nachkommastellen-Anzahl von
    // toString() steuern) - so ist "rounded" der Wert, der tatsaechlich angezeigt
    // wird, und stimmt exakt mit dem ueberein, was danach an Text entsteht.
    double factor = std::pow(10.0, maxDecimals);
    double rounded = std::round(value * factor) / factor;

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
