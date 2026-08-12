#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "operation.h"
#include "difficulty.h"
#include "sidebar_menu.h"
#include <QPushButton>
#include <QString>
#include <QTimer>
#include <QDebug>
#include <QGuiApplication>
#include <QStyleHints>
#include <QResizeEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , controller(Preset::Beginner)
{
    ui->setupUi(this);

    // --- Styling ---
    bool isDarkMode = (QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark);
    QString accentColor = "#5B8DEF";

    QString styleSheet = QString(R"(
        QMainWindow {
            background-color: %1;
        }
        QLabel#taskLabel {
            font-size: 32pt;
            font-weight: 600;
            color: %2;
        }
        QLabel#feedbackLabel {
            font-size: 13pt;
            color: %2;
            min-height: 20px;
        }
        QLineEdit#answerEdit {
            font-size: 18pt;
            padding: 10px 16px;
            border: 2px solid %3;
            border-radius: 8px;
            background-color: %1;
            color: %2;
            min-width: 160px;
            max-width: 220px;
        }
        QLineEdit#answerEdit:focus {
            border: 2px solid %4;
        }
        QPushButton#checkButton {
            background-color: %3;
            border-radius: 8px;
            font-size: 16pt;
            padding: 6px;
        }
        QPushButton {
            background-color: %3;
            border-radius: 6px;
            padding: 8px 16px;
            color: white;
            border: none;
        }
        QPushButton:hover {
            background-color: %4;
        }
    )").arg(isDarkMode ? "#1E1E1E" : "#FAFAFA")
                             .arg(isDarkMode ? "#FFFFFF" : "#1E1E1E")
                             .arg(accentColor)
                             .arg("#4A7BDB");

    setStyleSheet(styleSheet);

    // --- Sidebar einbauen: als freischwebendes Overlay über dem bestehenden Inhalt ---
    sidebar = new SidebarMenu(centralWidget());
    sidebar->move(0, 0);
    sidebar->resize(sidebar->width(), centralWidget()->height());
    sidebar->raise();

    // --- Sidebar-Signale verbinden ---
    connect(sidebar, &SidebarMenu::classSelected, this, [this](int schoolClass) {
        int level = classToLevel(schoolClass);
        controller.setDifficultyLevel(level);
        controller.startNewTask();
        displayCurrentTask();
    });

    connect(sidebar, &SidebarMenu::settingsClicked, this, []() {
        qDebug() << "Settings clicked - not implemented yet";
    });

    // --- Aufgaben-Bereich verbinden ---
    connect(ui->checkButton, &QPushButton::clicked, this, &MainWindow::onCheckButtonClicked);
    connect(ui->answerEdit, &QLineEdit::returnPressed, this, &MainWindow::onCheckButtonClicked);

    displayCurrentTask();
}

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::classToLevel(int schoolClass) const
{
    return (schoolClass - 3) * 10 + 1;
}

void MainWindow::displayCurrentTask()
{
    Task task = controller.getCurrentTask();
    ui->taskLabel->setText(task.questionText);
    ui->answerEdit->clear();
    ui->feedbackLabel->setText("");
}

void MainWindow::onCheckButtonClicked()
{
    bool isNumber;
    int answer = ui->answerEdit->text().toInt(&isNumber);

    if (!isNumber) {
        qDebug() << "Invalid input, not a number";
        ui->feedbackLabel->setText("Please enter a number!");
        return;
    }

    if (controller.checkAnswer(answer))
        ui->feedbackLabel->setText("Correct!");
    else
        ui->feedbackLabel->setText(QString("Wrong. The answer was: %1").arg(controller.getCurrentTask().solution));

    ui->checkButton->setEnabled(false);
    ui->answerEdit->setEnabled(false);

    QTimer::singleShot(1500, this, [this]() {
        controller.startNewTask();
        displayCurrentTask();
        ui->checkButton->setEnabled(true);
        ui->answerEdit->setEnabled(true);
        ui->answerEdit->setFocus();
    });
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    if (sidebar) {
        sidebar->resize(sidebar->width(), centralWidget()->height());
    }
}