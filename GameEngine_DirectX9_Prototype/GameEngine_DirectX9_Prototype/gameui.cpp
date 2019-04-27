#include "gameui.h"

const string GameUI::filePathToLoadImageFiles = "../Qt/";

GameUI::GameUI(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
}

GameUI::~GameUI()
{

}

void GameUI::slot2()
{
	ui.main_pushButton_1->setEnabled(true);
	ui.main_pushButton_2->setEnabled(false);
}

void GameUI::slot1()
{
	ui.main_pushButton_1->setEnabled(true);
	ui.main_pushButton_2->setEnabled(false);
}
