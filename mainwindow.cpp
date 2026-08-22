#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGuiApplication>
#include <QStyleHints>
#include <QDebug>
#include <QTimer>
#include <QResizeEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , controller(Preset::Beginner)
{
    ui->setupUi(this);

    bool isDarkMode = (QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark);
    QString accentColor = "#5B8DEF";

    QString styleSheet = QString(R"(
        QMainWindow { background-color: %1; }
        QLabel#taskLabel { font-size: 32pt; font-weight: 600; color: %2; }
        QLabel#feedbackLabel { font-size: 13pt; color: %2; min-height: 20px; }
        QLineEdit#answerEdit {
            font-size: 18pt; padding: 10px 16px; border: 2px solid %3;
            border-radius: 8px; background-color: %1; color: %2;
            min-width: 160px; max-width: 220px;
        }
        QLineEdit#answerEdit:focus { border: 2px solid %4; }
        QPushButton#checkButton { background-color: %3; border-radius: 8px; font-size: 16pt; padding: 6px; }
        QPushButton { background-color: %3; border-radius: 6px; padding: 8px 16px; color: white; border: none; }
        QPushButton:hover { background-color: %4; }
    )").arg(isDarkMode ? "#1E1E1E" : "#FAFAFA")
                             .arg(isDarkMode ? "#FFFFFF" : "#1E1E1E")
                             .arg(accentColor)
                             .arg("#4A7BDB");

    setStyleSheet(styleSheet);

    stack = new QStackedWidget(this);
    taskView = new TaskView(this);
    settingsView = new SettingsView(this);

    stack->addWidget(taskView);
    stack->addWidget(settingsView);

    setCentralWidget(stack);

    sidebar = new SidebarMenu(stack);
    sidebar->move(0, 0);
    sidebar->resize(sidebar->width(), stack->height());
    sidebar->raise();

    symbolMenu = new SymbolMenu(stack);
    symbolMenu->repositionAt(stack->width(), stack->height());
    symbolMenu->raise();

    connect(sidebar, &SidebarMenu::mentalMathModeChanged, this, [this](bool enabled) {
        controller.setMentalMathMode(enabled);
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
        controller.setDifficultyLevel(classToLevel(schoolClass));
        controller.startNewTask();
        showNewTask();
    });

    connect(taskView, &TaskView::answerSubmitted, this, &MainWindow::onAnswerSubmitted);
    connect(taskView, &TaskView::symbolMenuToggled, symbolMenu, &SymbolMenu::toggleOpen);
    connect(taskView, &TaskView::skipRequested, this, &MainWindow::onSkipRequested);
    connect(taskView, &TaskView::continueRequested, this, &MainWindow::onContinueRequested);
    connect(symbolMenu, &SymbolMenu::symbolSelected, taskView, &TaskView::insertSymbolAtFocus);

    showNewTask();
}

MainWindow::~MainWindow()
{
    delete ui;
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