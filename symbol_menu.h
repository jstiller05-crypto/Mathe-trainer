#ifndef SYMBOL_MENU_H
#define SYMBOL_MENU_H

#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QPropertyAnimation>
#include <QTimer>
#include <QStringList>

class SymbolMenu : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int menuHeight READ menuHeight WRITE setMenuHeight)

public:
    explicit SymbolMenu(QWidget *parent = nullptr);

    int menuHeight() const;
    void setMenuHeight(int height);

    void toggleOpen();
    void repositionAt(int parentWidth, int parentHeight);

signals:
    void categorySelected(const QString &category, const QString &subcategory);
    void settingsClicked();
    void symbolSelected(const QString &symbol);

protected:
    void leaveEvent(QEvent *event) override;
    void enterEvent(QEnterEvent *event) override;

private:
    bool expanded = false;
    QPropertyAnimation *heightAnimation;
    QTimer *collapseTimer;
    QPushButton *mentalMathButton;
    int lastParentWidth = 0;
    int lastParentHeight = 0;

    static constexpr int collapsedHeight = 0;
    static constexpr int expandedHeight = 180;
    static constexpr int menuWidth = 200;

    void updatePosition();
};

#endif