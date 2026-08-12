#ifndef SIDEBAR_MENU_H
#define SIDEBAR_MENU_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QPropertyAnimation>
#include <QTimer>
#include <QVector>

class SidebarMenu : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int barWidth READ barWidth WRITE setBarWidth)

public:
    explicit SidebarMenu(QWidget *parent = nullptr);

    int barWidth() const;
    void setBarWidth(int width);

signals:
    void classSelected(int schoolClass);
    void settingsClicked();

protected:
    void leaveEvent(QEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    bool expanded = false;
    QPropertyAnimation *widthAnimation;
    QTimer *collapseTimer;
    QVector<QPushButton*> classButtons;
    QPushButton *studentButton;
    QPushButton *settingsButton;

    static constexpr int collapsedWidth = 60;
    static constexpr int expandedWidth = 220;

    void toggleExpanded();
    void updateButtonLabels();
};

#endif