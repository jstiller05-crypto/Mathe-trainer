#ifndef SIDEBAR_MENU_H
#define SIDEBAR_MENU_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QPropertyAnimation>
#include <QTimer>
#include <QVector>
#include <QPair>
#include <QString>

class SidebarMenu : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int barWidth READ barWidth WRITE setBarWidth)

public:
    explicit SidebarMenu(QWidget *parent = nullptr);

    int barWidth() const;
    void setBarWidth(int width);
    void setAvailableSubcategories(const QString &category, const QStringList &availableSubcategories);

signals:
    void settingsClicked();
    void mentalMathModeChanged(bool enabled);
    void activeSelectionsChanged(const QVector<QPair<QString, QString>> &active);
    void testWrittenGridRequested();

protected:
    void leaveEvent(QEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    struct CategoryBlock {
        QString fullName;
        QPushButton *headerButton;
        QWidget *subContainer;
        QVector<QPushButton*> subButtons;
        bool subVisible = false;
    };

    bool expanded = false;
    QPropertyAnimation *widthAnimation;
    QTimer *collapseTimer;
    QVBoxLayout *layout;
    QPushButton *settingsButton;
    QPushButton *mentalMathButton;
    QPushButton *testGridButton;
    QVector<CategoryBlock> categoryBlocks;
    QVector<QPair<QString, QString>> activeSelections;


    static constexpr int collapsedWidth = 60;
    static constexpr int expandedWidth = 240;

    void toggleExpanded();
    void updateLabels();
    void buildCategoryTree();
};

#endif