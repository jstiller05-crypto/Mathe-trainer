#include "sidebar_menu.h"
#include <QMouseEvent>

SidebarMenu::SidebarMenu(QWidget *parent)
    : QWidget(parent)
{
    resize(collapsedWidth, 400);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(4);
    layout->setContentsMargins(8, 20, 8, 20);

    for (int schoolClass = 3; schoolClass <= 10; ++schoolClass) {
        QPushButton *button = new QPushButton(QString::number(schoolClass), this);
        button->setObjectName("classButton");
        button->setProperty("schoolClass", schoolClass);
        button->installEventFilter(this);

        connect(button, &QPushButton::clicked, this, [this, schoolClass]() {
            emit classSelected(schoolClass);
        });

        classButtons.append(button);
        layout->addWidget(button);
    }

    studentButton = new QPushButton("—", this);
    studentButton->setObjectName("studentButton");
    studentButton->installEventFilter(this);
    layout->addWidget(studentButton);

    layout->addStretch();

    settingsButton = new QPushButton("⚙", this);
    settingsButton->setObjectName("settingsButton");
    settingsButton->installEventFilter(this);
    connect(settingsButton, &QPushButton::clicked, this, [this]() {
        emit settingsClicked();
    });
    layout->addWidget(settingsButton);

    // --- Animation fürs Aus-/Einklappen ---
    widthAnimation = new QPropertyAnimation(this, "barWidth", this);
    widthAnimation->setDuration(300);
    connect(widthAnimation, &QPropertyAnimation::finished, this, [this]() {
        if (!expanded) {
            updateButtonLabels();
        }
    });

    // --- Timer für die Verzögerung vorm Einklappen ---
    collapseTimer = new QTimer(this);
    collapseTimer->setSingleShot(true);
    collapseTimer->setInterval(1000);
    connect(collapseTimer, &QTimer::timeout, this, [this]() {
        if (expanded) {
            toggleExpanded();
        }
    });

    setAttribute(Qt::WA_StyledBackground, true);

    setStyleSheet(R"(
        SidebarMenu {
            background-color: rgba(128, 128, 128, 76);
            border-right: 1px solid rgba(255, 255, 255, 30);
        }
        QPushButton#classButton, QPushButton#studentButton, QPushButton#settingsButton {
            background-color: transparent;
            border: 1px solid transparent;
            font-size: 13pt;
            padding: 10px;
            border-radius: 6px;
            text-align: left;
            color: white;
        }
        QPushButton#classButton:hover, QPushButton#studentButton:hover, QPushButton#settingsButton:hover {
            border: 1px solid #5B8DEF;
        }
    )");
}

int SidebarMenu::barWidth() const
{
    return width();
}

void SidebarMenu::setBarWidth(int newWidth)
{
    resize(newWidth, height());
}

void SidebarMenu::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    if (expanded) {
        collapseTimer->start();
    }
}

void SidebarMenu::enterEvent(QEnterEvent *event)
{
    Q_UNUSED(event);
    collapseTimer->stop();
}

void SidebarMenu::toggleExpanded()
{
    expanded = !expanded;
    int targetWidth = expanded ? expandedWidth : collapsedWidth;

    if (expanded) {
        updateButtonLabels();
    }

    widthAnimation->stop();
    widthAnimation->setStartValue(barWidth());
    widthAnimation->setEndValue(targetWidth);
    widthAnimation->start();
}

void SidebarMenu::updateButtonLabels()
{
    for (QPushButton *button : classButtons) {
        int schoolClass = button->property("schoolClass").toInt();
        button->setText(expanded ? QString("Klasse %1").arg(schoolClass) : QString::number(schoolClass));
    }
    studentButton->setText(expanded ? "Studenten-Niveau" : "—");
    settingsButton->setText(expanded ? "Einstellungen" : "⚙");
}

bool SidebarMenu::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress && !expanded) {
        toggleExpanded();
        return true;
    }
    return QWidget::eventFilter(watched, event);
}