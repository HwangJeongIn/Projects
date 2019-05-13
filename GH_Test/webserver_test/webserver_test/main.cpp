#define CURL_STATICLIB

#include <stdio.h>
#include <curl/curl.h>
#include <iostream>
#include <windows.h>
#include <string>
#include "gameui.h"
#include <QtWidgets/QApplication>

#pragma comment(lib, "wldap32.lib")
#pragma comment(lib, "ws2_32.lib")

using namespace std;


/*
libeay32.lib
ssleay32.lib
wldap32.lib
ws2_32.lib
*/

int main(void)
{
	int tempA = 0;
	QApplication a(tempA, nullptr);
	GameUI w;
	w.show();
	a.exec();

	return 0;

	//CURL *curl;
	//CURLcode res;

	//curl = curl_easy_init();
	//
	//string url;
	//cin >> url;

	//ShellExecute(NULL, (WCHAR*)"open", (WCHAR*)url.c_str(), NULL, NULL, SW_SHOWNORMAL);
	//
	//return 0;
	//if (curl) {
	//	//https://google.com
	//	curl_easy_setopt(curl, CURLOPT_URL, "google.com");
	//	/* example.com is redirected, so we tell libcurl to follow redirection */
	//	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

	//	/* Perform the request, res will get the return code */
	//	res = curl_easy_perform(curl);
	//	/* Check for errors */
	//	if (res != CURLE_OK)
	//		fprintf(stderr, "curl_easy_perform() failed: %s\n",
	//			curl_easy_strerror(res));

	//	/* always cleanup */
	//	curl_easy_cleanup(curl);
	//}
	//return 0;
}


