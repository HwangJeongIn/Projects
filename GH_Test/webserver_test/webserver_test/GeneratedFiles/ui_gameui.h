/********************************************************************************
** Form generated from reading UI file 'gameui.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GAMEUI_H
#define UI_GAMEUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
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
    QPushButton *pushButton_html;
    QPushButton *pushButton_web;
    QWidget *widget_web;
    QLabel *label;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *GameUIClass)
    {
        if (GameUIClass->objectName().isEmpty())
            GameUIClass->setObjectName(QStringLiteral("GameUIClass"));
        GameUIClass->resize(1920, 1080);
        centralWidget = new QWidget(GameUIClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        layoutWidget = new QWidget(centralWidget);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(1320, 20, 201, 381));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        pushButton_openWeb1 = new QPushButton(layoutWidget);
        pushButton_openWeb1->setObjectName(QStringLiteral("pushButton_openWeb1"));

        verticalLayout->addWidget(pushButton_openWeb1);

        pushButton_getHeader = new QPushButton(layoutWidget);
        pushButton_getHeader->setObjectName(QStringLiteral("pushButton_getHeader"));

        verticalLayout->addWidget(pushButton_getHeader);

        pushButton_post = new QPushButton(layoutWidget);
        pushButton_post->setObjectName(QStringLiteral("pushButton_post"));

        verticalLayout->addWidget(pushButton_post);

        pushButton_html = new QPushButton(layoutWidget);
        pushButton_html->setObjectName(QStringLiteral("pushButton_html"));

        verticalLayout->addWidget(pushButton_html);

        pushButton_web = new QPushButton(layoutWidget);
        pushButton_web->setObjectName(QStringLiteral("pushButton_web"));

        verticalLayout->addWidget(pushButton_web);

        widget_web = new QWidget(centralWidget);
        widget_web->setObjectName(QStringLiteral("widget_web"));
        widget_web->setGeometry(QRect(20, 20, 1280, 720));
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(480, 800, 731, 171));
        GameUIClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(GameUIClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1920, 26));
        GameUIClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(GameUIClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        GameUIClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(GameUIClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        GameUIClass->setStatusBar(statusBar);

        retranslateUi(GameUIClass);
        QObject::connect(pushButton_getHeader, SIGNAL(clicked()), GameUIClass, SLOT(pushButton_getHeader1()));
        QObject::connect(pushButton_openWeb1, SIGNAL(clicked()), GameUIClass, SLOT(pushButton_openWeb1()));
        QObject::connect(pushButton_post, SIGNAL(clicked()), GameUIClass, SLOT(pushButton_post1()));
        QObject::connect(pushButton_html, SIGNAL(clicked()), GameUIClass, SLOT(html()));
        QObject::connect(pushButton_web, SIGNAL(clicked()), GameUIClass, SLOT(web()));

        QMetaObject::connectSlotsByName(GameUIClass);
    } // setupUi

    void retranslateUi(QMainWindow *GameUIClass)
    {
        GameUIClass->setWindowTitle(QApplication::translate("GameUIClass", "GameUI", Q_NULLPTR));
        pushButton_openWeb1->setText(QApplication::translate("GameUIClass", "open_web", Q_NULLPTR));
        pushButton_getHeader->setText(QApplication::translate("GameUIClass", "get_header", Q_NULLPTR));
        pushButton_post->setText(QApplication::translate("GameUIClass", "post", Q_NULLPTR));
        pushButton_html->setText(QApplication::translate("GameUIClass", "html", Q_NULLPTR));
        pushButton_web->setText(QApplication::translate("GameUIClass", "web", Q_NULLPTR));
        label->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class GameUIClass: public Ui_GameUIClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GAMEUI_H
