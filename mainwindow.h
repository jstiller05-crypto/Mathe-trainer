#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "session_controller.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onCheckButtonClicked();

private:
    Ui::MainWindow *ui;
    SessionController controller;   // <- ersetzt "Task currentTask"
    void displayCurrentTask();       // <- umbenannt, zeigt nur noch an, generiert nicht mehr selbst
};

#endif