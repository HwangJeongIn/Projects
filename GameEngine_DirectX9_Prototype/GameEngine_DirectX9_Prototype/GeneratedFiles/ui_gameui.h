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
    QPushButton *start_pushButton_tutorial;
    QPushButton *end_pushButton_restart;
    QWidget *main_widget_UIWindow;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QCheckBox *checkBox_sound;
    QCheckBox *checkBox_gizmos;
    QCheckBox *checkBox_wireFrame;
    QCheckBox *checkBox_playerRendering;
    QCheckBox *checkBox_currentStage;
    QCheckBox *checkBox_fbxRendering;
    QWidget *main_widget_textWindow;
    QPushButton *pushButton_text;
    QLabel *label_text;
    QPushButton *start_pushButton_info;
    QWidget *start_widget_infoWindow;
    QLabel *start_label_info;
    QPushButton *start_pushButton_infoExit;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *GameUIClass)
    {
        if (GameUIClass->objectName().isEmpty())
            GameUIClass->setObjectName(QString::fromUtf8("GameUIClass"));
        GameUIClass->resize(1920, 1112);
        centralWidget = new QWidget(GameUIClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        lineEdit_1 = new QLineEdit(centralWidget);
        lineEdit_1->setObjectName(QString::fromUtf8("lineEdit_1"));
        lineEdit_1->setGeometry(QRect(1910, 0, 21, 1081));
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
        main_label_score->setGeometry(QRect(10, 0, 251, 51));
        main_label_score->setStyleSheet(QString::fromUtf8("color: rgb(255, 129, 3);\n"
"font: 75 20pt \"\353\247\221\354\235\200 \352\263\240\353\224\225\";\n"
"font-weight: bold;"));
        main_label_bossImage = new QLabel(centralWidget);
        main_label_bossImage->setObjectName(QString::fromUtf8("main_label_bossImage"));
        main_label_bossImage->setGeometry(QRect(1760, 920, 150, 150));
        start_pushButton_start = new QPushButton(centralWidget);
        start_pushButton_start->setObjectName(QString::fromUtf8("start_pushButton_start"));
        start_pushButton_start->setGeometry(QRect(790, 460, 349, 115));
        start_pushButton_start->setStyleSheet(QString::fromUtf8("color: rgb(255, 129, 3);\n"
"font: 75 48pt \"\353\247\221\354\235\200 \352\263\240\353\224\225\";"));
        start_pushButton_start->setFlat(true);
        start_pushButton_tutorial = new QPushButton(centralWidget);
        start_pushButton_tutorial->setObjectName(QString::fromUtf8("start_pushButton_tutorial"));
        start_pushButton_tutorial->setGeometry(QRect(790, 600, 349, 115));
        start_pushButton_tutorial->setStyleSheet(QString::fromUtf8("color: rgb(255, 129, 3);\n"
"font: 75 48pt \"\353\247\221\354\235\200 \352\263\240\353\224\225\";"));
        start_pushButton_tutorial->setFlat(true);
        end_pushButton_restart = new QPushButton(centralWidget);
        end_pushButton_restart->setObjectName(QString::fromUtf8("end_pushButton_restart"));
        end_pushButton_restart->setGeometry(QRect(790, 530, 349, 115));
        end_pushButton_restart->setStyleSheet(QString::fromUtf8("color: rgb(255, 129, 3);\n"
"font: 75 48pt \"\353\247\221\354\235\200 \352\263\240\353\224\225\";"));
        end_pushButton_restart->setFlat(true);
        main_widget_UIWindow = new QWidget(centralWidget);
        main_widget_UIWindow->setObjectName(QString::fromUtf8("main_widget_UIWindow"));
        main_widget_UIWindow->setGeometry(QRect(1620, 0, 281, 521));
        layoutWidget = new QWidget(main_widget_UIWindow);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 10, 267, 501));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        checkBox_sound = new QCheckBox(layoutWidget);
        checkBox_sound->setObjectName(QString::fromUtf8("checkBox_sound"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(checkBox_sound->sizePolicy().hasHeightForWidth());
        checkBox_sound->setSizePolicy(sizePolicy);
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

        checkBox_wireFrame = new QCheckBox(layoutWidget);
        checkBox_wireFrame->setObjectName(QString::fromUtf8("checkBox_wireFrame"));
        checkBox_wireFrame->setLayoutDirection(Qt::LeftToRight);
        checkBox_wireFrame->setStyleSheet(QString::fromUtf8("font: 13pt \"\353\247\221\354\235\200 \352\263\240\353\224\225\";"));
        checkBox_wireFrame->setChecked(false);

        verticalLayout->addWidget(checkBox_wireFrame);

        checkBox_playerRendering = new QCheckBox(layoutWidget);
        checkBox_playerRendering->setObjectName(QString::fromUtf8("checkBox_playerRendering"));
        checkBox_playerRendering->setLayoutDirection(Qt::LeftToRight);
        checkBox_playerRendering->setStyleSheet(QString::fromUtf8("font: 13pt \"\353\247\221\354\235\200 \352\263\240\353\224\225\";"));
        checkBox_playerRendering->setChecked(true);

        verticalLayout->addWidget(checkBox_playerRendering);

        checkBox_currentStage = new QCheckBox(layoutWidget);
        checkBox_currentStage->setObjectName(QString::fromUtf8("checkBox_currentStage"));
        checkBox_currentStage->setLayoutDirection(Qt::LeftToRight);
        checkBox_currentStage->setStyleSheet(QString::fromUtf8("font: 13pt \"\353\247\221\354\235\200 \352\263\240\353\224\225\";"));
        checkBox_currentStage->setChecked(true);

        verticalLayout->addWidget(checkBox_currentStage);

        checkBox_fbxRendering = new QCheckBox(layoutWidget);
        checkBox_fbxRendering->setObjectName(QString::fromUtf8("checkBox_fbxRendering"));
        checkBox_fbxRendering->setLayoutDirection(Qt::LeftToRight);
        checkBox_fbxRendering->setStyleSheet(QString::fromUtf8("font: 13pt \"\353\247\221\354\235\200 \352\263\240\353\224\225\";"));
        checkBox_fbxRendering->setChecked(true);

        verticalLayout->addWidget(checkBox_fbxRendering);

        main_widget_textWindow = new QWidget(centralWidget);
        main_widget_textWindow->setObjectName(QString::fromUtf8("main_widget_textWindow"));
        main_widget_textWindow->setGeometry(QRect(490, 40, 991, 111));
        pushButton_text = new QPushButton(main_widget_textWindow);
        pushButton_text->setObjectName(QString::fromUtf8("pushButton_text"));
        pushButton_text->setGeometry(QRect(940, 10, 41, 91));
        pushButton_text->setStyleSheet(QString::fromUtf8("font: 87 20pt \"Arial Black\";"));
        label_text = new QLabel(main_widget_textWindow);
        label_text->setObjectName(QString::fromUtf8("label_text"));
        label_text->setGeometry(QRect(20, 20, 901, 71));
        label_text->setStyleSheet(QString::fromUtf8("font: 75 18pt \"\353\247\221\354\235\200 \352\263\240\353\224\225\";\n"
"font-weight: bold;"));
        start_pushButton_info = new QPushButton(centralWidget);
        start_pushButton_info->setObjectName(QString::fromUtf8("start_pushButton_info"));
        start_pushButton_info->setGeometry(QRect(790, 730, 349, 115));
        start_pushButton_info->setStyleSheet(QString::fromUtf8("color: rgb(255, 129, 3);\n"
"font: 75 48pt \"\353\247\221\354\235\200 \352\263\240\353\224\225\";"));
        start_pushButton_info->setFlat(true);
        start_widget_infoWindow = new QWidget(centralWidget);
        start_widget_infoWindow->setObjectName(QString::fromUtf8("start_widget_infoWindow"));
        start_widget_infoWindow->setGeometry(QRect(700, 340, 531, 641));
        start_label_info = new QLabel(start_widget_infoWindow);
        start_label_info->setObjectName(QString::fromUtf8("start_label_info"));
        start_label_info->setGeometry(QRect(0, 0, 531, 641));
        start_label_info->setMaximumSize(QSize(731, 16777215));
        start_label_info->setStyleSheet(QString::fromUtf8("font: 75 13.5pt \"\353\247\221\354\235\200 \352\263\240\353\224\225\";\n"
"background-color: rgb(180, 180, 180);\n"
"font-weight: bold;"));
        start_pushButton_infoExit = new QPushButton(start_widget_infoWindow);
        start_pushButton_infoExit->setObjectName(QString::fromUtf8("start_pushButton_infoExit"));
        start_pushButton_infoExit->setGeometry(QRect(462, 10, 61, 61));
        GameUIClass->setCentralWidget(centralWidget);
        labelForDX->raise();
        start_label->raise();
        end_label->raise();
        lineEdit_1->raise();
        main_line_hp->raise();
        lineEdit_2->raise();
        lineEdit_3->raise();
        lineEdit_4->raise();
        main_label_score->raise();
        startEnd_label_title->raise();
        start_pushButton_start->raise();
        start_pushButton_tutorial->raise();
        end_pushButton_restart->raise();
        main_widget_UIWindow->raise();
        main_label_bossImage->raise();
        main_widget_textWindow->raise();
        start_pushButton_info->raise();
        start_widget_infoWindow->raise();
        menuBar = new QMenuBar(GameUIClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1920, 26));
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
        QObject::connect(start_pushButton_tutorial, SIGNAL(clicked()), GameUIClass, SLOT(start_pushButton_tutorial()));
        QObject::connect(end_pushButton_restart, SIGNAL(clicked()), GameUIClass, SLOT(end_pushButton_restart()));
        QObject::connect(checkBox_sound, SIGNAL(toggled(bool)), GameUIClass, SLOT(checkBox_sound(bool)));
        QObject::connect(checkBox_gizmos, SIGNAL(toggled(bool)), GameUIClass, SLOT(checkBox_gizmos(bool)));
        QObject::connect(checkBox_wireFrame, SIGNAL(toggled(bool)), GameUIClass, SLOT(checkBox_wireFrame(bool)));
        QObject::connect(checkBox_fbxRendering, SIGNAL(toggled(bool)), GameUIClass, SLOT(checkBox_fbxRendering(bool)));
        QObject::connect(checkBox_playerRendering, SIGNAL(toggled(bool)), GameUIClass, SLOT(checkBox_playerRendering(bool)));
        QObject::connect(checkBox_currentStage, SIGNAL(toggled(bool)), GameUIClass, SLOT(checkBox_currentStage(bool)));
        QObject::connect(pushButton_text, SIGNAL(clicked()), GameUIClass, SLOT(pushButton_text()));
        QObject::connect(start_pushButton_info, SIGNAL(clicked()), GameUIClass, SLOT(start_pushButton_info()));
        QObject::connect(start_pushButton_infoExit, SIGNAL(clicked()), GameUIClass, SLOT(start_pushButton_infoExit()));

        QMetaObject::connectSlotsByName(GameUIClass);
    } // setupUi

    void retranslateUi(QMainWindow *GameUIClass)
    {
        GameUIClass->setWindowTitle(QApplication::translate("GameUIClass", "GameEngine_With_DirectX9__FMOD__BulletPhysics__FBX SDK__QT", nullptr));
        start_label->setText(QString());
        end_label->setText(QString());
        labelForDX->setText(QString());
        startEnd_label_title->setText(QApplication::translate("GameUIClass", "<html><head/><body><p>FOREST</p></body></html>", nullptr));
        main_label_score->setText(QApplication::translate("GameUIClass", "<html><head/><body><p><span style=\" font-weight:600;\">SCORE : </span></p></body></html>", nullptr));
        main_label_bossImage->setText(QString());
        start_pushButton_start->setText(QApplication::translate("GameUIClass", "Start", nullptr));
        start_pushButton_tutorial->setText(QApplication::translate("GameUIClass", "Tutorial", nullptr));
        end_pushButton_restart->setText(QApplication::translate("GameUIClass", "Restart", nullptr));
        checkBox_sound->setText(QApplication::translate("GameUIClass", "Toggle Sound", nullptr));
        checkBox_gizmos->setText(QApplication::translate("GameUIClass", "Toggle Gizmos", nullptr));
        checkBox_wireFrame->setText(QApplication::translate("GameUIClass", "Toggle WireFrame", nullptr));
        checkBox_playerRendering->setText(QApplication::translate("GameUIClass", "Toggle PlayerRendering", nullptr));
        checkBox_currentStage->setText(QApplication::translate("GameUIClass", "Toggle CurrentStage", nullptr));
        checkBox_fbxRendering->setText(QApplication::translate("GameUIClass", "Toggle FbxRendering", nullptr));
        pushButton_text->setText(QApplication::translate("GameUIClass", "X", nullptr));
#ifndef QT_NO_TOOLTIP
        label_text->setToolTip(QApplication::translate("GameUIClass", "<html><head/><body><p align=\"center\"><br/></p></body></html>", nullptr));
#endif // QT_NO_TOOLTIP
        label_text->setText(QApplication::translate("GameUIClass", "test", nullptr));
        start_pushButton_info->setText(QApplication::translate("GameUIClass", "Info", nullptr));
#ifndef QT_NO_WHATSTHIS
        start_label_info->setWhatsThis(QApplication::translate("GameUIClass", "<html><head/><body><p align=\"center\"><br/></p></body></html>", nullptr));
#endif // QT_NO_WHATSTHIS
        start_label_info->setText(QApplication::translate("GameUIClass", "<html><head/><body><p align=\"center\"><span style=\" font-size:16pt;\">\355\201\264\353\235\274\354\235\264\354\226\270\355\212\270</span></p><p align=\"center\">\355\231\251\354\240\225\354\235\270<span style=\" font-size:16pt;\"><br/></span></p><p align=\"center\"><span style=\" font-size:16pt;\">SDK</span></p><p align=\"center\">DIRECTX9 SDK / FBX SDK</p><p align=\"center\">QT SDK / FMOD SDK</p><p align=\"center\">BULLET PHYSICS SDK</p><p align=\"center\"><br/></p><p align=\"center\"><span style=\" font-size:16pt;\">\353\246\254\354\206\214\354\212\244</span></p><p align=\"center\">Unity AssetStore / Mixamo</p><p align=\"center\">BlackDesert BGM</p><p align=\"center\"><br/></p><p align=\"center\"><span style=\" font-size:11pt;\">** \354\242\213\354\235\200 \353\260\251\355\226\245\354\234\274\353\241\234 \354\247\200\353\217\204\355\225\264\354\243\274\354\213\240 \352\265\220\354\210\230\353\213\230\353\223\244 \354\240\225\353\247\220 \352\260\220\354\202\254\353\223\234\353\246\275\353\213\210\353\213\244"
                        ". **</span></p></body></html>", nullptr));
        start_pushButton_infoExit->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class GameUIClass: public Ui_GameUIClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GAMEUI_H
