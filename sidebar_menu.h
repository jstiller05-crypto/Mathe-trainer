#ifndef SIDEBAR_MENU_H
#define SIDEBAR_MENU_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QPropertyAnimation>
#include <QTimer>
#include <QVector>
#include <QString>

class SidebarMenu : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int barWidth READ barWidth WRITE setBarWidth)

public:
    explicit SidebarMenu(QWidget *parent = nullptr);

    int barWidth() const;
    void setBarWidth(int width);

signals:
    void categorySelected(const QString &category, const QString &subcategory);
    void settingsClicked();

protected:
    void leaveEvent(QEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    struct CategoryBlock {
        QString fullName;
        QPushButton *headerButton;
        QWidget *subContainer;
        bool subVisible = false;
    };

    bool expanded = false;
    QPropertyAnimation *widthAnimation;
    QTimer *collapseTimer;
    QVBoxLayout *layout;
    QPushButton *settingsButton;
    QVector<CategoryBlock> categoryBlocks;

    static constexpr int collapsedWidth = 60;
    static constexpr int expandedWidth = 240;

    void toggleExpanded();
    void updateLabels();
    void buildCategoryTree();
};

#endif