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
	// �����ͷ� �۾������� ���߿� ���� �ϴ� ����
	static string TextFileNamesList_name;
	static map<string, string> TextFileTable;

	// ��Ƽ������ ���α׷����� �ƴѰ�� �ʿ�����Ŷ�� ���������� �ϴ� �ۼ�
public :
	static void LoadFileNames()
	{
		// �ؽ�Ʈ ���Ͽ� �� �ִ� �������� ��ι޾Ƽ� �ʿ� �����Ѵ�.

		ifstream namesListFile(TextFileNamesList_name);
		if (namesListFile.is_open())
		{
			string line, tag, fileName;
			string temp;

			while (getline(namesListFile, line))
			{
				// �ϴ� �±׿� �ؽ�Ʈ ���ϸ��� ������ ���α��� �̵��ؾ��Ѵ�.
				// �� ������ TAG_�� �����ϴ� �����̴�
				temp = line.substr(0, 4);
				if (temp != "TAG_")
					continue;

				// TAG_�� �����ϴ� ����

				// tab�������� tag�� fileName�� ���������ֱ� ������ �Ǳ������� ã�´�.
				int tabkeyPos = line.find('\t', 0);

				// tag�� fileName�� �ʱ�ȭ�Ѵ�.
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
		// ���� �±׷� �˻��ؼ� ��ϵǾ��ִ��� Ȯ���Ѵ�.
		// ���� ��ϵǾ����� �ʴٸ� �����Ѵ�.
		if (TextFileTable.find(fileTagName) == TextFileTable.end()) return false;

		// ������������ ���ϰ�ü�� ����� �����ش�.
		// ���� ���� ���⿡ ���������� �����Ѵ�.
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
		// ���� �±׷� �˻��ؼ� ��ϵǾ��ִ��� Ȯ���Ѵ�.
		// ���� ��ϵǾ����� �ʴٸ� �����Ѵ�.
		if (TextFileTable.find(fileTagName) == TextFileTable.end()) return false;

		// ������������ ���ϰ�ü�� ����� �����ش�.
		// ���� ���� ���⿡ ���������� �����Ѵ�.
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