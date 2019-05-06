#ifndef GAMEUI_H
#define GAMEUI_H

#include <QtWidgets/QMainWindow>
#include "GeneratedFiles/ui_gameui.h"

#include <d3dx9.h>
#include <string>
#include <map>
#include <fstream>
#include <iostream>

using namespace std;


class GameObject;
class GamePlayManager;

class GameUI : public QMainWindow
{
	Q_OBJECT

public :
	enum TextFileType
	{
		TEXTFILE_NONE,
		TEXTFILE_TUTORIAL,
		TEXTFILE_ONE,
		TEXTFILE_BOSS
	};

	// 플레이어에게 이벤트가 발생했을때 처리하기 위해서 타입을 정해주었다.
	enum EventType
	{
		EVENT_NONE,
		// 튜토리얼 이벤트
		/*1*/EVENT_T_HAVETOMOVE,
		/*2*/EVENT_T_HAVETOSHOOT, 
		/*3*/EVENT_T_HAVETOBETRANSPARENT,
		/*4*/EVENT_T_HAVETODESTROYTARGET,
		/*5*/EVENT_T_HAVETODESTROYDUMMY,
		/*6*/EVENT_T_ENDEVENT
	};
private :
	Ui::GameUIClass ui;
	QPixmap m_img[3];
	float maxPlayerHp;
	float currentPlayerHp;
	float score;
	
	GameObject * objToAccessSystem;
	GamePlayManager * mainSceneGamePlayManager;

	bool currentMain_widget_UIWindowFlag;
	int countOfenemiesThatAreTargetingPlayer;

	static const string filePathToLoadImageFiles;


	// 파일스트림을 위한 변수들
	pair<TextFileType,int> currentTextFileAndPos;

	// 파일을 읽기 위한 TextFileType / textFileName 테이블
	map<TextFileType, string> fileNameTable;

	ifstream textFileStream;

	// 라인을 읽기위한 tag
	const int tagLength;
	const string lineTag;
	const string eventTag;
	const string endTag;

	const int numOfEvents;



public:

	friend class Locator;

	GameUI(QWidget *parent = 0);
	~GameUI();

	inline QPaintEngine* paintEngine()
	{
		return NULL;
	}

	void setUI();


	void resetUI()
	{

	}

	//GamePlayManager * getGamePlayManager();

	EventType readTextFile(TextFileType textFileType, bool isBeginning = false);

	void setTextWindowText(const char * info);

	float getCurrentPlayerHpFactor();
	void setCurrentPlayerHpFactor(float factor);

	void addScore(float value);
	void resetScore();

	void setMainSceneUI();
	void setStartSceneUI();
	void setEndSceneUI();

	void setBossImage(bool flag);

	void setActiveMainWidgetUIWindow(bool flag);	
	void toggleMainWidgetUIWindow();

	void setActiveMainWidgetTextWindow(bool flag);

	void setActiveStartWidgetInfoWindow(bool flag);
	

	void addEnemyThatIsTargetingPlayer();
	void removeEnemyThatIsNotTargetingPlayer();

	void showUI()
	{
		//setAutoFillBackground(true);
		//ui.main_line->setAutoFillBackground(true);
		//ui.main_pushButton_1->setAutoFillBackground(true);
		//ui.main_pushButton_2->setAutoFillBackground(true);
		//auto palette = ui.main_line->palette();
		//palette.setColor(QPalette::Window, QColor(100, 100, 0, 0));
		//ui.main_line->setPalette(palette);
		//ui.main_line->setStyleSheet("background - color: none");

		/*
		ui.main_line->setStyleSheet("color: green; background: rgba( 10, 0, 50, 0);");
		ui.main_line->setAutoFillBackground(true);

		ui.main_pushButton_1->setStyleSheet("color: green; background: black;");
		ui.main_pushButton_1->setAutoFillBackground(true);

		ui.main_pushButton_2->setStyleSheet("color: green; background: black;");
		ui.main_pushButton_2->setAutoFillBackground(true);
		*/


		//ui.main_line->update();
		//ui.main_pushButton_1->update();
		//ui.main_pushButton_2->update();
		//ui.widgetForDX->update();

			//auto editorPalette = palette();
			//editorPalette.setColor(QPalette::Active, QPalette::Base, Qt::transparent);
			//editorPalette.setColor(QPalette::Inactive, QPalette::Base, Qt::transparent);
			//setPalette(editorPalette);
		//ui.main_line->setStyleSheet("background:transparent");
		//ui.main_line->setAttribute(Qt::WA_TranslucentBackground);
		//ui.main_line->setWindowFlags(Qt::FramelessWindowHint);

		//ui.main_line->setPalette((QPalette::WindowText, Qt::transparent));
		//ui.main_line->setStyleSheet("background-color: rgba( 255, 255, 255, 0% );");
		//ui.main_line->palette().setColor()
		//ui.main_line->setAutoFillBackground(false);
		//setAutoFillBackground(false);



		//setAttribute(Qt::WA_NoSystemBackground);
		//setAttribute(Qt::WA_NoBackground);


		//ui.widgetForDX->update();
		//ui.main_pushButton_1->close();
		//ui.main_pushButton_2->show();
		//ui.main_line->show();

		//ui.start_graphicsView->setEnabled(false);
		//ui.start_pushButton_1->setEnabled(false);

		//ui.end_pushButton_1->setEnabled(true);
		//ui.end_graphicsView->setEnabled(true);
	}

	bool initD3D
	(int width, int height,
		bool windowed,
		D3DDEVTYPE deviceType,
		IDirect3DDevice9** device);

private slots:

void start_pushButton_start();
//void start_pushButton_load();
void start_pushButton_tutorial();

void end_pushButton_restart();
void checkBox_sound(bool value);
void checkBox_gizmos(bool value);

void checkBox_wireFrame(bool value);
void checkBox_fbxRendering(bool value);
void checkBox_playerRendering(bool value);
void checkBox_currentStage(bool value);

void pushButton_text();
void start_pushButton_info();
void start_pushButton_infoExit();



};

#endif // GAMEUI_H
