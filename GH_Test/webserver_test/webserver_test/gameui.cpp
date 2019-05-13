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

	

	//QWebView *view = new QWebView(ui.widget_web);
	//view->loadHtml(QString("http://google.com/"));
	//view->setVisible(true);
	//view->load(QUrl("http://google.com/"));
	//view->show();

	// 파일을 읽기 위한 TextFileType / textFileName 테이블

	//// 하드코딩으로 등록
	//fileNameTable.insert(pair<TextFileType, string>(TextFileType::TEXTFILE_TUTORIAL, "_TutorialStage.txt"));
	//fileNameTable.insert(pair<TextFileType, string>(TextFileType::TEXTFILE_ONE, "_OneStage.txt"));
	//fileNameTable.insert(pair<TextFileType, string>(TextFileType::TEXTFILE_BOSS, "_BossStage.txt"));


	// 스타일 시트 정하기
	//ui.main_label_score->setStyleSheet("color: rgb(255, 129, 3); font-weight: bold; ");
	//ui.label_text->setStyleSheet("font-weight: bold; font: 18pt \"맑은 고딕\"; ");
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

void GameUI::pushButton_post1()
{
	CURLcode res;
	curl = curl_easy_init();
	if (curl) {
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


void GameUI::pushButton_openWeb1()
{



	//QString link = "www.google.com";
	//QDes
	
	//int handle =  (HWND)ui.centralWidget->winId;
	//ui.textBrowser->loadResource(ui.textBrowser->winId(), QUrl("https://google.com"));
	QString link = "www.google.com";
	QDesktopServices::openUrl(QUrl(link));


	
}





