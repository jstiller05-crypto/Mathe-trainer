#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "operation.h"
#include <QPushButton>
#include <QString>
#include <QTimer>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , controller(Difficulty::Beginner)   // <- Controller wird hier mit Startwert erzeugt
{
    ui->setupUi(this);
    connect(ui->checkButton, &QPushButton::clicked, this, &MainWindow::onCheckButtonClicked);
    displayCurrentTask();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::displayCurrentTask()
{
    Task task = controller.getCurrentTask();   // <- fragt beim Controller nach, generiert selbst nichts

    QString text = QString("%1 %2 %3 = ?")
                       .arg(task.firstNumber)
                       .arg(operationToSymbol(task.operation))
                       .arg(task.secondNumber);

    ui->taskLabel->setText(text);
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

    if (controller.checkAnswer(answer))       // <- fragt den Controller, statt selbst zu vergleichen
        ui->feedbackLabel->setText("Correct!");
    else
        ui->feedbackLabel->setText(QString("Wrong. The answer was: %1").arg(controller.getCurrentTask().solution));

    ui->checkButton->setEnabled(false);
    ui->answerEdit->setEnabled(false);

    QTimer::singleShot(1500, this, [this]() {
        controller.startNewTask();            // <- Controller erzeugt die neue Aufgabe
        displayCurrentTask();                  // <- MainWindow zeigt sie nur noch an
        ui->checkButton->setEnabled(true);
        ui->answerEdit->setEnabled(true);
        ui->answerEdit->setFocus();
    });
}