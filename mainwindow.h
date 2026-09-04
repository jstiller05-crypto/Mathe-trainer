#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "session_controller.h"
#include "sidebar_menu.h"
#include "task_view.h"
#include "settings_view.h"
#include "symbol_menu.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString &numberFontFamily, QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QString numberFontFamily;

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