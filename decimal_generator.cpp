#include "decimal_generator.h"
#include "number_format.h"
#include "random_utils.h"
#include <cmath>
#include <algorithm>
#include <QDebug>

// Eigene, bewusst einfache Umrechnung (wie jeder Generator).
static int maxOperandForLevel(DifficultyLevel level, bool mentalMath)
{
    int base = 3 + level / 15;
    return mentalMath ? base : base * 2;
}

// Erzeugt eine Dezimalzahl als Ganzzahl in der jeweils kleinsten Stelle (Standard:
// Hundertstel) und teilt ERST DANACH durch die passende Zehnerpotenz, statt z.B.
// "3 + rand()%10/10.0 + rand()%10/100.0" (Vorkommateil + zwei EINZELN gewuerfelte
// Nachkommastellen) zu rechnen. Der Unterschied: bei der zweiten Methode werden
// mehrere SEPARATE Gleitkomma-Divisionen addiert, deren Rundungsfehler sich
// gegenseitig verstaerken koennen (0.1 laesst sich z.B. gar nicht exakt als
// Binaerzahl speichern) - das Ergebnis waere dann z.B. 3.4500000000000002 statt
// exakt 3.45. Eine EINZIGE Ganzzahl-Division hat dagegen nur eine einzige
// Rundungsstelle und liefert zuverlaessig den erwarteten Wert.
static double randomDecimalValue(int maxWhole, int decimals = 2)
{
    int scale = 1;
    for (int i = 0; i < decimals; ++i) scale *= 10;

    // F17: vorher "rand() % (maxWhole * scale) + 1" - unter Windows ist RAND_MAX nur
    // 32767, bei decimals=4 (scale=10000, siehe "Runde ..."-Aufgaben) war der Divisor
    // also schon ab maxWhole >= 4 GROESSER als RAND_MAX, wodurch rand()%N nie Werte
    // ueber RAND_MAX lieferte - der Wertebereich blieb praktisch immer bei maximal
    // 3,2767. randomInt() (random_utils.h, auf QRandomGenerator basierend) ist nicht an
    // RAND_MAX gebunden und deckt den vollen Bereich ab.
    int wholeScaled = randomInt(1, maxWhole * scale);
    return wholeScaled / static_cast<double>(scale);
}

static Task buildTaskFromValue(const QString &promptText, double value, bool mentalMath)
{
    Task task;
    task.ruleName = "Decimal";
    task.promptText = promptText;
    task.answers.append({ "", value });
    task.autoAdvance = mentalMath;

    QString answerStr = QString::number(value);
    task.writtenCalculation.expression = task.promptText;
    task.writtenCalculation.answerDigitCount = answerStr.length();
    task.writtenCalculation.freeformAnswer = answerStr.contains('.') || answerStr.contains('-');
    task.writtenCalculation.mode = WrittenCalculation::DisplayMode::SingleLine;

    qDebug() << "[Decimal] mentalMath:" << mentalMath << "|" << task.promptText;
    return task;
}

Task generateDecimalTask(DifficultyLevel level, bool mentalMath)
{
    int maxOperand = maxOperandForLevel(level, mentalMath);
    int form = randomInt(0, 3);

    QString promptText;
    double value;

    switch (form) {
    case 0: {
        // Addition: "3,45 + 2,7 ="
        double a = randomDecimalValue(maxOperand);
        double b = randomDecimalValue(maxOperand);
        value = a + b;
        promptText = QString("%1 + %2 =").arg(formatGermanDecimal(a)).arg(formatGermanDecimal(b));
        break;
    }
    case 1: {
        // Subtraktion: "6,8 - 2,35 =" - groesserer Wert zuerst, damit das Ergebnis
        // nicht negativ wird (negative Dezimalzahlen sind Sache von "Negative Zahlen").
        double a = randomDecimalValue(maxOperand);
        double b = randomDecimalValue(maxOperand);
        if (b > a) std::swap(a, b);
        value = a - b;
        promptText = QString("%1 - %2 =").arg(formatGermanDecimal(a)).arg(formatGermanDecimal(b));
        break;
    }
    case 2: {
        // Multiplikation: Dezimalzahl × ganze Zahl, damit das Ergebnis kontrollierbar
        // bleibt: "2,5 × 4 ="
        double a = randomDecimalValue(maxOperand);
        int b = randomInt(1, maxOperand);
        value = a * b;
        promptText = QString("%1 × %2 =").arg(formatGermanDecimal(a)).arg(b);
        break;
    }
    default: {
        // Runden: "Runde 3,4567 auf 2 Nachkommastellen =" - der Ausgangswert braucht
        // MEHR als 2 Nachkommastellen, sonst gibt es nichts zu runden - deshalb hier
        // 4 Nachkommastellen statt der sonst ueblichen 2.
        double source = randomDecimalValue(maxOperand, 4);
        value = std::round(source * 100.0) / 100.0;
        promptText = QString("Runde %1 auf 2 Nachkommastellen =").arg(formatGermanDecimal(source, 4));
        break;
    }
    }

    return buildTaskFromValue(promptText, value, mentalMath);
}

TaskFragment generateDecimalFragment(DifficultyLevel level, bool mentalMath)
{
    int maxOperand = maxOperandForLevel(level, mentalMath);
    double value = randomDecimalValue(maxOperand);

    TaskFragment fragment;
    fragment.value = value;
    fragment.display = formatGermanDecimal(value);
    fragment.precedence = Precedence::Atom;
    return fragment;
}
