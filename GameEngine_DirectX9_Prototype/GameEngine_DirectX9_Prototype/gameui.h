#ifndef GAMEUI_H
#define GAMEUI_H

#include <QtWidgets/QMainWindow>
#include "GeneratedFiles/ui_gameui.h"

#include <d3dx9.h>
#include <string>

using namespace std;


class GameObject;

class GameUI : public QMainWindow
{
	Q_OBJECT

private :
	Ui::GameUIClass ui;
	QPixmap m_img[3];
	float maxPlayerHp;
	float currentPlayerHp;
	float score;
	
	GameObject * objToAccessSystem;


	static const string filePathToLoadImageFiles;

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

	float getCurrentPlayerHpFactor();
	void setCurrentPlayerHpFactor(float factor);

	void addScore(float value);

	void setMainSceneUI();
	void setStartSceneUI();
	void setEndSceneUI();

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
void start_pushButton_load();
void end_pushButton_restart();
void checkBox_sound(bool value);
void checkBox_gizmos(bool value);



};

#endif // GAMEUI_H
