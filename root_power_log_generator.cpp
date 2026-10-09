#include "root_power_log_generator.h"
#include "random_utils.h"
#include <cmath>
#include <QDebug>

// Nur fuer die ANZEIGE/Feldbreite: auf 2 Nachkommastellen gerundete Zeichenkette.
// Der echte fragment.value/expectedValue bleibt IMMER der volle double-Wert (Punkt 3) -
// wuerde man schon beim Runden fuer die Anzeige den Wert selbst veraendern, koennten
// sich bei Ketten aus mehreren gerundeten Gliedern Abweichungen aufsummieren, die
// groesser als die 0.001-Toleranz in checkAnswers() werden (z.B. √83 + √47).
static QString roundedDisplayString(double value)
{
    return QString::number(std::round(value * 100.0) / 100.0);
}

static TaskFragment buildPowerFragment(DifficultyLevel level, bool mentalMath)
{
    bool fullRangeUnlocked = level >= RootPowerLogCriteria::PowerFullMinLevel;

    int base = mentalMath ? randomInt(2, 9) : randomInt(2, 13);
    int exponent = fullRangeUnlocked ? (mentalMath ? randomInt(2, 3) : randomInt(2, 4)) : 2;

    TaskFragment fragment;
    fragment.value = std::pow(base, exponent);
    fragment.display = QString("%1^%2").arg(base).arg(exponent);
    fragment.precedence = Precedence::Atom;   // "4^2" ist ein einzelner Wert, keine Verkettung
    return fragment;
}

Task generatePowerTask(DifficultyLevel level, bool mentalMath)
{
    TaskFragment fragment = buildPowerFragment(level, mentalMath);

    Task task;
    task.ruleName = "Power";
    task.promptText = fragment.display + " =";
    task.answers.append({ "", fragment.value });
    task.autoAdvance = mentalMath;

    // Potenz hat kein Operanden-Schema fuers Stacked-Raster - immer SingleLine.
    QString answerStr = QString::number(fragment.value);
    task.writtenCalculation.expression = task.promptText;
    task.writtenCalculation.answerDigitCount = answerStr.length();
    task.writtenCalculation.freeformAnswer = answerStr.contains('.');
    task.writtenCalculation.mode = WrittenCalculation::DisplayMode::SingleLine;

    qDebug() << "[Power] Level:" << level << "| mentalMath:" << mentalMath << "|" << task.promptText;
    return task;
}

TaskFragment generatePowerFragment(DifficultyLevel level, bool mentalMath)
{
    return buildPowerFragment(level, mentalMath);
}

static TaskFragment buildRootFragment(DifficultyLevel level, bool mentalMath)
{
    Q_UNUSED(level);
    TaskFragment fragment;

    if (mentalMath) {
        int root = randomInt(2, 11);
        int operand = root * root;
        fragment.value = root;
        fragment.display = QString("√%1").arg(operand);
    } else {
        int operand = randomInt(10, 99);
        fragment.value = std::sqrt(operand);   // voller double-Wert, NICHT gerundet (Punkt 3)
        fragment.display = QString("√%1").arg(operand);
    }

    fragment.precedence = Precedence::Atom;   // "√81" ist ein einzelner Wert, keine Verkettung
    return fragment;
}

Task generateRootTask(DifficultyLevel level, bool mentalMath)
{
    TaskFragment fragment = buildRootFragment(level, mentalMath);

    Task task;
    task.ruleName = "Root";
    task.promptText = fragment.display + " =";
    task.answers.append({ "", fragment.value });
    task.autoAdvance = mentalMath;

    // Wurzel hat kein Operanden-Schema fuers Stacked-Raster - immer SingleLine. Im
    // Nicht-Kopfrechnen-Fall ist das Ergebnis meist irrational (z.B. 6.8556546...) -
    // fuer die Feldbreite zaehlt trotzdem nur die auf 2 Nachkommastellen gerundete
    // ANZEIGE (roundedDisplayString), expectedValue oben bleibt der exakte Wert.
    QString displayStr = roundedDisplayString(fragment.value);
    task.writtenCalculation.expression = task.promptText;
    task.writtenCalculation.answerDigitCount = displayStr.length();
    task.writtenCalculation.freeformAnswer = displayStr.contains('.');
    task.writtenCalculation.mode = WrittenCalculation::DisplayMode::SingleLine;

    qDebug() << "[Root] Level:" << level << "| mentalMath:" << mentalMath << "|" << task.promptText;
    return task;
}

TaskFragment generateRootFragment(DifficultyLevel level, bool mentalMath)
{
    return buildRootFragment(level, mentalMath);
}

static TaskFragment buildLogFragment(DifficultyLevel level, bool mentalMath)
{
    Q_UNUSED(level);
    TaskFragment fragment;

    if (mentalMath) {
        int exponent = randomInt(1, 4);
        int operand = static_cast<int>(std::pow(10, exponent));
        fragment.value = exponent;
        fragment.display = QString("log(%1)").arg(operand);
    } else {
        int operand = randomInt(100, 9999);
        fragment.value = std::log10(operand);   // voller double-Wert, NICHT gerundet (Punkt 3)
        fragment.display = QString("log(%1)").arg(operand);
    }

    fragment.precedence = Precedence::Atom;   // "log(100)" ist ein einzelner Wert, keine Verkettung
    return fragment;
}

Task generateLogTask(DifficultyLevel level, bool mentalMath)
{
    TaskFragment fragment = buildLogFragment(level, mentalMath);

    Task task;
    task.ruleName = "Log";
    task.promptText = fragment.display + " =";
    task.answers.append({ "", fragment.value });
    task.autoAdvance = mentalMath;

    // Logarithmus hat kein Operanden-Schema fuers Stacked-Raster - immer SingleLine.
    // Fuer die Feldbreite zaehlt nur die auf 2 Nachkommastellen gerundete ANZEIGE,
    // expectedValue oben bleibt der exakte Wert (siehe roundedDisplayString-Kommentar).
    QString displayStr = roundedDisplayString(fragment.value);
    task.writtenCalculation.expression = task.promptText;
    task.writtenCalculation.answerDigitCount = displayStr.length();
    task.writtenCalculation.freeformAnswer = displayStr.contains('.');
    task.writtenCalculation.mode = WrittenCalculation::DisplayMode::SingleLine;

    qDebug() << "[Log] Level:" << level << "| mentalMath:" << mentalMath << "|" << task.promptText;
    return task;
}

TaskFragment generateLogFragment(DifficultyLevel level, bool mentalMath)
{
    return buildLogFragment(level, mentalMath);
}