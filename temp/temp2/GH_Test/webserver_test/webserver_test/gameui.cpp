#define CURL_STATICLIB

#include <stdio.h>

#include <iostream>
#include <windows.h>
#include <string>
#include "gameui.h"


GameUI::GameUI(QWidget *parent)
	: QMainWindow(parent), curl(nullptr)
{
	ui.setupUi(this);

	//ui.label->setText("")
	

	webView = new QWebEngineView(ui.widget_web);
	webView->resize(QSize(webView->parentWidget()->size()));


	//----------------------------------------------------------------------------------------------
	WCHAR tempPath[512];
	GetModuleFileName(NULL, tempPath, 512);

	wstring tempFilePathToLoadHtml = tempPath;

	int count = 0;
	int i = tempFilePathToLoadHtml.size();
	for (; i >= 0; --i)
	{
		if (tempFilePathToLoadHtml[i] == '\\') ++count;

		// 64비트로 빌드시 하나더 폴더를 올라가야한다 // 3개
		// 32비트로 빌드시 폴더 2개만 더 올라가면 된다.
		if (count == 3) break;

	}

	// 2개의 역슬래쉬를 만나지 못했다. // 오류 발생
	if (i == -1)
		return;

	// 2개의 역슬래쉬를 만났다.
	tempFilePathToLoadHtml = tempFilePathToLoadHtml.substr(0, i + 1);
	//filePathToLoadHtml += filePathToLoadHtml + L'\\';
	tempFilePathToLoadHtml += L"Web\\";
	char finalURLPath[512];
	sprintf(finalURLPath, "%ws", tempFilePathToLoadHtml.c_str());

	// 최종적으로 결정
	filePathToLoadHtml = finalURLPath;

	//qDebug() << (QUrl::fromLocalFile(filePathToLoadHtml.c_str()));


	//----------------------------------------------------------------------------------------------


	//webView->resize(ui.widget_web->size());
	//":/Images/Help.html"
	// 루트디렉터리에서 찾기


	openUrl(QUrl("http://google.com/"));


	//webView->load(QUrl("file:///C:/Bitnami/wampstack-7.1.29-0/apache2/htdocs/index.html"));
	//webView->load(QUrl::fromLocalFile("/C:/Bitnami/wampstack-7.1.29-0/apache2/htdocs/index.html"));
	//webView->load(QUrl::fromLocalFile("/Web/index.html")); // 프로젝트 디렉터리 기준으로 찾음






	//qDebug() << QUrl::fromLocalFile("C:/Bitnami/wampstack-7.1.29-0/apache2/htdocs/index.html");
	//qDebug() << (QUrl::fromLocalFile("\\Web\\index.html"));

	//webView->load(QUrl("qrc: ///Web/index.html"));
	//webView->load(QUrl("http://google.com/"));
	//webView->setHtml(QString("C:/Bitnami/wampstack-7.1.29-0/apache2/htdocs/index.html"));// , QUrl("http://127.0.0.1:8080/index.html"));

	//view->setVisible(true);

	webView->show();

}

GameUI::~GameUI()
{
	//QDesktopSer
}

void GameUI::setUI()
{
	//label

	//ui.main_line->setStyleSheet("color: green; background: rgba( 10, 0, 50, 0);");
	//ui.main_line->setAutoFillBackground(true);

	//ui.label->setStyleSheet("color: green; background: none;");
	//ui.label->setAutoFillBackground(true);

	//ui.lineEdit_1->setVisible(false);
	//ui.lineEdit_2->setVisible(false);
	//ui.lineEdit_3->setVisible(false);
	//ui.lineEdit_4->setVisible(false);
}

void GameUI::openLocalHtml(const char * htmlName)
{
	if (!htmlName) return;

	string finalURLPath = filePathToLoadHtml + htmlName;

	webView->load(QUrl::fromLocalFile(finalURLPath.c_str()));
	webView->show();
}

void GameUI::openUrl(QUrl & url)
{
	webView->load(url);
	webView->show();
}

void GameUI::pushButton_post1()
{
	//webView->page()->triggerAction(QWebEnginePage::Stop);
	//webView->stop();
		CURLcode res;
		curl = curl_easy_init();
		if (curl) 
		{
			//https://google.com
			curl_easy_setopt(curl, CURLOPT_URL, "localhost:8080/test.php?userid=bseo&passwd=1234");
			/* example.com is redirected, so we tell libcurl to follow redirection */
			//curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
			//curl_easy_setopt(curl, CURLOPT_HEADER, 1L);
			curl_easy_setopt(curl, CURLOPT_POST, 1L);
			/* Perform the request, res will get the return code */
			res = curl_easy_perform(curl);
			/* Check for errors */
			if (res != CURLE_OK)
				fprintf(stderr, "curl_easy_perform() failed: %s\n",
					curl_easy_strerror(res));
	
			/* always cleanup */
			curl_easy_cleanup(curl);
		}
}

void GameUI::pushButton_getHeader1()
{

	CURLcode res;
	curl = curl_easy_init();
	if (curl) {
		//https://google.com
		curl_easy_setopt(curl, CURLOPT_URL, "google.com");
		/* example.com is redirected, so we tell libcurl to follow redirection */
		//curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_HEADER, 1L);
		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		/* Check for errors */
		if (res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));

		/* always cleanup */
		curl_easy_cleanup(curl);
	}

}

void GameUI::web()
{
	openUrl(QUrl("http://google.com/"));
}

void GameUI::html()
{
	openLocalHtml("index.html");
}

//void GameUI::pushButton_openLocalHtmlW1()
//{
//	openLocalHtml("index.html");
//}
//
//void GameUI::pushButton_openWebW1()
//{
//	openUrl(QUrl("http://google.com/"));
//}


void GameUI::pushButton_openWeb1()
{

	//QString link = "www.google.com";
	//int handle =  (HWND)ui.centralWidget->winId;
	//ui.textBrowser->loadResource(ui.textBrowser->winId(), QUrl("https://google.com"));
	QString link = "www.google.com";
	QDesktopServices::openUrl(QUrl(link));



}





