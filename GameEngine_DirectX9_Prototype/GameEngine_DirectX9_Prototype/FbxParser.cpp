#include "FbxParser.h"

void FbxParser::init()
{
	// 매니저를 만든다.
	manager = FbxManager::Create();

	// 매니저를 이용해서 i/o setting객체를 만들어준다.
	ios = FbxIOSettings::Create(manager, IOSROOT);

	// 만든 i/o setting객체를 이용해서 setiosetting을 해준다.
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
	// 실제 Scene포인터(FbxInfo)를 가리키지 않을떄 리턴
	if (!scene) return;

	// 씬이 이미 있는 경우 리턴해준다.
	if ((*scene)) return;

	const char * cStrFileName = fileName.c_str();
	// 임포터를 만들어준다.
	FbxImporter* importer = FbxImporter::Create(manager, "");

	// Use the first argument as the filename for the importer.
	if (!importer->Initialize(cStrFileName, -1, manager->GetIOSettings()))
		return;

	// scene의 이름은 파일의 이름과 같게 해준다.
	(*scene) = FbxScene::Create(manager, cStrFileName);

	// 임포터를 이용해서 씬을 임포트 시킨다.
	importer->Import((*scene));

	// 파일이 임포트 되면 임포터를 지워준다.
	importer->Destroy();
}
