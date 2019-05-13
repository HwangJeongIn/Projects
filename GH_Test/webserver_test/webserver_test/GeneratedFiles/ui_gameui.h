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
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GameUIClass
{
public:
    QWidget *centralWidget;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QPushButton *pushButton_openWeb1;
    QPushButton *pushButton_getHeader;
    QPushButton *pushButton_post;
    QLabel *label;
    QWidget *widget_web;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *GameUIClass)
    {
        if (GameUIClass->objectName().isEmpty())
            GameUIClass->setObjectName(QString::fromUtf8("GameUIClass"));
        GameUIClass->resize(1997, 1112);
        centralWidget = new QWidget(GameUIClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        layoutWidget = new QWidget(centralWidget);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(1220, 20, 221, 421));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        pushButton_openWeb1 = new QPushButton(layoutWidget);
        pushButton_openWeb1->setObjectName(QString::fromUtf8("pushButton_openWeb1"));

        verticalLayout->addWidget(pushButton_openWeb1);

        pushButton_getHeader = new QPushButton(layoutWidget);
        pushButton_getHeader->setObjectName(QString::fromUtf8("pushButton_getHeader"));

        verticalLayout->addWidget(pushButton_getHeader);

        pushButton_post = new QPushButton(layoutWidget);
        pushButton_post->setObjectName(QString::fromUtf8("pushButton_post"));

        verticalLayout->addWidget(pushButton_post);

        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(570, 330, 671, 391));
        widget_web = new QWidget(centralWidget);
        widget_web->setObjectName(QString::fromUtf8("widget_web"));
        widget_web->setGeometry(QRect(30, 20, 1171, 991));
        GameUIClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(GameUIClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1997, 26));
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
        QObject::connect(pushButton_openWeb1, SIGNAL(clicked()), GameUIClass, SLOT(pushButton_openWeb1()));
        QObject::connect(pushButton_post, SIGNAL(clicked()), GameUIClass, SLOT(pushButton_post1()));
        QObject::connect(pushButton_getHeader, SIGNAL(clicked()), GameUIClass, SLOT(pushButton_getHeader1()));

        QMetaObject::connectSlotsByName(GameUIClass);
    } // setupUi

    void retranslateUi(QMainWindow *GameUIClass)
    {
        GameUIClass->setWindowTitle(QApplication::translate("GameUIClass", "Web_test", nullptr));
        pushButton_openWeb1->setText(QApplication::translate("GameUIClass", "Open Web // google.com", nullptr));
        pushButton_getHeader->setText(QApplication::translate("GameUIClass", "Get header // google.com", nullptr));
        pushButton_post->setText(QApplication::translate("GameUIClass", "Post", nullptr));
        label->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class GameUIClass: public Ui_GameUIClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GAMEUI_H
