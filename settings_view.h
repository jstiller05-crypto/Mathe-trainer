#ifndef SETTINGS_VIEW_H
#define SETTINGS_VIEW_H

#include <QWidget>
#include "difficulty.h"

class SettingsView : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsView(QWidget *parent = nullptr);

signals:
    void classSelected(int schoolClass);
    void taskModeChanged(TaskMode mode);
    void worksheetModeChanged(bool enabled);
};

#endif