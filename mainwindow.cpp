#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "operation.h"
#include "difficulty.h"
#include <QPushButton>
#include <QString>
#include <QTimer>
#include <QDebug>
#include <QGuiApplication>
#include <QStyleHints>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , controller(Preset::Beginner)
{
    ui->setupUi(this);

    bool isDarkMode = (QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark);
    QString accentColor = "#5B8DEF";

    QString styleSheet = QString(R"(
        QMainWindow {
            background-color: %1;
        }
        QLabel#taskLabel {
            font-size: 22pt;
            color: %2;
        }
        QPushButton {
            background-color: %3;
            border-radius: 6px;
            padding: 8px 16px;
            color: white;
        }
        QPushButton:hover {
            background-color: %4;
        }
    )").arg(isDarkMode ? "#1E1E1E" : "#FAFAFA")
                             .arg(isDarkMode ? "#FFFFFF" : "#1E1E1E")
                             .arg(accentColor)
                             .arg(accentColor);

    setStyleSheet(styleSheet);

    connect(ui->checkButton, &QPushButton::clicked, this, &MainWindow::onCheckButtonClicked);

    connect(ui->beginnerButton, &QPushButton::clicked, this, [this]() {
        controller.setDifficultyLevel(Preset::Beginner);
        controller.startNewTask();
        displayCurrentTask();
    });

    connect(ui->middleButton, &QPushButton::clicked, this, [this]() {
        controller.setDifficultyLevel(Preset::Intermediate);
        controller.startNewTask();
        displayCurrentTask();
    });

    connect(ui->advancedButton, &QPushButton::clicked, this, [this]() {
        controller.setDifficultyLevel(Preset::Advanced);
        controller.startNewTask();
        displayCurrentTask();
    });

    displayCurrentTask();
}

MainWindow::~MainWindow()
{
    delete ui;
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