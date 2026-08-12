/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *mainLayout;
    QSpacerItem *topSpacer;
    QLabel *taskLabel;
    QHBoxLayout *answerRow;
    QSpacerItem *leftAnswerSpacer;
    QLineEdit *answerEdit;
    QPushButton *checkButton;
    QSpacerItem *rightAnswerSpacer;
    QLabel *feedbackLabel;
    QSpacerItem *bottomSpacer;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(900, 650);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        mainLayout = new QVBoxLayout(centralwidget);
        mainLayout->setSpacing(24);
        mainLayout->setObjectName("mainLayout");
        mainLayout->setContentsMargins(60, 40, 60, 40);
        topSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        mainLayout->addItem(topSpacer);

        taskLabel = new QLabel(centralwidget);
        taskLabel->setObjectName("taskLabel");
        taskLabel->setAlignment(Qt::AlignCenter);

        mainLayout->addWidget(taskLabel);

        answerRow = new QHBoxLayout();
        answerRow->setObjectName("answerRow");
        leftAnswerSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        answerRow->addItem(leftAnswerSpacer);

        answerEdit = new QLineEdit(centralwidget);
        answerEdit->setObjectName("answerEdit");
        answerEdit->setAlignment(Qt::AlignCenter);

        answerRow->addWidget(answerEdit);

        checkButton = new QPushButton(centralwidget);
        checkButton->setObjectName("checkButton");
        checkButton->setMaximumWidth(44);

        answerRow->addWidget(checkButton);

        rightAnswerSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        answerRow->addItem(rightAnswerSpacer);


        mainLayout->addLayout(answerRow);

        feedbackLabel = new QLabel(centralwidget);
        feedbackLabel->setObjectName("feedbackLabel");
        feedbackLabel->setAlignment(Qt::AlignCenter);

        mainLayout->addWidget(feedbackLabel);

        bottomSpacer = new QSpacerItem(20, 60, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        mainLayout->addItem(bottomSpacer);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 900, 17));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Mathe Trainer", nullptr));
        taskLabel->setText(QCoreApplication::translate("MainWindow", "Task", nullptr));
        checkButton->setText(QCoreApplication::translate("MainWindow", "\342\234\223", nullptr));
        feedbackLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
