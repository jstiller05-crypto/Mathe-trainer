#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "session_controller.h"
#include "sidebar_menu.h"
#include "task_view.h"
#include "settings_view.h"
#include "symbol_menu.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    Ui::MainWindow *ui;
    SessionController controller;
    SidebarMenu *sidebar;
    QStackedWidget *stack;
    TaskView *taskView;
    SettingsView *settingsView;
    SymbolMenu *symbolMenu;

    int classToLevel(int schoolClass) const;
    void showNewTask();
    void onAnswerSubmitted();
    void onSkipRequested();
    void onContinueRequested();
};

#endif