#include "divisibility_generator.h"
#include <cstdlib>
#include <algorithm>
#include <QDebug>

// Feste Obergrenze fuer die Operanden, UNABHAENGIG von der normalen Level-Skalierung
// weiter unten: zwei grosse, zueinander teilerfremde Zahlen (z.B. 59 und 58) haetten
// sonst ein kgV weit im vierstelligen Bereich zur Folge (59 * 58 = 3422), waehrend
// ggT/kgV eigentlich ein Kl.5-Thema mit ueberschaubaren Zahlen sein soll.
static constexpr int kMaxOperand = 60;

// Eigene, bewusst einfache Umrechnung (wie jeder Generator) - gedeckelt durch
// kMaxOperand (s.o.).
static int maxOperandForLevel(DifficultyLevel level, bool mentalMath)
{
    int base = 6 + level / 4;
    int scaled = mentalMath ? base : base * 2;
    return std::min(scaled, kMaxOperand);
}

// Euklidischer Algorithmus, SELBST geschrieben (nicht std::gcd aus <numeric>) - als
// Lernbeispiel fuer eine while-Schleife.
//
// Die Idee: der groesste gemeinsame Teiler (ggT) von a und b aendert sich NICHT,
// wenn man die groessere der beiden Zahlen durch den REST ersetzt, der bei der
// Division durch die kleinere entsteht (a % b) - dieser Rest hat naemlich exakt
// dieselben gemeinsamen Teiler wie a und b zusammen. Wiederholt man das immer
// wieder, wird b bei jedem Schritt kleiner, bis irgendwann der Rest 0 ist - was
// dann in a uebrig geblieben ist, ist der ggT.
//
// Beispiel ggT(48, 18):
//   48 % 18 = 12   ->  naechster Schritt: a=18, b=12
//   18 % 12 = 6    ->  naechster Schritt: a=12, b=6
//   12 % 6  = 0    ->  b ist jetzt 0 -> FERTIG, ggT = a = 6
static int computeGcd(int a, int b)
{
    while (b != 0) {
        int remainder = a % b;
        a = b;
        b = remainder;
    }
    return a;   // sobald b == 0 ist, steht der ggT in a
}

// Das kleinste gemeinsame Vielfache (kgV) haengt eng mit dem ggT zusammen: im
// Produkt a*b stecken die gemeinsamen Teiler von a und b GENAU ZWEIMAL (einmal aus
// a, einmal aus b) - teilt man durch den ggT (der genau diese gemeinsamen Teiler
// enthaelt), bleiben sie nur noch EINMAL uebrig. Beispiel: ggT(4,6)=2, also
// kgV(4,6) = 4*6/2 = 12 - und 12 ist tatsaechlich die kleinste Zahl, die sowohl
// durch 4 als auch durch 6 teilbar ist.
static int computeLcm(int a, int b)
{
    return (a * b) / computeGcd(a, b);
}

static Task buildTaskFromResult(const QString &display, int value, bool mentalMath)
{
    Task task;
    task.ruleName = "Divisibility";
    task.promptText = display + " =";
    task.answers.append({ "", static_cast<double>(value) });
    task.autoAdvance = mentalMath;

    QString answerStr = QString::number(value);
    task.writtenCalculation.expression = task.promptText;
    task.writtenCalculation.answerDigitCount = answerStr.length();
    task.writtenCalculation.freeformAnswer = false;   // ggT/kgV zweier positiver Zahlen ist immer eine positive Ganzzahl
    task.writtenCalculation.mode = WrittenCalculation::DisplayMode::SingleLine;

    qDebug() << "[Divisibility] mentalMath:" << mentalMath << "|" << task.promptText;
    return task;
}

Task generateDivisibilityTask(DifficultyLevel level, bool mentalMath)
{
    int maxOperand = maxOperandForLevel(level, mentalMath);
    // Mindestens 2 statt 1 - ggT/kgV mit 1 waere immer trivial (ggT(1,x)=1) und
    // wuerde die eigentliche Uebung (Primfaktoren/Teiler suchen) nicht ansprechen.
    int a = rand() % maxOperand + 2;
    int b = rand() % maxOperand + 2;

    bool useLcm = (rand() % 2 == 0);
    QString display = useLcm ? QString("kgV(%1, %2)").arg(a).arg(b)
                               : QString("ggT(%1, %2)").arg(a).arg(b);
    int value = useLcm ? computeLcm(a, b) : computeGcd(a, b);

    return buildTaskFromResult(display, value, mentalMath);
}

TaskFragment generateDivisibilityFragment(DifficultyLevel level, bool mentalMath)
{
    int maxOperand = maxOperandForLevel(level, mentalMath);
    int a = rand() % maxOperand + 2;
    int b = rand() % maxOperand + 2;

    bool useLcm = (rand() % 2 == 0);

    TaskFragment fragment;
    fragment.value = useLcm ? computeLcm(a, b) : computeGcd(a, b);
    fragment.display = useLcm ? QString("kgV(%1, %2)").arg(a).arg(b)
                                : QString("ggT(%1, %2)").arg(a).arg(b);
    fragment.precedence = Precedence::Atom;   // fertiger Wert, wie jede andere Zahl auch
    return fragment;
}

// Q_ASSERT siehe Kommentar bei runCombineSelfTest() in arithmetic_unit.cpp - hier
// fuer computeGcd()/computeLcm() mit fest bekannten Werten.
void runDivisibilitySelfTest()
{
    Q_ASSERT(computeGcd(12, 18) == 6);
    Q_ASSERT(computeLcm(4, 6) == 12);
    Q_ASSERT(computeGcd(17, 5) == 1);   // teilerfremd - der ggT zweier teilerfremder Zahlen ist immer 1

    qDebug() << "[Divisibility] Selbsttest fuer computeGcd()/computeLcm() erfolgreich (3/3 Faelle).";
}
