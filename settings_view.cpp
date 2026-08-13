#include "settings_view.h"
#include <QVBoxLayout>
#include <QLabel>

SettingsView::SettingsView(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *placeholder = new QLabel("Einstellungen (folgt später)", this);
    placeholder->setAlignment(Qt::AlignCenter);
    layout->addWidget(placeholder);
}