#include "symbol_menu.h"

SymbolMenu::SymbolMenu(QWidget *parent)
    : QWidget(parent)
{
    resize(menuWidth, collapsedHeight);

    QGridLayout *grid = new QGridLayout(this);
    grid->setSpacing(4);
    grid->setContentsMargins(8, 8, 8, 8);

    // Startbelegung 3x3 - Untermenüs (z.B. eigene Trigonometrie-Gruppe) heben wir uns für später auf
    QStringList symbols = { "√", "^", "π", "sin", "cos", "tan", "×", "÷", "°" };

    int row = 0, col = 0;
    for (const QString &symbol : symbols) {
        QPushButton *button = new QPushButton(symbol, this);
        button->setObjectName("symbolButton");
        connect(button, &QPushButton::clicked, this, [this, symbol]() {
            emit symbolSelected(symbol);
        });
        grid->addWidget(button, row, col);
        col++;
        if (col >= 3) { col = 0; row++; }
    }

    heightAnimation = new QPropertyAnimation(this, "menuHeight", this);
    heightAnimation->setDuration(300);

    collapseTimer = new QTimer(this);
    collapseTimer->setSingleShot(true);
    collapseTimer->setInterval(1000);
    connect(collapseTimer, &QTimer::timeout, this, [this]() {
        if (expanded) toggleOpen();
    });

    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet(R"(
        SymbolMenu {
            background-color: rgba(128, 128, 128, 76);
            border: 1px solid rgba(255, 255, 255, 30);
            border-radius: 8px;
        }
        QPushButton#symbolButton {
            background-color: transparent;
            border: 1px solid transparent;
            font-size: 14pt;
            color: white;
            border-radius: 6px;
        }
        QPushButton#symbolButton:hover {
            border: 1px solid #5B8DEF;
        }
    )");
}

int SymbolMenu::menuHeight() const
{
    return height();
}

void SymbolMenu::setMenuHeight(int newHeight)
{
    resize(menuWidth, newHeight);
    updatePosition();   // Unterkante bleibt fix - Menü wächst nach OBEN, nicht nach unten
}

void SymbolMenu::updatePosition()
{
    int x = (lastParentWidth - width()) / 2;
    int y = lastParentHeight - height() - 60;   // 60px Abstand vom unteren Rand für das Auslöse-Icon
    move(x, y);
}

void SymbolMenu::repositionAt(int parentWidth, int parentHeight)
{
    lastParentWidth = parentWidth;
    lastParentHeight = parentHeight;
    updatePosition();
}

void SymbolMenu::toggleOpen()
{
    expanded = !expanded;
    int target = expanded ? expandedHeight : collapsedHeight;

    heightAnimation->stop();
    heightAnimation->setStartValue(menuHeight());
    heightAnimation->setEndValue(target);
    heightAnimation->start();
}

void SymbolMenu::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    if (expanded) collapseTimer->start();
}

void SymbolMenu::enterEvent(QEnterEvent *event)
{
    Q_UNUSED(event);
    collapseTimer->stop();
}