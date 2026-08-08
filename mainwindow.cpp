#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "operation.h"
#include "task_generator.h"
#include "difficulty.h"
#include <QPushButton>
#include <QString>
#include <cstdlib>
#include <ctime>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    srand(time(nullptr));

    connect(ui->checkButton, &QPushButton::clicked, this, &MainWindow::onCheckButtonClicked);

    displayNewTask();
}

void MainWindow::displayNewTask()
{
    currentTask = generateTask(Difficulty::Beginner);
    QString text = QString("%1 %2 %3 = ?")
                       .arg(currentTask.firstNumber)
                       .arg(operationToSymbol(currentTask.operation))   // <- statt currentTask.operation direkt
                       .arg(currentTask.secondNumber);
    ui->taskLabel->setText(text);
    ui->answerEdit->clear();
    ui->feedbackLabel->setText("");
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::onCheckButtonClicked()
{
    bool isNumber;
    int answer = ui->answerEdit->text().toInt(&isNumber);

    if (!isNumber) {
        ui->feedbackLabel->setText("Please enter a number!");
        return;
    }

    if (answer == currentTask.solution)
        ui->feedbackLabel->setText("Correct!");
    else
        ui->feedbackLabel->setText(QString("Wrong. The answer was: %1").arg(currentTask.solution));

    ui->checkButton->setEnabled(false);   // verhindert Mehrfach-Klicks während der Pause
    ui->answerEdit->setEnabled(false);

    // Nach 1500 Millisekunden (1,5 Sek.) wird die übergebene Funktion EINMALIG ausgeführt
    QTimer::singleShot(1500, this, [this]() {
        displayNewTask();
        ui->checkButton->setEnabled(true);
        ui->answerEdit->setEnabled(true);
        ui->answerEdit->setFocus();   // Cursor direkt wieder ins Eingabefeld
    });
}