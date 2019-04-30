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
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
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
    QLineEdit *lineEdit_1;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QCheckBox *checkBox_sound;
    QCheckBox *checkBox_gizmos;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QFrame *main_line_hp;
    QLabel *start_label;
    QLabel *end_label;
    QLineEdit *lineEdit_2;
    QLineEdit *lineEdit_3;
    QLineEdit *lineEdit_4;
    QLabel *labelForDX;
    QLabel *startEnd_label_title;
    QLabel *main_label_score;
    QLabel *main_label_bossImage;
    QPushButton *start_pushButton_start;
    QPushButton *start_pushButton_load;
    QPushButton *end_pushButton_restart;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *GameUIClass)
    {
        if (GameUIClass->objectName().isEmpty())
            GameUIClass->setObjectName(QString::fromUtf8("GameUIClass"));
        GameUIClass->resize(2127, 1222);
        centralWidget = new QWidget(GameUIClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        lineEdit_1 = new QLineEdit(centralWidget);
        lineEdit_1->setObjectName(QString::fromUtf8("lineEdit_1"));
        lineEdit_1->setGeometry(QRect(1910, 0, 21, 1081));
        layoutWidget = new QWidget(centralWidget);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(1930, 0, 191, 531));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        checkBox_sound = new QCheckBox(layoutWidget);
        checkBox_sound->setObjectName(QString::fromUtf8("checkBox_sound"));
        checkBox_sound->setLayoutDirection(Qt::LeftToRight);
        checkBox_sound->setStyleSheet(QString::fromUtf8("font: 13pt \"\353\247\221\354\235\200 \352\263\240\353\224\225\";"));
        checkBox_sound->setChecked(true);

        verticalLayout->addWidget(checkBox_sound);

        checkBox_gizmos = new QCheckBox(layoutWidget);
        checkBox_gizmos->setObjectName(QString::fromUtf8("checkBox_gizmos"));
        checkBox_gizmos->setLayoutDirection(Qt::LeftToRight);
        checkBox_gizmos->setStyleSheet(QString::fromUtf8("font: 13pt \"\353\247\221\354\235\200 \352\263\240\353\224\225\";"));
        checkBox_gizmos->setChecked(true);

        verticalLayout->addWidget(checkBox_gizmos);

        pushButton = new QPushButton(layoutWidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        verticalLayout->addWidget(pushButton);

        pushButton_2 = new QPushButton(layoutWidget);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));

        verticalLayout->addWidget(pushButton_2);

        pushButton_3 = new QPushButton(layoutWidget);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));

        verticalLayout->addWidget(pushButton_3);

        main_line_hp = new QFrame(centralWidget);
        main_line_hp->setObjectName(QString::fromUtf8("main_line_hp"));
        main_line_hp->setGeometry(QRect(460, 1000, 1000, 15));
        main_line_hp->setStyleSheet(QString::fromUtf8("color: rgb(225, 0, 0);"));
        main_line_hp->setFrameShadow(QFrame::Plain);
        main_line_hp->setLineWidth(20);
        main_line_hp->setMidLineWidth(10);
        main_line_hp->setFrameShape(QFrame::HLine);
        start_label = new QLabel(centralWidget);
        start_label->setObjectName(QString::fromUtf8("start_label"));
        start_label->setGeometry(QRect(0, 0, 1920, 1080));
        start_label->setStyleSheet(QString::fromUtf8(""));
        end_label = new QLabel(centralWidget);
        end_label->setObjectName(QString::fromUtf8("end_label"));
        end_label->setGeometry(QRect(0, 0, 1920, 1080));
        end_label->setStyleSheet(QString::fromUtf8(""));
        lineEdit_2 = new QLineEdit(centralWidget);
        lineEdit_2->setObjectName(QString::fromUtf8("lineEdit_2"));
        lineEdit_2->setGeometry(QRect(0, 1070, 1931, 21));
        lineEdit_3 = new QLineEdit(centralWidget);
        lineEdit_3->setObjectName(QString::fromUtf8("lineEdit_3"));
        lineEdit_3->setGeometry(QRect(950, 0, 20, 1091));
        lineEdit_4 = new QLineEdit(centralWidget);
        lineEdit_4->setObjectName(QString::fromUtf8("lineEdit_4"));
        lineEdit_4->setGeometry(QRect(0, 530, 1931, 21));
        labelForDX = new QLabel(centralWidget);
        labelForDX->setObjectName(QString::fromUtf8("labelForDX"));
        labelForDX->setGeometry(QRect(0, 0, 1920, 1080));
        startEnd_label_title = new QLabel(centralWidget);
        startEnd_label_title->setObjectName(QString::fromUtf8("startEnd_label_title"));
        startEnd_label_title->setGeometry(QRect(510, 150, 901, 191));
        QFont font;
        font.setFamily(QString::fromUtf8("\353\247\221\354\235\200 \352\263\240\353\224\225"));
        font.setPointSize(90);
        font.setBold(false);
        font.setItalic(false);
        font.setWeight(50);
        startEnd_label_title->setFont(font);
        startEnd_label_title->setStyleSheet(QString::fromUtf8("color : rgb(255, 177, 21);\n"
"font: 90pt \"\353\247\221\354\235\200 \352\263\240\353\224\225\";"));
        startEnd_label_title->setFrameShape(QFrame::NoFrame);
        startEnd_label_title->setFrameShadow(QFrame::Plain);
        startEnd_label_title->setScaledContents(false);
        startEnd_label_title->setAlignment(Qt::AlignCenter);
        main_label_score = new QLabel(centralWidget);
        main_label_score->setObjectName(QString::fromUtf8("main_label_score"));
        main_label_score->setGeometry(QRect(10, 0, 631, 71));
        main_label_score->setStyleSheet(QString::fromUtf8("color: rgb(255, 129, 3);\n"
"font: 75 20pt \"\353\247\221\354\235\200 \352\263\240\353\224\225\";"));
        main_label_bossImage = new QLabel(centralWidget);
        main_label_bossImage->setObjectName(QString::fromUtf8("main_label_bossImage"));
        main_label_bossImage->setGeometry(QRect(1810, 0, 100, 100));
        start_pushButton_start = new QPushButton(centralWidget);
        start_pushButton_start->setObjectName(QString::fromUtf8("start_pushButton_start"));
        start_pushButton_start->setGeometry(QRect(790, 460, 349, 115));
        start_pushButton_start->setStyleSheet(QString::fromUtf8("color: rgb(255, 129, 3);\n"
"font: 75 48pt \"\353\247\221\354\235\200 \352\263\240\353\224\225\";"));
        start_pushButton_start->setFlat(true);
        start_pushButton_load = new QPushButton(centralWidget);
        start_pushButton_load->setObjectName(QString::fromUtf8("start_pushButton_load"));
        start_pushButton_load->setGeometry(QRect(790, 600, 349, 115));
        start_pushButton_load->setStyleSheet(QString::fromUtf8("color: rgb(255, 129, 3);\n"
"font: 75 48pt \"\353\247\221\354\235\200 \352\263\240\353\224\225\";"));
        start_pushButton_load->setFlat(true);
        end_pushButton_restart = new QPushButton(centralWidget);
        end_pushButton_restart->setObjectName(QString::fromUtf8("end_pushButton_restart"));
        end_pushButton_restart->setGeometry(QRect(790, 530, 349, 115));
        end_pushButton_restart->setStyleSheet(QString::fromUtf8("color: rgb(255, 129, 3);\n"
"font: 75 48pt \"\353\247\221\354\235\200 \352\263\240\353\224\225\";"));
        end_pushButton_restart->setFlat(true);
        GameUIClass->setCentralWidget(centralWidget);
        labelForDX->raise();
        start_label->raise();
        end_label->raise();
        lineEdit_1->raise();
        layoutWidget->raise();
        main_line_hp->raise();
        lineEdit_2->raise();
        lineEdit_3->raise();
        lineEdit_4->raise();
        main_label_score->raise();
        main_label_bossImage->raise();
        startEnd_label_title->raise();
        start_pushButton_start->raise();
        start_pushButton_load->raise();
        end_pushButton_restart->raise();
        menuBar = new QMenuBar(GameUIClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 2127, 26));
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
        QObject::connect(start_pushButton_start, SIGNAL(clicked()), GameUIClass, SLOT(start_pushButton_start()));
        QObject::connect(start_pushButton_load, SIGNAL(clicked()), GameUIClass, SLOT(start_pushButton_load()));
        QObject::connect(end_pushButton_restart, SIGNAL(clicked()), GameUIClass, SLOT(end_pushButton_restart()));
        QObject::connect(checkBox_sound, SIGNAL(toggled(bool)), GameUIClass, SLOT(checkBox_sound(bool)));
        QObject::connect(checkBox_gizmos, SIGNAL(toggled(bool)), GameUIClass, SLOT(checkBox_gizmos(bool)));

        QMetaObject::connectSlotsByName(GameUIClass);
    } // setupUi

    void retranslateUi(QMainWindow *GameUIClass)
    {
        GameUIClass->setWindowTitle(QApplication::translate("GameUIClass", "GameEngine_With_DirectX9__FMOD__BulletPhysics__FBX SDK__QT", nullptr));
        checkBox_sound->setText(QApplication::translate("GameUIClass", "Toggle Sound", nullptr));
        checkBox_gizmos->setText(QApplication::translate("GameUIClass", "Toggle Gizmos", nullptr));
        pushButton->setText(QApplication::translate("GameUIClass", "PushButton", nullptr));
        pushButton_2->setText(QApplication::translate("GameUIClass", "PushButton", nullptr));
        pushButton_3->setText(QApplication::translate("GameUIClass", "PushButton", nullptr));
        start_label->setText(QString());
        end_label->setText(QString());
        labelForDX->setText(QString());
        startEnd_label_title->setText(QApplication::translate("GameUIClass", "<html><head/><body><p>FOREST</p></body></html>", nullptr));
        main_label_score->setText(QApplication::translate("GameUIClass", "<html><head/><body><p><span style=\" font-weight:600;\">SCORE : </span></p></body></html>", nullptr));
        main_label_bossImage->setText(QString());
        start_pushButton_start->setText(QApplication::translate("GameUIClass", "Start", nullptr));
        start_pushButton_load->setText(QApplication::translate("GameUIClass", "Load", nullptr));
        end_pushButton_restart->setText(QApplication::translate("GameUIClass", "Restart", nullptr));
    } // retranslateUi

};

namespace Ui {
    class GameUIClass: public Ui_GameUIClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GAMEUI_H
