/********************************************************************************
** Form generated from reading UI file 'gameui.ui'
**
** Created by: Qt User Interface Compiler version 5.12.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GAMEUI_H
#define UI_GAMEUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GameUIClass
{
public:
    QWidget *centralWidget;
    QWidget *widget;
    QPushButton *start_pushButton_1;
    QPushButton *end_pushButton_1;
    QPushButton *main_pushButton_2;
    QPushButton *main_pushButton_1;
    QFrame *main_line;
    QLabel *end_label;
    QLabel *start_label;
    QWidget *widgetForDX;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *GameUIClass)
    {
        if (GameUIClass->objectName().isEmpty())
            GameUIClass->setObjectName(QString::fromUtf8("GameUIClass"));
        GameUIClass->resize(1454, 1002);
        centralWidget = new QWidget(GameUIClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        widget = new QWidget(centralWidget);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setGeometry(QRect(10, 10, 1431, 931));
        start_pushButton_1 = new QPushButton(widget);
        start_pushButton_1->setObjectName(QString::fromUtf8("start_pushButton_1"));
        start_pushButton_1->setGeometry(QRect(550, 350, 121, 71));
        end_pushButton_1 = new QPushButton(widget);
        end_pushButton_1->setObjectName(QString::fromUtf8("end_pushButton_1"));
        end_pushButton_1->setGeometry(QRect(550, 270, 121, 71));
        main_pushButton_2 = new QPushButton(widget);
        main_pushButton_2->setObjectName(QString::fromUtf8("main_pushButton_2"));
        main_pushButton_2->setGeometry(QRect(1160, 70, 111, 28));
        main_pushButton_2->setAutoRepeatInterval(100);
        main_pushButton_1 = new QPushButton(widget);
        main_pushButton_1->setObjectName(QString::fromUtf8("main_pushButton_1"));
        main_pushButton_1->setGeometry(QRect(1160, 35, 110, 28));
        main_pushButton_1->setCheckable(false);
        main_line = new QFrame(widget);
        main_line->setObjectName(QString::fromUtf8("main_line"));
        main_line->setGeometry(QRect(290, 630, 721, 20));
        main_line->setStyleSheet(QString::fromUtf8("color: rgb(225, 0, 0);"));
        main_line->setFrameShadow(QFrame::Plain);
        main_line->setLineWidth(20);
        main_line->setMidLineWidth(10);
        main_line->setFrameShape(QFrame::HLine);
        end_label = new QLabel(widget);
        end_label->setObjectName(QString::fromUtf8("end_label"));
        end_label->setGeometry(QRect(0, 10, 1281, 671));
        start_label = new QLabel(widget);
        start_label->setObjectName(QString::fromUtf8("start_label"));
        start_label->setGeometry(QRect(13, 4, 1271, 681));
        widgetForDX = new QWidget(widget);
        widgetForDX->setObjectName(QString::fromUtf8("widgetForDX"));
        widgetForDX->setGeometry(QRect(0, 0, 1291, 761));
        widgetForDX->raise();
        end_label->raise();
        start_label->raise();
        end_pushButton_1->raise();
        start_pushButton_1->raise();
        main_pushButton_2->raise();
        main_pushButton_1->raise();
        main_line->raise();
        GameUIClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(GameUIClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1454, 26));
        GameUIClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(GameUIClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        mainToolBar->setEnabled(true);
        GameUIClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(GameUIClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        statusBar->setEnabled(true);
        GameUIClass->setStatusBar(statusBar);

        retranslateUi(GameUIClass);
        QObject::connect(main_pushButton_2, SIGNAL(clicked()), GameUIClass, SLOT(slot2()));
        QObject::connect(main_pushButton_1, SIGNAL(clicked()), GameUIClass, SLOT(slot1()));

        QMetaObject::connectSlotsByName(GameUIClass);
    } // setupUi

    void retranslateUi(QMainWindow *GameUIClass)
    {
        GameUIClass->setWindowTitle(QApplication::translate("GameUIClass", "GameEngine_With_DirectX9__FMOD__BulletPhysics__FBX SDK__QT", nullptr));
        start_pushButton_1->setText(QApplication::translate("GameUIClass", "Start", nullptr));
        end_pushButton_1->setText(QApplication::translate("GameUIClass", "End", nullptr));
        main_pushButton_2->setText(QApplication::translate("GameUIClass", "Exit", nullptr));
        main_pushButton_1->setText(QApplication::translate("GameUIClass", "Turn On Sound", nullptr));
        end_label->setText(QString());
        start_label->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class GameUIClass: public Ui_GameUIClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GAMEUI_H
