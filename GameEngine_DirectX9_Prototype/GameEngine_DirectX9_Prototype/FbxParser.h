#ifndef FBXPARSER_H
#define FBXPARSER_H

#include <fbxsdk.h>
#include <iostream>
#include <vector>
#include <map>
#include <string>

using namespace std;


/*
���� ������ִ� ������ �ϴ� Ŭ����
���� �ý��ۿ� ����Ѵ�.

manager�� �ϳ��� ������ �Ǳ� ������ �̱������� ���־�� �ϴµ� ������� �����ұ�?

** �����ϴ� ���
1. GameObject�� ���� �����Ѵ�.
   ���� : ���ݱ��� �ý��ۿ� �����Ҷ��� ���� ������� �ϰ����� �� �� �ִ�.
   ���� : GameObject�ʹ� ���������� ������ ���ٰ� ������ ���� �־ ���� �������� ������ ����.
		  �� Component�ȿ��� loadSceneFromFBXFile�� ȣ���ؼ� ���������� �̸��� FbxInfo.getScene()�� ȣ���ؼ� �Ѱ���� �Ѵ�.
		  �׳� Fbx���ο��� ó�����شٸ� �̷��� ���� �� �ʿ䰡 ����. �׸��� ���������� FbxInfo�� ���������� ����� �ʱ�ȭ �� �� ������
		  �ܺ��� �ý����� �޾ƿͼ� �ܺ��ڵ忡�� �ʱ�ȭ ��������Ѵ�. // �� FbxInfo�� ���������� ����� �� ����. �ܺ� �ý��ۿ� �������̴�.

2. FbxInfo���� �����ؼ� ���������� �ٷ� �ʱ�ȭ�� ���ִ� ���
	���� : FbxInfo�� Parser�� �������̽��� �ٷ� ������ �����ϱ� ������ ���������ε� �ʱ�ȭ�� �����ϴ�.
	���� : �ᱹ �̱����ϳ��� Locator���� ����� ���ǰ� �ȴ�. �ʱ�ȭ���ִ� �۾��� ���ִ� 
		   �������̽��� ���� ������־�� �ϸ�(Locator���� register�ϴ� �۾�) �߰������� ����� ���Ѿ� �ϴ� ���ŷο����ִ�.


�̱����� �Ѱ��� ��Ƽ� ���, ����, ��ü�� �ý����� �ϰ����� ���ؼ� 1��ä��
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
		// �νý����� �ƴҶ��� �ʱ�ȭ
		if(!isNullSystem)
			init();
	}

	virtual ~FbxParser()
	{
		// �νý����� �ƴҶ��� �ʱ�ȭ
		if (!isNullSystem)
			release();
	}

public:

	friend class Locator;

	// FbxInfo�� �ִ� scene�����Ͱ� ����Ű�� ���� ��������ְ�ͱ� ������
	// ���� �����͸� ������ش�.
	// �� ���⼭ ���۷��� ������ ���� ���ϴ� ������ ���ο��� Create�Լ��� Scene�����͸� ��ȯ�ϱ� �����̴�.
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
	//  ���߿� �α� ��� ������ �ٲܿ���

};

#endif