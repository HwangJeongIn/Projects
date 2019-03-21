#ifndef FBXPARSER_H
#define FBXPARSER_H

#include <fbxsdk.h>
#include <iostream>
#include <vector>
#include <map>
#include <string>

using namespace std;


/*
씬을 만들어주는 역할을 하는 클래스
내부 시스템에 등록한다.

manager는 하나만 있으면 되기 때문에 싱글턴으로 해주어야 하는데 어떤식으로 접근할까?

** 접근하는 방법
1. GameObject를 통해 접근한다.
   장점 : 지금까지 시스템에 접근할때와 같은 방식으로 일관성을 줄 수 있다.
   단점 : GameObject와는 직접적으로 관련이 없다고 생각할 수도 있어서 괜히 끼워맞춘 느낌이 난다.
		  또 Component안에서 loadSceneFromFBXFile을 호출해서 내부적으로 이름과 FbxInfo.getScene()을 호출해서 넘겨줘야 한다.
		  그냥 Fbx내부에서 처리해준다면 이러한 수고를 할 필요가 없다. 그리고 결정적으로 FbxInfo는 내부적으로 절대로 초기화 될 수 없으며
		  외부의 시스템을 받아와서 외부코드에서 초기화 시켜줘야한다. // 즉 FbxInfo는 독립적으로 사용할 수 없다. 외부 시스템에 종속적이다.

2. FbxInfo에서 접근해서 내부적으로 바로 초기화를 해주는 방법
	장점 : FbxInfo가 Parser의 인터페이스에 바로 접근이 가능하기 떄문에 독립적으로도 초기화가 가능하다.
	단점 : 결국 싱글턴하나가 Locator에서 벗어나서 사용되게 된다. 초기화해주는 작업을 해주는 
		   인터페이스를 따로 만들어주어야 하며(Locator에서 register하는 작업) 추가적으로 상속을 시켜야 하는 번거로움이있다.


싱글턴을 한곳에 모아서 사용, 해제, 전체적 시스템의 일관성을 위해서 1번채택
*/

class Locator;

class FbxParser
{
private:
	FbxManager* manager;
	FbxIOSettings *ios;
	FbxGeometryConverter * converter;
	bool isNullSystem;
	void init();

	void release();

protected:

	FbxParser(bool isNullSystem = false)
		: manager(nullptr), ios(nullptr), converter(nullptr)
	{
		// 널시스템이 아닐때만 초기화
		if(!isNullSystem)
			init();
	}

	virtual ~FbxParser()
	{
		// 널시스템이 아닐때만 초기화
		if (!isNullSystem)
			release();
	}

public:

	friend class Locator;

	// FbxInfo에 있는 scene포인터가 가리키는 값을 변경시켜주고싶기 때문에
	// 더블 포인터를 사용해준다.
	// 단 여기서 레퍼런스 변수를 쓰지 못하는 이유는 내부에서 Create함수가 Scene포인터를 반환하기 때문이다.
	virtual void loadSceneFromFbxFile(const string & fileName, FbxScene ** scene);
	virtual void convertGeometryInfo(FbxNodeAttribute ** nodeAttribute);
};

class NullFbxParser : public FbxParser
{
private:
	NullFbxParser()
		: FbxParser(true) {}
	virtual ~NullFbxParser() {}
	virtual void loadSceneFromFbxFile(const string & fileName, FbxScene ** scene) {}

public:
	friend class Locator;

};

class DebuggingFbxParser : public FbxParser
{

private:
	FbxParser & wrapped;

	DebuggingFbxParser(FbxParser & fbxParser)
		: wrapped(fbxParser) {}
	virtual ~DebuggingFbxParser() {}

	void log(const char * message)
	{
		if (!message) return;
		cout << message << endl;
	}

	FbxParser * getWrappedFbxParser() { return &wrapped; }
public:
	friend class Locator;
	//  나중에 로그 찍는 것으로 바꿀예정

};

#endif