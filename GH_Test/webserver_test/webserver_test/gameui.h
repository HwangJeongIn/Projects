#ifndef GAMEUI_H
#define GAMEUI_H

#include <QtWidgets/QMainWindow>
#include "GeneratedFiles\ui_gameui.h"

// À¥
#include <QtCore/qurl.h>
#include <QtGui/qdesktopservices.h>

//#include <QtWebView/QtWebViewDepends>
#include <QtWebView/qwebview_global.h>
#include <QtWebView/qtwebviewfunctions.h>
#include <QtWebView/qtwebviewversion.h>

// À§Á¬ // kit
#include <QtWebEngineWidgets\qwebengineview.h>
#include <QtWebEngineWidgets\qwebenginehistory.h>
#include <QtWebEngineWidgets\qwebenginepage.h>
//#include <QtWebView/5.12.3/QtWebView/private/qwebview_p.h>



#include <d3dx9.h>
#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <curl/curl.h>

using namespace std;


class GameUI : public QMainWindow
{
	Q_OBJECT

public:
	GameUI(QWidget *parent = 0);
	~GameUI();

	void setUI();

	inline QPaintEngine* paintEngine()
	{
		return NULL;
	}

	void openLocalHtml(const char * htmlName);
	void openUrl(QUrl & url);

private:
	Ui::GameUIClass ui;
	CURL *curl;
	QWebEngineView *webView;
	string filePathToLoadHtml;

private slots:

	void pushButton_openWeb1();
	void pushButton_openWeb2() {}
	void pushButton_post1();
	void pushButton_getHeader1();
	void web();
	void html();
	//void pushButton_openLocalHtmlW1();
	//void pushButton_openWebW1();


};

#endif // GAMEUI_H
