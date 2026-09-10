// Generator-Pruefstand: ein zweites, GUI-loses Konsolen-Werkzeug (siehe CMakeLists.txt,
// Ziel "generator-bench"). Ruft die bestehenden Aufgaben-Generatoren viele Male auf
// und wertet ihre Ausgabe statistisch aus - fuer schnelle Plausibilitaets-Checks beim
// Entwickeln, ohne jedes Mal durch die GUI klicken zu muessen.
//
// KEINE Widgets noetig, deshalb QCoreApplication statt QApplication - QCoreApplication
// bringt nur den Event-Loop/Kommandozeilen-Kram mit, ohne die (hier ungenutzte)
// GUI-Infrastruktur von QApplication.
#include "arithmetic_unit.h"
#include "divisibility_generator.h"
#include "number_format.h"
#include "difficulty.h"

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QTextStream>
#include <QStringConverter>
#include <QFile>
#include <QHash>
#include <QVector>
#include <QDebug>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>

#ifdef Q_OS_WIN
// Fuer SetConsoleOutputCP()/SetConsoleCP() - ohne das interpretiert die Windows-Konsole
// UTF-8-Bytes standardmaessig mit der falschen Codepage, wodurch Umlaute als Mojibake
// erscheinen (z.B. "GrÃ¶ÃŸen" statt "Größen").
#include <Windows.h>
#endif

// Sammelt die Kennzahlen fuer EINEN Lauf (eine Unterkategorie ODER "Gemischt").
struct Stats {
    int count = 0;
    double sumValue = 0.0;
    double minValue = 0.0;
    double maxValue = 0.0;
    int negativeCount = 0;
    int decimalCount = 0;
    int bracketCount = 0;
    int chainHeuristicCount = 0;
    qint64 totalExpressionLength = 0;
    QHash<QString, int> promptCounts;   // fuer Duplikat-Erkennung ueber task.promptText
};

static void addSample(Stats &stats, const Task &task)
{
    double value = task.answers.isEmpty() ? 0.0 : task.answers.first().expectedValue;

    if (stats.count == 0) {
        stats.minValue = value;
        stats.maxValue = value;
    } else {
        stats.minValue = std::min(stats.minValue, value);
        stats.maxValue = std::max(stats.maxValue, value);
    }
    stats.sumValue += value;
    stats.count++;

    if (value < 0) stats.negativeCount++;
    // Kleine Toleranz statt exaktem Vergleich - wie ueberall sonst im Projekt
    // (checkAnswers(), isPlausible()), wegen Rundungsfehlern bei Gleitkommazahlen.
    if (std::abs(value - std::round(value)) > 0.001) stats.decimalCount++;

    const QString &expression = task.writtenCalculation.expression;
    if (expression.contains('(')) stats.bracketCount++;

    // HEURISTIK, KEIN echter Parser: zaehlt einfach die Zeichen aus {+,-,×,÷} in der
    // expression, 2 oder mehr gelten als "verkettet". Das Minuszeichen einer bereits
    // geklammerten negativen Zahl (z.B. "(-4)") zaehlt hier mit - eine Aufgabe wie
    // "(-3) + 5" hat dadurch schon 2 Treffer ("-" und "+") und gilt als verkettet,
    // obwohl es nur EIN Operator ist. Die Kategorie "Negative Zahlen" wird durch
    // diese Heuristik also leicht ueberschaetzt. Fuer ein Debug-Werkzeug tolerierbar -
    // kein Grund, dafuer einen echten Ausdrucks-Parser zu bauen.
    int operatorCharCount = 0;
    for (QChar ch : expression) {
        if (ch == '+' || ch == '-' || ch == QChar(0x00D7) || ch == QChar(0x00F7)) {
            operatorCharCount++;
        }
    }
    if (operatorCharCount >= 2) stats.chainHeuristicCount++;

    stats.totalExpressionLength += expression.length();
    stats.promptCounts[task.promptText]++;
}

static QString taskModeLabel(TaskMode mode)
{
    switch (mode) {
    case TaskMode::MentalMath: return "Kopfrechnen";
    case TaskMode::Calculator: return "Taschenrechner";
    case TaskMode::Hard:       return "Schwere Aufgabe";
    }
    return "?";
}

static Stats runSubcategory(DifficultyLevel level, const QString &subcategory, TaskMode mode, int samples)
{
    Stats stats;
    // Eine Liste mit nur EINEM Eintrag erzwingt automatisch chainLength <= 1 in
    // arithmetic_unit.cpp: pickChainLength() deckelt die Kettenlaenge auf die Anzahl
    // der fragment-faehigen Unterkategorien in der Auswahl - bei nur einer aktiven
    // Unterkategorie ist "verschmelzen" gar nicht moeglich (dafuer braeuchte es
    // mindestens 2 verschiedene, um daraus eine Kette zu bauen). Dieser Aufruf
    // liefert also garantiert Aufgaben NUR dieser einen Art.
    QStringList onlyThis = { subcategory };

    for (int i = 0; i < samples; ++i) {
        Task task = generateArithmeticTask(level, onlyThis, mode);
        addSample(stats, task);
    }
    return stats;
}

static Stats runMixed(DifficultyLevel level, const QStringList &allSubcategories, TaskMode mode, int samples)
{
    Stats stats;
    for (int i = 0; i < samples; ++i) {
        // Volle Liste statt einer einzelnen Unterkategorie - hier DARF (und soll)
        // arithmetic_unit.cpp verketten. Zeigt das tatsaechliche Bild, das ein
        // Spieler ohne Sidebar-Einschraenkung zu sehen bekommt.
        Task task = generateArithmeticTask(level, allSubcategories, mode);
        addSample(stats, task);
    }
    return stats;
}

// QTextStream::setFieldWidth() setzt die Mindestbreite fuer JEDE nachfolgende
// Ausgabe, bis es erneut geaendert wird - anders als qDebug(), das nur einzelne
// durch Leerzeichen getrennte Werte ohne jede Ausrichtung aneinanderreiht. Deshalb
// wird hier vor JEDER Spalte die Breite neu gesetzt (und am Ende auf 0 zurueckgesetzt,
// damit nachfolgender "normaler" Text nicht ungewollt mit aufgefuellt wird).
static void printTableHeader(QTextStream &out)
{
    out.setFieldAlignment(QTextStream::AlignLeft);
    out.setFieldWidth(24); out << "Unterkategorie";
    out.setFieldAlignment(QTextStream::AlignRight);
    out.setFieldWidth(8);  out << "Anzahl";
    out.setFieldWidth(10); out << "Min";
    out.setFieldWidth(10); out << "Max";
    out.setFieldWidth(9);  out << "Ø";
    out.setFieldWidth(8);  out << "Neg%";
    out.setFieldWidth(8);  out << "Dez%";
    out.setFieldWidth(10); out << "Klammer%";
    out.setFieldWidth(12); out << "Verkettet%";
    out.setFieldWidth(12); out << "Duplikate%";
    out.setFieldWidth(10); out << "Ø Laenge";
    out.setFieldWidth(0);
    out << Qt::endl;
}

static void printTableRow(QTextStream &out, const QString &label, const Stats &stats)
{
    double mean = stats.count > 0 ? stats.sumValue / stats.count : 0.0;
    double negPercent = stats.count > 0 ? 100.0 * stats.negativeCount / stats.count : 0.0;
    double decPercent = stats.count > 0 ? 100.0 * stats.decimalCount / stats.count : 0.0;
    double bracketPercent = stats.count > 0 ? 100.0 * stats.bracketCount / stats.count : 0.0;
    double chainPercent = stats.count > 0 ? 100.0 * stats.chainHeuristicCount / stats.count : 0.0;
    double duplicatePercent = stats.count > 0 ? 100.0 * (stats.count - stats.promptCounts.size()) / stats.count : 0.0;
    double avgLength = stats.count > 0 ? static_cast<double>(stats.totalExpressionLength) / stats.count : 0.0;

    out.setFieldAlignment(QTextStream::AlignLeft);
    out.setFieldWidth(24); out << label;
    out.setFieldAlignment(QTextStream::AlignRight);
    out.setFieldWidth(8);  out << stats.count;
    out.setFieldWidth(10); out << QString::number(stats.minValue, 'f', 2);
    out.setFieldWidth(10); out << QString::number(stats.maxValue, 'f', 2);
    out.setFieldWidth(9);  out << QString::number(mean, 'f', 2);
    out.setFieldWidth(8);  out << QString::number(negPercent, 'f', 1);
    out.setFieldWidth(8);  out << QString::number(decPercent, 'f', 1);
    out.setFieldWidth(10); out << QString::number(bracketPercent, 'f', 1);
    out.setFieldWidth(12); out << QString::number(chainPercent, 'f', 1);
    out.setFieldWidth(12); out << QString::number(duplicatePercent, 'f', 1);
    out.setFieldWidth(10); out << QString::number(avgLength, 'f', 1);
    out.setFieldWidth(0);
    out << Qt::endl;
}

static void writeCsvHeader(QTextStream &out)
{
    out << "Level;Klasse;Modus;Unterkategorie;Anzahl;Min;Max;Mittelwert;NegProzent;"
           "DezProzent;KlammerProzent;VerkettetProzent;DuplikateProzent;MittlereLaenge\n";
}

// Semikolon als Trennzeichen, Komma als Dezimaltrennzeichen (deutsches Excel-Format) -
// fuer die Zahlenspalten wird formatGermanDecimal() aus number_format.h wiederverwendet
// statt eine zweite Formatierung zu bauen (dieselbe Logik steckt schon in
// MainWindow::formatSolution() und im Dezimalzahlen-Generator).
static void writeCsvRow(QTextStream &out, DifficultyLevel level, int schoolClass, TaskMode mode,
                         const QString &label, const Stats &stats)
{
    double mean = stats.count > 0 ? stats.sumValue / stats.count : 0.0;
    double negPercent = stats.count > 0 ? 100.0 * stats.negativeCount / stats.count : 0.0;
    double decPercent = stats.count > 0 ? 100.0 * stats.decimalCount / stats.count : 0.0;
    double bracketPercent = stats.count > 0 ? 100.0 * stats.bracketCount / stats.count : 0.0;
    double chainPercent = stats.count > 0 ? 100.0 * stats.chainHeuristicCount / stats.count : 0.0;
    double duplicatePercent = stats.count > 0 ? 100.0 * (stats.count - stats.promptCounts.size()) / stats.count : 0.0;
    double avgLength = stats.count > 0 ? static_cast<double>(stats.totalExpressionLength) / stats.count : 0.0;

    out << level << ";" << schoolClass << ";" << taskModeLabel(mode) << ";" << label << ";"
        << stats.count << ";"
        << formatGermanDecimal(stats.minValue) << ";"
        << formatGermanDecimal(stats.maxValue) << ";"
        << formatGermanDecimal(mean) << ";"
        << formatGermanDecimal(negPercent) << ";"
        << formatGermanDecimal(decPercent) << ";"
        << formatGermanDecimal(bracketPercent) << ";"
        << formatGermanDecimal(chainPercent) << ";"
        << formatGermanDecimal(duplicatePercent) << ";"
        << formatGermanDecimal(avgLength) << "\n";
}

// Eigener Nachrichten-Handler: verwirft ALLE qDebug()-Ausgaben. Die Generatoren
// (allen voran arithmetic_unit.cpp) protokollieren normalerweise jeden einzelnen
// Verkettungsschritt per qDebug() - bei den hier ueblichen 500 Samples pro
// Unterkategorie/Modus/Level waeren das mehrere MILLIONEN Debug-Zeilen, die die
// eigentliche Ergebnistabelle voellig unlesbar machen wuerden. qWarning()/
// qCritical()/qFatal() (z.B. die wichtige Meldung "Keine plausible Aufgabe nach 20
// Versuchen" aus arithmetic_unit.cpp) sollen dagegen weiterhin auf stderr sichtbar
// bleiben, weil sie auf ein echtes Problem in einem Generator hinweisen koennten.
// WICHTIG: dieser Handler greift nur bei qDebug()/qWarning()/... - die eigentliche
// Ergebnistabelle laeuft ueber ein eigenes QTextStream(stdout) weiter unten und ist
// davon komplett unberuehrt.
static void quietDebugMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context);

    if (type == QtDebugMsg) return;   // verwerfen, s.o.

    switch (type) {
    case QtWarningMsg:  QTextStream(stderr) << "Warning: " << msg << Qt::endl; break;
    case QtCriticalMsg: QTextStream(stderr) << "Critical: " << msg << Qt::endl; break;
    case QtFatalMsg:
        QTextStream(stderr) << "Fatal: " << msg << Qt::endl;
        abort();
    default:
        break;
    }
}

int main(int argc, char *argv[])
{
    // Muss VOR dem ersten qDebug()/qWarning()-Aufruf installiert sein - deshalb
    // ganz am Anfang, noch vor srand() und vor QCoreApplication.
    qInstallMessageHandler(quietDebugMessageHandler);

#ifdef Q_OS_WIN
    // Windows-Konsole nutzt standardmaessig nicht UTF-8 als Codepage - ohne diese beiden
    // Aufrufe wuerden die (unten per setEncoding() als UTF-8 geschriebenen) Umlaute in
    // der Tabellenausgabe als Mojibake erscheinen.
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    // Gleiches Vorgehen wie in main.cpp der App: ohne srand() waere die
    // Zufallsfolge bei jedem Start identisch.
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Der Pruefstand soll NIE auf einer kaputten combine()/ggT-Implementierung
    // laufen, ohne dass es sofort auffaellt - deshalb dieselben Selbsttests wie in
    // main.cpp der App, ebenfalls nur im Debug-Build (siehe dortiger Kommentar).
#ifndef QT_NO_DEBUG
    runCombineSelfTest();
    runDivisibilitySelfTest();
#endif

    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("generator-bench");

    QCommandLineParser parser;
    parser.setApplicationDescription(
        "Generator-Pruefstand: ruft die Aufgaben-Generatoren viele Male auf und wertet ihre Ausgabe statistisch aus.");
    parser.addHelpOption();

    QCommandLineOption samplesOption("samples", "Aufgaben pro Level/Modus/Unterkategorie.", "N", "500");
    QCommandLineOption csvOption("csv", "Ergebnisse zusaetzlich als CSV-Datei schreiben.", "pfad");
    parser.addOption(samplesOption);
    parser.addOption(csvOption);
    parser.process(app);

    int samples = parser.value(samplesOption).toInt();
    if (samples <= 0) samples = 500;

    QTextStream out(stdout);
    out.setEncoding(QStringConverter::Utf8);   // explizit statt Qt's Locale-Ratewerk auf der Konsole

    bool writeCsv = parser.isSet(csvOption);
    QFile csvFile;
    QTextStream csvOut;
    if (writeCsv) {
        csvFile.setFileName(parser.value(csvOption));
        if (csvFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            csvOut.setDevice(&csvFile);
            csvOut.setEncoding(QStringConverter::Utf8);
            csvOut.setGenerateByteOrderMark(true);   // Windows-Excel erkennt UTF-8 nur zuverlaessig mit BOM
            writeCsvHeader(csvOut);
        } else {
            qWarning() << "Konnte CSV-Datei nicht oeffnen:" << csvFile.fileName();
            writeCsv = false;
        }
    }

    // Die acht ueber die Klassen-Buttons in den Einstellungen tatsaechlich
    // erreichbaren Level (Kl.3-10), nicht die volle 1-100-Skala.
    const QVector<TaskMode> modes = { TaskMode::MentalMath, TaskMode::Calculator, TaskMode::Hard };

    for (int schoolClass = 3; schoolClass <= 10; ++schoolClass) {
        DifficultyLevel level = classToLevel(schoolClass);

        for (TaskMode mode : modes) {
            out << "=== Klasse " << schoolClass << " (Level " << level << ") - "
                << taskModeLabel(mode) << " ===" << Qt::endl;
            printTableHeader(out);

            QStringList subcategories = arithmeticAvailableSubcategories(level);

            for (const QString &subcategory : subcategories) {
                Stats stats = runSubcategory(level, subcategory, mode, samples);
                printTableRow(out, subcategory, stats);
                if (writeCsv) writeCsvRow(csvOut, level, schoolClass, mode, subcategory, stats);
            }

            Stats mixedStats = runMixed(level, subcategories, mode, samples);
            printTableRow(out, "Gemischt", mixedStats);
            if (writeCsv) writeCsvRow(csvOut, level, schoolClass, mode, "Gemischt", mixedStats);

            out << Qt::endl;
        }
    }

    if (writeCsv) {
        csvFile.close();
        out << "CSV geschrieben nach: " << csvFile.fileName() << Qt::endl;
    }

    return 0;
}
