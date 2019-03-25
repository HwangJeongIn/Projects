#include "FbxParser.h"

void FbxParser::init()
{
	// �Ŵ����� �����.
	manager = FbxManager::Create();

	// �Ŵ����� �̿��ؼ� i/o setting��ü�� ������ش�.
	ios = FbxIOSettings::Create(manager, IOSROOT);

	// ���� i/o setting��ü�� �̿��ؼ� setiosetting�� ���ش�.
	manager->SetIOSettings(ios);

	converter = new FbxGeometryConverter(manager);
}

void FbxParser::release()
{
	if (ios)
		ios->Destroy();
	ios = nullptr;

	if (converter)
		delete converter;

	if (manager)
		manager->Destroy();
	manager = nullptr;

}


void FbxParser::loadSceneFromFbxFile(const string & fileName, FbxScene ** scene, FbxImporter ** importer)
{
	// ���� Scene������(FbxInfo)�� ����Ű�� ������ ����
	// ���������� ���� Importer�����͸� ����Ű�� ������ ����
	if (!scene || !importer) return;

	// ���� �̹� �ִ� ��� �������ش�.
	if ((*scene)) return;

	const char * cStrFileName = fileName.c_str();
	// �����͸� ������ش�.
	 (*importer) = FbxImporter::Create(manager, "");

	// Use the first argument as the filename for the importer.
	if (!(*importer)->Initialize(cStrFileName, -1, manager->GetIOSettings()))
		return;

	// scene�� �̸��� ������ �̸��� ���� ���ش�.
	(*scene) = FbxScene::Create(manager, cStrFileName);

	// �����͸� �̿��ؼ� ���� ����Ʈ ��Ų��.
	(*importer)->Import((*scene));


	// �����͵� �ʿ��ϱ� ������ ������Ʈ���� ������ �ְ� �����.
	// ������ ����Ʈ �Ǹ� �����͸� �����ش�.
	//importer->Destroy();
}

void FbxParser::convertGeometryInfo(FbxNodeAttribute ** nodeAttribute)
{
	// ���������Ͱ� nullptr�϶� ����
	if (!nodeAttribute) return;

	// �����ʹ� �������� ���� ���� ������ �ʴ� ��� ����
	// ���� attribute���� ���� ���
	if ((*nodeAttribute) == nullptr) return;

	// nodeAttribute�� ��������� �˾ƺ��� �� ���¿� ���ؼ��� ��ȯ���ش�.
	switch ((*nodeAttribute)->GetAttributeType())
	{
		case FbxNodeAttribute::eSkeleton :
		{
			converter->Triangulate((*nodeAttribute), true, true);
		}
		break;

		case FbxNodeAttribute::eMesh :
		{
			converter->Triangulate((*nodeAttribute), true, true);
		}
		break;
	}

}
