#include "mainwindow.h"
#include "arithmetic_unit.h"
#include "number_format.h"
#include <QGuiApplication>
#include <QStyleHints>
#include <QDebug>
#include <QTimer>
#include <QResizeEvent>
#include <QColor>

MainWindow::MainWindow(const QString &numberFontFamily, QWidget *parent)
    : QMainWindow(parent)
    , controller(Preset::Beginner)
    , numberFontFamily(numberFontFamily)
{
    // Kein .ui-Designer-Layout mehr - die komplette UI wird hier programmatisch
    // aufgebaut (Sidebar, TaskView, SettingsView als Overlay/Stack), das alte
    // ui_mainwindow.h/mainwindow.ui haette sowieso nur einen sofort wieder
    // ersetzten centralwidget plus eine leere, ungenutzte Menu-/Statusbar geliefert.
    setWindowTitle("Mathe Trainer");
    resize(900, 650);

    bool isDarkMode = (QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark);
    QString accentColor = "#5B8DEF";
    // Diese eine Stelle bleibt die Quelle der Wahrheit fuers Theme (Punkt 1) - die
    // Textfarbe wird sowohl im Stylesheet unten (%2) als auch an TaskView/WrittenGrid-
    // Widget weitergereicht, damit das Karo-Raster im hellen Theme nicht mehr fest
    // weiss (und damit praktisch unsichtbar) zeichnet.
    QString textColor = isDarkMode ? "#FFFFFF" : "#1E1E1E";

    // taskLabel/feedbackLabel/answerEdit gibt es seit dem Wegfall der einzeiligen
    // Label-Darstellung nicht mehr (siehe task_view.h) - die zugehoerigen Regeln sind
    // raus. textColor (fuer diese Regeln frueher noetig) wird nur noch direkt in C++
    // gebraucht (siehe setInkColor() unten), nicht mehr im Stylesheet selbst - die
    // Platzhalter sind deshalb wieder LUECKENLOS durchnummeriert (%1..%4). Qt's
    // arg()-Kette fuellt bei jedem Aufruf immer die kleinste noch vorhandene Nummer -
    // eine Luecke (z.B. fehlendes %2 bei weiterhin %1,%3,%4,%5) wuerde sonst die
    // nachfolgenden Werte leise auf die falschen Platzhalter verschieben.
    QString styleSheet = QString(R"(
        QMainWindow { background-color: %1; }
        QLineEdit#writtenAnswerDigit { font-family: "%4"; }
        QPushButton#checkButton { background-color: %2; border-radius: 8px; font-size: 16pt; padding: 6px; }
        QPushButton { background-color: %2; border-radius: 6px; padding: 8px 16px; color: white; border: none; }
        QPushButton:hover { background-color: %3; }
    )").arg(isDarkMode ? "#1E1E1E" : "#FAFAFA")
                             .arg(accentColor)
                             .arg("#4A7BDB")
                             .arg(numberFontFamily);

    setStyleSheet(styleSheet);

    stack = new QStackedWidget(this);
    taskView = new TaskView(this);
    taskView->setNumberFontFamily(numberFontFamily);
    taskView->setInkColor(QColor(textColor));
    settingsView = new SettingsView(this);

    stack->addWidget(taskView);
    stack->addWidget(settingsView);

    setCentralWidget(stack);

    sidebar = new SidebarMenu(stack);
    sidebar->move(0, 0);
    sidebar->resize(sidebar->width(), stack->height());
    sidebar->raise();
    sidebar->setAvailableSubcategories("Arithmetik", arithmeticAvailableSubcategories(Preset::Beginner));

    symbolMenu = new SymbolMenu(stack);
    symbolMenu->repositionAt(stack->width(), stack->height());
    symbolMenu->raise();

    connect(settingsView, &SettingsView::taskModeChanged, this, [this](TaskMode mode) {
        stack->setCurrentWidget(taskView);
        sidebar->raise();
        symbolMenu->raise();
        controller.setTaskMode(mode);
        controller.startNewTask();
        showNewTask();
    });

    connect(sidebar, &SidebarMenu::activeSelectionsChanged, this, [this](const QVector<QPair<QString, QString>> &active) {
        stack->setCurrentWidget(taskView);
        sidebar->raise();
        symbolMenu->raise();
        controller.setActiveSelections(active);
        controller.startNewTask();
        showNewTask();
    });

    connect(sidebar, &SidebarMenu::settingsClicked, this, [this]() {
        stack->setCurrentWidget(settingsView);
        sidebar->raise();
        symbolMenu->raise();
    });

    connect(settingsView, &SettingsView::classSelected, this, [this](int schoolClass) {
        stack->setCurrentWidget(taskView);
        sidebar->raise();
        symbolMenu->raise();
        int level = classToLevel(schoolClass);
        controller.setDifficultyLevel(level);
        // setAvailableSubcategories() emittiert absichtlich nichts mehr (Punkt 1) -
        // die evtl. veraenderte Auswahl (manche Unterkategorien koennten bei diesem
        // Level wegfallen) wird hier explizit abgeholt, damit GENAU EINMAL generiert wird.
        sidebar->setAvailableSubcategories("Arithmetik", arithmeticAvailableSubcategories(level));
        controller.setActiveSelections(sidebar->activeSelectionList());
        controller.startNewTask();
        showNewTask();
    });

    connect(settingsView, &SettingsView::worksheetModeChanged, this, [this](bool enabled) {
        // Der Button sitzt jetzt in den Einstellungen - ohne diesen Wechsel wuerde man
        // beim Anklicken auf der Einstellungen-Seite stehen bleiben und nichts sehen,
        // weil das Aufgabenblatt (wie jede Aufgabe) in der TaskView angezeigt wird.
        stack->setCurrentWidget(taskView);
        sidebar->raise();
        symbolMenu->raise();

        if (!enabled) {
            showNewTask();
            taskView->setInputEnabled(true);
            return;
        }

        // 9 unterschiedliche Aufgaben aus den aktuell aktiven Kategorien/dem aktuellen
        // Modus - jede einzeln ueber startNewTask() erzeugt, damit die gleiche Vielfalt
        // entsteht wie beim normalen Durchklicken einzelner Aufgaben.
        QVector<Task> sheet;
        for (int i = 0; i < 9; ++i) {
            controller.startNewTask();
            sheet.append(controller.getCurrentTask());
        }
        taskView->showWorksheet(sheet);

        // Aufgabenblatt zeigt 9 statische Aufgaben zum handschriftlichen Ausfuellen -
        // ohne diese Zeile wuerden Check/Weiter weiterhin die zuletzt (unsichtbar)
        // erzeugte Einzelaufgabe aus der obigen Schleife pruefen bzw. ueberspringen.
        taskView->setInputEnabled(false);
        taskView->setContinueButtonVisible(false);
    });

    connect(taskView, &TaskView::answerSubmitted, this, &MainWindow::onAnswerSubmitted);
    connect(taskView, &TaskView::symbolMenuToggled, symbolMenu, &SymbolMenu::toggleOpen);
    connect(taskView, &TaskView::skipRequested, this, &MainWindow::onSkipRequested);
    connect(taskView, &TaskView::continueRequested, this, &MainWindow::onContinueRequested);
    connect(symbolMenu, &SymbolMenu::symbolSelected, taskView, &TaskView::insertSymbolAtFocus);

    showNewTask();
}

// Formatiert die erwartete(n) Loesung(en) einer Aufgabe fuer die "Falsch - richtig
// waere ..."-Rueckmeldung - die eigentliche Rundungs-/Komma-Logik steckt jetzt in
// formatGermanDecimal() (number_format.h), damit sie nicht doppelt gehalten wird
// (dieselbe Logik braucht z.B. auch der Dezimalzahlen-Generator fuer den Aufgabentext).
QString MainWindow::formatSolution(const Task &task) const
{
    QStringList values;
    for (const AnswerSlot &slot : task.answers) {
        values.append(formatGermanDecimal(slot.expectedValue));
    }
    return values.join(", ");
}

void MainWindow::showNewTask()
{
    taskView->showTask(controller.getCurrentTask());
    taskView->setContinueButtonVisible(false);
}

void MainWindow::onAnswerSubmitted()
{
    QVector<QString> inputs = taskView->currentAnswerTexts();
    QVector<bool> correctness = controller.checkAnswers(inputs);
    taskView->showAnswerColors(correctness);

    bool allCorrect = !correctness.isEmpty() && std::all_of(correctness.begin(), correctness.end(), [](bool c) { return c; });
    // Ein leeres Feld (siehe WrittenGridWidget::currentAnswerText(), Punkt 3) ist
    // "ungueltig", nicht "falsch geloest" - dafuer gibt es eine eigene Rueckmeldung
    // statt einer Loesungsangabe, die dann ja niemand eingetippt hat.
    bool anyEmpty = std::any_of(inputs.begin(), inputs.end(), [](const QString &s) { return s.isEmpty(); });

    if (allCorrect) {
        taskView->showFeedbackText(tr("Richtig!"), true);
    } else if (anyEmpty) {
        taskView->showFeedbackText(tr("Da fehlt noch eine Ziffer."), false);
    } else {
        taskView->showFeedbackText(tr("Falsch – richtig wäre %1").arg(formatSolution(controller.getCurrentTask())), false);
    }
    taskView->setInputEnabled(false);

    // Bei falscher Antwort NIE automatisch weiterspringen (auch nicht im Kopfrechnen-
    // Modus) - sonst ist die gerade angezeigte Loesung nach 1,5 Sekunden schon wieder
    // weg, bevor man sie gelesen hat. autoAdvance gilt deshalb nur noch zusaetzlich zur
    // ohnehin schon aufgabenabhaengigen Einstellung (siehe Task::autoAdvance).
    bool autoAdvance = allCorrect && controller.getCurrentTask().autoAdvance;

    if (autoAdvance) {
        QTimer::singleShot(1500, this, [this]() {
            controller.startNewTask();
            showNewTask();
            taskView->setInputEnabled(true);
            taskView->focusFirstField();
        });
    } else {
        taskView->setContinueButtonVisible(true);
    }
}

void MainWindow::onSkipRequested()
{
    controller.startNewTask();
    showNewTask();
    taskView->setInputEnabled(true);
    taskView->focusFirstField();
}

void MainWindow::onContinueRequested()
{
    controller.startNewTask();
    showNewTask();
    taskView->setInputEnabled(true);
    taskView->focusFirstField();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    if (sidebar) sidebar->resize(sidebar->width(), centralWidget()->height());
    if (symbolMenu) symbolMenu->repositionAt(centralWidget()->width(), centralWidget()->height());
}