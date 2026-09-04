#include "mainwindow.h"
#include "arithmetic_unit.h"
#include <QGuiApplication>
#include <QStyleHints>
#include <QDebug>
#include <QTimer>
#include <QResizeEvent>

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

    QString styleSheet = QString(R"(
        QMainWindow { background-color: %1; }
        QLabel#taskLabel { font-size: 32pt; font-weight: 600; color: %2; font-family: "%5"; }
        QLabel#feedbackLabel { font-size: 13pt; color: %2; min-height: 20px; }
        QLineEdit#answerEdit {
            font-size: 18pt; padding: 10px 16px; border: 2px solid %3;
            border-radius: 8px; background-color: %1; color: %2;
            min-width: 160px; max-width: 220px; font-family: "%5";
        }
        QLineEdit#answerEdit:focus { border: 2px solid %4; }
        QLineEdit#writtenAnswerDigit { font-family: "%5"; }
        QPushButton#checkButton { background-color: %3; border-radius: 8px; font-size: 16pt; padding: 6px; }
        QPushButton { background-color: %3; border-radius: 6px; padding: 8px 16px; color: white; border: none; }
        QPushButton:hover { background-color: %4; }
    )").arg(isDarkMode ? "#1E1E1E" : "#FAFAFA")
                             .arg(isDarkMode ? "#FFFFFF" : "#1E1E1E")
                             .arg(accentColor)
                             .arg("#4A7BDB")
                             .arg(numberFontFamily);   // NEU - %5

    setStyleSheet(styleSheet);

    stack = new QStackedWidget(this);
    taskView = new TaskView(this);
    taskView->setNumberFontFamily(numberFontFamily);
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
        sidebar->setAvailableSubcategories("Arithmetik", arithmeticAvailableSubcategories(level));
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

        if (!enabled) { showNewTask(); return; }

        // 9 unterschiedliche Aufgaben aus den aktuell aktiven Kategorien/dem aktuellen
        // Modus - jede einzeln ueber startNewTask() erzeugt, damit die gleiche Vielfalt
        // entsteht wie beim normalen Durchklicken einzelner Aufgaben.
        QVector<Task> sheet;
        for (int i = 0; i < 9; ++i) {
            controller.startNewTask();
            sheet.append(controller.getCurrentTask());
        }
        taskView->showWorksheet(sheet);
    });

    connect(taskView, &TaskView::answerSubmitted, this, &MainWindow::onAnswerSubmitted);
    connect(taskView, &TaskView::symbolMenuToggled, symbolMenu, &SymbolMenu::toggleOpen);
    connect(taskView, &TaskView::skipRequested, this, &MainWindow::onSkipRequested);
    connect(taskView, &TaskView::continueRequested, this, &MainWindow::onContinueRequested);
    connect(symbolMenu, &SymbolMenu::symbolSelected, taskView, &TaskView::insertSymbolAtFocus);

    showNewTask();
}

int MainWindow::classToLevel(int schoolClass) const
{
    return (schoolClass - 3) * 10 + 1;
}

void MainWindow::showNewTask()
{
    taskView->showTask(controller.getCurrentTask());
    taskView->setContinueButtonVisible(false);
}

void MainWindow::onAnswerSubmitted()
{
    QVector<bool> correctness = controller.checkAnswers(taskView->currentAnswerTexts());
    taskView->showAnswerColors(correctness);

    bool allCorrect = std::all_of(correctness.begin(), correctness.end(), [](bool c) { return c; });
    taskView->showFeedbackText(allCorrect ? "Correct!" : "Not quite right.");
    taskView->setInputEnabled(false);

    bool autoAdvance = controller.getCurrentTask().autoAdvance;

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