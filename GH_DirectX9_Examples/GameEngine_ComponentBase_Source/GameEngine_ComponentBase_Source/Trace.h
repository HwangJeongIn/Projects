#ifndef TRACE_H
#define TRACE_H

#include <iostream>
#include <fstream>
#include <string>
#include <map>

using namespace std;

class Trace
{
private :
	// 포인터로 작업할지는 나중에 생각 일단 구현
	static string TextFileNamesList_name;
	static map<string, string> TextFileTable;

	// 멀티스레드 프로그래밍이 아닌경우 필요없을거라고 생각되지만 일단 작성
public :
	static void LoadFileNames()
	{
		// 텍스트 파일에 들어가 있는 정보들을 모두받아서 맵에 저장한다.

		ifstream namesListFile(TextFileNamesList_name);
		if (namesListFile.is_open())
		{
			string line, tag, fileName;
			string temp;

			while (getline(namesListFile, line))
			{
				// 일단 태그와 텍스트 파일명이 나오는 라인까지 이동해야한다.
				// 그 라인은 TAG_로 시작하는 라인이다
				temp = line.substr(0, 4);
				if (temp != "TAG_")
					continue;

				// TAG_로 시작하는 라인

				// tab기준으로 tag와 fileName이 나누어져있기 때문에 탭기준으로 찾는다.
				int tabkeyPos = line.find('\t', 0);

				// tag와 fileName을 초기화한다.
				tag = line.substr(0, tabkeyPos );
				fileName = line.substr(tabkeyPos + 1, line.size()-(tabkeyPos + 1));

				TextFileTable[tag] = fileName;
			}
			namesListFile.close();
		}
		else cout << "Unable to open file" << endl;

	}

	static bool Write(const string & fileTagName, const string & info/*, bool appMode = true*/)
	{
		bool appMode = true;
		// 먼저 태그로 검색해서 등록되어있는지 확인한다.
		// 만약 등록되어있지 않다면 리턴한다.
		if (TextFileTable.find(fileTagName) == TextFileTable.end()) return false;

		// 쓰기전용으로 파일객체를 만들고 열어준다.
		// 역시 파일 열기에 실패했으면 리턴한다.
		unsigned int mode = ios_base::out;
		if (appMode) mode |= ios_base::app;

		ofstream file(TextFileTable[fileTagName], mode);

		if (!file.is_open())
		{
			cout << "Unable to open file " << TextFileTable[fileTagName] << endl;
			return false;
		}

		file << info << endl;

		file.close();
	}

	static bool Write(const string & fileTagName, const string & tag, const string & info/*, bool appMode = true*/)
	{
		bool appMode = true;
		// 먼저 태그로 검색해서 등록되어있는지 확인한다.
		// 만약 등록되어있지 않다면 리턴한다.
		if (TextFileTable.find(fileTagName) == TextFileTable.end()) return false;

		// 쓰기전용으로 파일객체를 만들고 열어준다.
		// 역시 파일 열기에 실패했으면 리턴한다.
		unsigned int mode = ios_base::out;
		if (appMode) mode |= ios_base::app;

		ofstream file(TextFileTable[fileTagName], mode);

		if (!file.is_open())
		{
			cout << "Unable to open file " << TextFileTable[fileTagName] << endl;
			return false;
		}

		file << tag << " :: " << info << endl;

		file.close();
	}

	static void Clear(const string & fileTagName)
	{
		if (TextFileTable.find(fileTagName) == TextFileTable.end()) return;

		ofstream file(TextFileTable[fileTagName]); file.close();
	}
};

//for (auto it = TextFileTable.begin(); it != TextFileTable.end(); ++it)
//{
//	debugFile << it->first << "\t" << it->second << endl;
//}
#endif