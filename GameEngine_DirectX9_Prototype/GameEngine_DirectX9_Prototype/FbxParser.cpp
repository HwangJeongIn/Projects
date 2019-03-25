#include "FbxParser.h"

void FbxParser::init()
{
	// 매니저를 만든다.
	manager = FbxManager::Create();

	// 매니저를 이용해서 i/o setting객체를 만들어준다.
	ios = FbxIOSettings::Create(manager, IOSROOT);

	// 만든 i/o setting객체를 이용해서 setiosetting을 해준다.
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
	// 실제 Scene포인터(FbxInfo)를 가리키지 않을떄 리턴
	// 마찬가지로 실제 Importer포인터를 가리키지 않을때 리턴
	if (!scene || !importer) return;

	// 씬이 이미 있는 경우 리턴해준다.
	if ((*scene)) return;

	const char * cStrFileName = fileName.c_str();
	// 임포터를 만들어준다.
	 (*importer) = FbxImporter::Create(manager, "");

	// Use the first argument as the filename for the importer.
	if (!(*importer)->Initialize(cStrFileName, -1, manager->GetIOSettings()))
		return;

	// scene의 이름은 파일의 이름과 같게 해준다.
	(*scene) = FbxScene::Create(manager, cStrFileName);

	// 임포터를 이용해서 씬을 임포트 시킨다.
	(*importer)->Import((*scene));


	// 임포터도 필요하기 때문에 컴포넌트에서 가지고 있게 만든다.
	// 파일이 임포트 되면 임포터를 지워준다.
	//importer->Destroy();
}

void FbxParser::convertGeometryInfo(FbxNodeAttribute ** nodeAttribute)
{
	// 더블포인터가 nullptr일때 리턴
	if (!nodeAttribute) return;

	// 포인터는 가지지만 실제 값을 가지지 않는 경우 리턴
	// 실제 attribute값이 없는 경우
	if ((*nodeAttribute) == nullptr) return;

	// nodeAttribute가 어떤형태인지 알아보고 그 형태에 대해서만 변환해준다.
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
