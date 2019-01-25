#include <iostream>
#include <map>
#include <string>

using namespace std;

class GameObject
{

};


void func1(map<string *, GameObject *> & stringTable)
{
	string temp = "hello";


	//stringTable[""] = new GameObject();

}

void func2(char * a)
{
	a = "123";
}

int main()
{
	char * a;
	func2(a);
	cout << a << endl;

	//map<string* , string> a;
	//map<string&, GameObject *> stringTable;
	//auto a = stringTable["1"];
	//string temp1 = "hello";

	//func1(stringTable);

	//auto it = stringTable.find(temp);

	//if (it == stringTable.end())
	//{
	//	cout << "not found" << endl;
	//}
	//else
	//{
	//	cout << (*it).second << endl;
	//}
	


	/*
		auto it = stringTable.find("1");
	
	if (it == stringTable.end())
	{
		cout << "not found" << endl;
	}
	else
	{
		cout << &(*it) << endl;
	}
	//map<int, int> intTable;
	//
	//auto b = intTable[1];
	//cout << b << endl;
	const string cStr = "hello";
	string str = "hello";
	if (str == cStr)
	{
		cout << "same value" << endl;
	}
	*/


	return 0;
}