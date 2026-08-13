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

    // --- Styling (bleibt gleich, wirkt weiterhin auf alle Views) ---
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

    // --- Views erzeugen, in einen Stack packen ---
    stack = new QStackedWidget(this);
    taskView = new TaskView(this);
    settingsView = new SettingsView(this);

    stack->addWidget(taskView);       // Index 0
    stack->addWidget(settingsView);    // Index 1
    // spätere Views: einfach stack->addWidget(statisticsView) ergänzen

    setCentralWidget(stack);

    // --- Sidebar als Overlay über dem Stack ---
    sidebar = new SidebarMenu(stack);
    sidebar->move(0, 0);
    sidebar->resize(sidebar->width(), stack->height());
    sidebar->raise();

    // --- Sidebar-Signale: MainWindow entscheidet, was passiert ---
    connect(sidebar, &SidebarMenu::classSelected, this, [this](int schoolClass) {
        stack->setCurrentWidget(taskView);
        sidebar->raise();
        controller.setDifficultyLevel(classToLevel(schoolClass));
        controller.startNewTask();
        showNewTask();
    });

    connect(sidebar, &SidebarMenu::settingsClicked, this, [this]() {
        stack->setCurrentWidget(settingsView);
        sidebar->raise();
    });
    // --- TaskView-Signal: MainWindow fragt beim Controller nach ---
    connect(taskView, &TaskView::answerSubmitted, this, &MainWindow::onAnswerSubmitted);

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
    taskView->showQuestion(controller.getCurrentTask().questionText);
}

void MainWindow::onAnswerSubmitted()
{
    bool isNumber;
    int answer = taskView->currentAnswerText().toInt(&isNumber);

    if (!isNumber) {
        taskView->showFeedback("Please enter a number!");
        return;
    }

    if (controller.checkAnswer(answer))
        taskView->showFeedback("Correct!");
    else
        taskView->showFeedback(QString("Wrong. The answer was: %1").arg(controller.getCurrentTask().solution));

    taskView->setInputEnabled(false);

    QTimer::singleShot(1500, this, [this]() {
        controller.startNewTask();
        showNewTask();
        taskView->setInputEnabled(true);
        taskView->focusAnswerField();
    });
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    if (sidebar) {
        sidebar->resize(sidebar->width(), centralWidget()->height());
    }
}