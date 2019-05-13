#ifndef GAMEUI_H
#define GAMEUI_H

#include <QtWidgets/QMainWindow>
#include "GeneratedFiles/ui_gameui.h"

// À¥
#include <QtCore/qurl.h>
#include <QtGui/qdesktopservices.h>

//#include <QtWebView/QtWebViewDepends>
#include <QtWebView/qwebview_global.h>
#include <QtWebView/qtwebviewfunctions.h>
#include <QtWebView/qtwebviewversion.h>

//#include <QtWebView/5.12.3/QtWebView/private/qwebview_p.h>



#include <d3dx9.h>
#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include <curl/curl.h>

using namespace std;


class GameUI : public QMainWindow
{
	Q_OBJECT

public :

private :
	Ui::GameUIClass ui;


	CURL *curl;

public:

	friend class Locator;

	GameUI(QWidget *parent = 0);
	~GameUI();

	void setUI();

	inline QPaintEngine* paintEngine()
	{
		return NULL;
	}

private slots:

void pushButton_openWeb1();
void pushButton_openWeb2() {}
void pushButton_post1();
void pushButton_getHeader1();




};

#endif // GAMEUI_H
