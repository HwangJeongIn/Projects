#include "FbxParser.h"

void FbxParser::init()
{
	// �Ŵ����� �����.
	manager = FbxManager::Create();

	// �Ŵ����� �̿��ؼ� i/o setting��ü�� ������ش�.
	ios = FbxIOSettings::Create(manager, IOSROOT);

	// ���� i/o setting��ü�� �̿��ؼ� setiosetting�� ���ش�.
	manager->SetIOSettings(ios);
}

void FbxParser::release()
{
	if (ios)
		ios->Destroy();
	ios = nullptr;

	if (manager)
		manager->Destroy();
	manager = nullptr;

}


void FbxParser::loadSceneFromFbxFile(const string & fileName, FbxScene ** scene)
{
	// ���� Scene������(FbxInfo)�� ����Ű�� ������ ����
	if (!scene) return;

	// ���� �̹� �ִ� ��� �������ش�.
	if ((*scene)) return;

	const char * cStrFileName = fileName.c_str();
	// �����͸� ������ش�.
	FbxImporter* importer = FbxImporter::Create(manager, "");

	// Use the first argument as the filename for the importer.
	if (!importer->Initialize(cStrFileName, -1, manager->GetIOSettings()))
		return;

	// scene�� �̸��� ������ �̸��� ���� ���ش�.
	(*scene) = FbxScene::Create(manager, cStrFileName);

	// �����͸� �̿��ؼ� ���� ����Ʈ ��Ų��.
	importer->Import((*scene));

	// ������ ����Ʈ �Ǹ� �����͸� �����ش�.
	importer->Destroy();
}
