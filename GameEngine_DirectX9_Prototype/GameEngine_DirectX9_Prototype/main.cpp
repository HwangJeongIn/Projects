
#include <conio.h>
#include "d3dUtility.h"
#include "camera.h"
#include "Locator.h"
#include "Scene.h"

#include "gameui.h"
#include <QtWidgets/qapplication.h>


//
// Globals
//


const int Width  = 1280;
const int Height = 720;

//Camera TheCamera(Camera::LANDOBJECT);

//
// Framework functions
//
/*bool Setup()
{
	//
	// Setup a basic scene.  The scene will be created the
	// first time this function is called.
	//

	d3d::DrawBasicScene(Device, 0.0f); 

	//
	// Set projection matrix.
	//

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
			&proj,
			D3DX_PI * 0.25f, // 45 - degree
			(float)Width / (float)Height,
			1.0f,
			1000.0f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);


	Trace::LoadFileNames();
	Trace::Clear("TAG_DEBUG");



	return true;
}*/

//void Cleanup()
//{
//	// pass 0 for the first parameter to instruct cleanup.
//	d3d::DrawBasicScene(0, 0.0f);
//}

void SetStartScene()
{


	// startScene
	Locator::provideScene(Locator::SystemType::RELEASETYPE, Locator::SceneType::START);

	// mainCamera
	GameObject * mainCamera = GameObject::Instantiate("mainCamera", "MainCamera");
	mainCamera->addComponent<MainCamera>();
	mainCamera->getTransform()->setWorldPosition(0, 40, -100);
	mainCamera->getTransform()->setWorldRotation(15, 0, 0);

	// player
	GameObject * player = GameObject::Instantiate("player", "Player");
	player->addChild(mainCamera);
	//player->addComponent<MeshRenderer>()->loadXFile("car.x");
	player->addComponent<PlayerScript>();
	RigidBody * playerRigidBody = player->addComponent<RigidBody>();
	playerRigidBody->setSphereCollider(2);

	//FbxModelRenderer * fbxModelRendererplayer = player->addComponent<FbxModelRenderer>();
	//fbxModelRendererplayer->loadFbxFile("akai_e_espiritu.fbx");
	//AnimationFSM * playerAnimationFSM = player->addComponent<AnimationFSM>();



	//MoveOnTerrainScript * playerMoveOnTerrainScript = player->addComponent<MoveOnTerrainScript>();
	//car1MoveOnTerrainScript->setTerrain(groundTerrain);

	//playerMoveOnTerrainScript->setObjectHeight(4.0f);
	player->getTransform()->setWorldPosition(-15, 0, 0);
	player->getTransform()->setWorldRotation(0, 180, 0);

	GameObject * playerChild1 = player->addChild("bigShip1", "BigShip");
	playerChild1->addComponent<MeshRenderer>()->loadXFile("bigship1.x");
	playerChild1->getTransform()->setLocalPosition(5, 3, 0);
	playerChild1->getTransform()->setLocalRotation(0, 0, 0);

	playerChild1->addComponent<MoveScript_C>();


}

void SetEndScene()
{
	// endScene
	Locator::provideScene(Locator::SystemType::RELEASETYPE, Locator::SceneType::END);

	// mainCamera
	GameObject * mainCamera = GameObject::Instantiate("mainCamera", "MainCamera");
	mainCamera->addComponent<MainCamera>();
	mainCamera->getTransform()->setWorldPosition(0, 40, -100);
	mainCamera->getTransform()->setWorldRotation(15, 0, 0);

	// player
	GameObject * player = GameObject::Instantiate("player", "Player");
	player->addChild(mainCamera);
	//player->addComponent<MeshRenderer>()->loadXFile("car.x");
	player->addComponent<PlayerScript>();
	RigidBody * playerRigidBody = player->addComponent<RigidBody>();
	playerRigidBody->setSphereCollider(2);

	//FbxModelRenderer * fbxModelRendererplayer = player->addComponent<FbxModelRenderer>();
	//fbxModelRendererplayer->loadFbxFile("akai_e_espiritu.fbx");
	//AnimationFSM * playerAnimationFSM = player->addComponent<AnimationFSM>();



	//MoveOnTerrainScript * playerMoveOnTerrainScript = player->addComponent<MoveOnTerrainScript>();
	//car1MoveOnTerrainScript->setTerrain(groundTerrain);

	//playerMoveOnTerrainScript->setObjectHeight(4.0f);
	player->getTransform()->setWorldPosition(-15, 0, 0);
	player->getTransform()->setWorldRotation(0, 180, 0);

	GameObject * playerChild1 = player->addChild("bigShip1", "BigShip");
	playerChild1->addComponent<MeshRenderer>()->loadXFile("bigship1.x");
	playerChild1->getTransform()->setLocalPosition(5, 3, 0);
	playerChild1->getTransform()->setLocalRotation(0, 0, 0);

	playerChild1->addComponent<MoveScript_C>();
}


Terrain * GenerateTerrain()
{
	// ground
	GameObject * ground = GameObject::Instantiate("ground", "Ground");
	//RigidBody * groundRigidBody = ground->addComponent<RigidBody>();
	//groundRigidBody->setBoxCollider(Vector3{ 200,10,200 });
	//groundRigidBody->turnOnStaticFlag();
	Terrain * groundTerrain = ground->addComponent<Terrain>();
	// adsfasfdSsabgaw.raw / coastMountain64.raw / Ash.raw / adsfsaf.raw


	//groundTerrain->loadRawFile("coastMountain64.raw", 64, 64, 25, .5f);
	groundTerrain->loadRawFile("terrain.raw", 129, 129, 7.0, .5f);
	
	groundTerrain->loadTextureFromFile("grass.bmp");

	srand(time(0));

	for (int i = 0; i < 10; ++i)
	{
		// 터레인의 로컬좌표계 기준으로 나무를 심어준다. // 즉 중심이 원점이고 xz평면으로 뻗어있는 터레인기준으로 생각해준다.

		Vector3 pos = { 0,0,0 };
		float depth = groundTerrain->getDepth();
		float width = groundTerrain->getWidth();

		float y = 0.0f;

		pos.setX((rand() % (int)width) - width / 2);
		pos.setZ((rand() % (int)depth) - depth / 2);
		while (!groundTerrain->getLocalHeight(pos, &y))
		{
			// 범위안에 들어올때까지 뽑아준다. 
			// 범위 맞춰서 랜덤값 적용해서 무조건 통과하긴하겠지만 그냥 예외처리하였다.
			pos.setX((rand() % (int)width) - width / 2);
			pos.setZ((rand() % (int)depth) - depth / 2);

		}
		pos.setY(y + 40);

		char randomChar = rand() % 3 + '0';

		string tempName = "tree";
		tempName += randomChar;
		tempName += ".jpg";
		GameObject * groundChild1 = ground->addChild("billBoard", "BillBoard");
		BillBoard * billBoard1 = groundChild1->addComponent<BillBoard>();
		billBoard1->generateBillBoard();
		billBoard1->loadTextureFromFile(tempName);
		groundChild1->getTransform()->setWorldPosition(pos);
	}

	for (int i = 0; i < 30; ++i)
	{
		// 터레인의 로컬좌표계 기준으로 나무를 심어준다. // 즉 중심이 원점이고 xz평면으로 뻗어있는 터레인기준으로 생각해준다.

		Vector3 pos = { 0,0,0 };
		float depth = groundTerrain->getDepth();
		float width = groundTerrain->getWidth();

		float y = 0.0f;

		pos.setX((rand() % (int)width) - width / 2);
		pos.setZ((rand() % (int)depth) - depth / 2);
		while (!groundTerrain->getLocalHeight(pos, &y))
		{
			// 범위안에 들어올때까지 뽑아준다. 
			// 범위 맞춰서 랜덤값 적용해서 무조건 통과하긴하겠지만 그냥 예외처리하였다.
			pos.setX((rand() % (int)width) - width / 2);
			pos.setZ((rand() % (int)depth) - depth / 2);

		}
		pos.setY(y + 5);

		char randomChar = rand() % 2 + '0';

		string tempName = "grass";
		tempName += randomChar;
		tempName += ".jpg";
		GameObject * groundChild1 = ground->addChild("billBoard", "BillBoard");
		BillBoard * billBoard1 = groundChild1->addComponent<BillBoard>();
		billBoard1->setSize(20, 20);
		billBoard1->generateBillBoard();
		billBoard1->loadTextureFromFile(tempName);
		groundChild1->getTransform()->setWorldPosition(pos);
	}
	return groundTerrain;
}



LRESULT CALLBACK d3d::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg )
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
		
	case WM_KEYDOWN:
		if( wParam == VK_ESCAPE )
			::DestroyWindow(hwnd);

		break;
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

//
// WinMain
//
int WINAPI WinMain(HINSTANCE hinstance,
	HINSTANCE prevInstance,
	PSTR cmdLine,
	int showCmd)
{
	IDirect3DDevice9* device = 0;


	//if(!d3d::InitD3D(hinstance,
	//	Width, Height, true, D3DDEVTYPE_HAL, &device))
	//{
	//	::MessageBox(0, "InitD3D() - FAILED", 0, 0);
	//	return 0;
	//}

	int tempA = 0;
	QApplication a(tempA, 0);

	GameUI gameUI;

	//////////////////////////////////////////////////////////////////////////
	gameUI.initD3D(Width, Height, true, D3DDEVTYPE_HAL, &device);


	gameUI.show();
	//layout->addWidget(pDxWidget);
	//w.centralWidget()->setLayout(layout);
	////w.setLayout(layout);

	//pDxWidget->initD3D();
	//pDxWidget->StartRenderFunc(30);
	//pDxWidget->show();

	//////////////////////////////////////////////////////////////////////////



	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI * 0.25f, // 45 - degree
		(float)Width / (float)Height,
		5.0f,
		8000.0f);
	device->SetTransform(D3DTS_PROJECTION, &proj);
	// wrap adress mode로 텍스처 지정
	device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	// 품질
	device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

	// 밉맵필터
	device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	D3DXVECTOR3 dir(1.0f, -1.0f, 1.0f);
	D3DXCOLOR col(1.0f, 1.0f, 1.0f, 1.0f);
	D3DLIGHT9 light = d3d::InitDirectionalLight(&dir, &col);

	device->SetLight(0, &light);
	device->LightEnable(0, true);

	device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	device->SetRenderState(D3DRS_SPECULARENABLE, true);

	//device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);


	// 디버깅용 txt파일을 로드
	Trace::LoadFileNames();
	Trace::Clear("TAG_DEBUG");

	// Gizmos클래스 초기화
	Gizmos::InitGizmos(device);

	/*
	서비스 등록
	*/
	Locator::provideScene(Locator::SystemType::RELEASETYPE);
	//Locator::provideAudio(Locator::SystemType::RELEASETYPE);
	Locator::providePhysics(Locator::SystemType::RELEASETYPE);
	Locator::provideFbxParser(Locator::SystemType::RELEASETYPE);
	Locator::provideDevice(device);


	SetStartScene();
	SetEndScene();


	// mainScene
	Locator::provideScene(Locator::SystemType::RELEASETYPE, Locator::SceneType::MAIN);
	//Scene의 카메라와 기본 월드 세팅을 해준다.
	GameObject * mainCamera = GameObject::Instantiate("mainCamera", "MainCamera");
	mainCamera->addComponent<MainCamera>();
	mainCamera->getTransform()->setWorldPosition(0, 50, -100);
	mainCamera->getTransform()->setWorldRotation(25, 0, 0);



	//// remover
	//GameObject * remover = GameObject::Instantiate("remover", "Remover");
	////ground->addComponent<MeshRenderer>()->loadXFile("car.x");
	//remover->addComponent<MeshRenderer>()->loadXFile("bigship1.x");
	//RigidBody * removerRigidBody = remover->addComponent<RigidBody>();
	//remover->getTransform()->setPosition(0, 0, 0);
	//removerRigidBody->setBoxCollider(Vector3{ 4, 4, 4 });
	////removerRigidBody->turnOnIsTriggerFlag();
	////removerRigidBody->turnOnStaticFlag();

	//// remover2
	//GameObject * remover2 = GameObject::Instantiate("remover2", "Remover");
	////ground->addComponent<MeshRenderer>()->loadXFile("car.x");
	//remover2->addComponent<MeshRenderer>()->loadXFile("bigship1.x");
	//RigidBody * remover2RigidBody = remover2->addComponent<RigidBody>();
	//remover2->getTransform()->setPosition(10, 0, 0);
	//remover2RigidBody->setBoxCollider(Vector3{ 4, 4, 4 });
	//remover2RigidBody->turnOnIsTriggerFlag();
	//remover2RigidBody->turnOnStaticFlag();



	//D3DMATERIAL9 mtrl;
	//// Set the RGBA for diffuse reflection.
	//mtrl.Diffuse.r = 1.0f;
	//mtrl.Diffuse.g = 1.0f;
	//mtrl.Diffuse.b = 1.0f;
	//mtrl.Diffuse.a = 1.0f;
	//// Set the RGBA for ambient reflection.
	//mtrl.Ambient.r = 1.0f;
	//mtrl.Ambient.g = 1.0f;
	//mtrl.Ambient.b = 1.0f;
	//mtrl.Ambient.a = 1.0f;
	//// Set the color and sharpness of specular highlights.
	//mtrl.Specular.r = 1.0f;
	//mtrl.Specular.g = 1.0f;
	//mtrl.Specular.b = 1.0f;
	//mtrl.Specular.a = 1.0f;
	//mtrl.Power = 500.0f;
	//// Set the RGBA for emissive color.
	//mtrl.Emissive.r = 0.0f;
	//mtrl.Emissive.g = 0.0f;
	//mtrl.Emissive.b = 0.0f;
	//mtrl.Emissive.a = 0.0f;
	//groundTerrain->setMaterial(mtrl);


	// -1단계
	//GameObject * player = GameObject::Instantiate("player", "Player");
	//															// Fir_Tree.fbx / free_male_1.FBX / Fortress_Gate.FBX / Rabbit.fbx / akai_e_espiritu.fbx / Crouch Walk Left.fbx / Standing Aim Recoil.fbx
	//// standing run forward.fbx
	//FbxModelRenderer * fbxModelRendererplayer = player->addComponent<FbxModelRenderer>();
	//fbxModelRendererplayer->loadFbxFile("akai_e_espiritu.fbx");
	////fbxModelRendererplayer->play("mixamo.com");
	////fbxModelRendererplayer->setScale(Vector3(3,1,3));
	//AnimationFSM * playerAnimationFSM = player->addComponent<AnimationFSM>();

	//player->getTransform()->setRotation(Vector3( 0,180,0 ));
	//player->getTransform()->setPosition(Vector3(0, 00, 0));
	//player->addComponent<PlayerScript>();
	//RigidBody * playerRigidBody = player->addComponent<RigidBody>();
	//playerRigidBody->setSphereCollider(2);



	// 0단계
	//GameObject * car0 = GameObject::Instantiate("car0", "Car");
	//car0->addComponent<MeshRenderer>()->loadXFile("car.x");
	//car0->addComponent<RigidBody>()->setSphereCollider(2);
	///car0->addComponent<BoxCollider>();

	// player
	GameObject * player = GameObject::Instantiate("player", "Player");
	player->addChild(mainCamera);
	//player->addComponent<MeshRenderer>()->loadXFile("car.x");
	player->addComponent<PlayerScript>();
	RigidBody * playerRigidBody = player->addComponent<RigidBody>();
	playerRigidBody->setSphereCollider(2);

	//FbxModelRenderer * fbxModelRendererplayer = player->addComponent<FbxModelRenderer>();
	//fbxModelRendererplayer->loadFbxFile("akai_e_espiritu.fbx");
	//AnimationFSM * playerAnimationFSM = player->addComponent<AnimationFSM>();



	MoveOnTerrainScript * playerMoveOnTerrainScript = player->addComponent<MoveOnTerrainScript>();
	//car1MoveOnTerrainScript->setTerrain(groundTerrain);

	playerMoveOnTerrainScript->setObjectHeight(3.0f);
	player->getTransform()->setWorldPosition(-15, 0, 0);
	player->getTransform()->setWorldRotation(0, 0, 0);

	GameObject * playerChild1 = player->addChild("bigShip1", "BigShip");
	playerChild1->addComponent<MeshRenderer>()->loadXFile("bigship1.x");
	playerChild1->getTransform()->setLocalPosition(5, 3, 0);
	playerChild1->getTransform()->setLocalRotation(0, 0, 0);

	playerChild1->addComponent<MoveScript_C>();

	//GameObject * playerChild1Child1 = playerChild1->addChild("bigShip1", "BigShip");
	//playerChild1Child1->addComponent<MeshRenderer>()->loadXFile("bigship1.x");
	////playerChild1Child1->getTransform()->setLocalPosition(5, 3, 0);
	//playerChild1Child1->getTransform()->setLocalRotation(30, 30, 50);

	//playerChild1Child1->addComponent<MoveScript_C>();


	// enemy
	GameObject * enemy = GameObject::Instantiate("enemy", "Enemy");
	//enemy->addChild(mainCamera);
	enemy->addComponent<MeshRenderer>()->loadXFile("bigship1.x");
	BasicEnemyScript * enemyBasicEnemyScript = enemy->addComponent<BasicEnemyScript>();
	//enemy->addComponent<PlayerScript>();
	RigidBody * enemyRigidBody = enemy->addComponent<RigidBody>();
	enemyRigidBody->setSphereCollider(2);

	GameObject * enemyRangeCollider = enemy->addChild("enemyRangeCollider", "EnemyRangeCollider");
	//enemyRangeCollider->getTransform()->setLocalPosition(0, 0, 0);
	enemyRangeCollider->addComponent<MeshRenderer>()->loadXFile("car.x");

	RigidBody * enemyRangeColliderRigidBody = enemyRangeCollider->addComponent<RigidBody>();
	// 탐색 범위 10 // 안움직여야 하기 때문에 static객체 설정 + 뚫어야 하기 때문에 trigger객체 설정
	enemyRangeColliderRigidBody->setSphereCollider(30.0f);
	enemyRangeColliderRigidBody->turnOnIsTriggerFlag();
	//enemyRangeColliderRigidBody->setGravity(Vector3{ 0, 0, 0 });
	enemyRangeColliderRigidBody->turnOnStaticFlag();
	enemyRangeCollider->addComponent<BasicEnemySearchRangeScript>();

	MoveOnTerrainScript * enemyMoveOnTerrainScript = enemy->addComponent<MoveOnTerrainScript>();
	//car1MoveOnTerrainScript->setTerrain(groundTerrain);
	enemyMoveOnTerrainScript->setObjectHeight(3.0f);
	//enemy->getTransform()->setPosition(0, 0, 0);
	//enemy->getTransform()->setRotation(0, 0, 0);


	// 2단계

	/*GameObject * car1Child1Child1 = car1Child1->addChild("car3", "Car");
	car1Child1Child1->addComponent<MeshRenderer>()->loadXFile("car.x");
	car1Child1Child1->getTransform()->setPosition(0, 5, 0);

	GameObject * car1Child1Child2 = car1Child1->addChild("car4", "Car");
	car1Child1Child2->addComponent<MeshRenderer>()->loadXFile("car.x");
	car1Child1Child2->getTransform()->setPosition(7, 0, 0);*/


	// 맵등록 + 맵을 사용할 Script 등록
	Terrain * groundTerrain = GenerateTerrain();
	playerMoveOnTerrainScript->setTerrain(groundTerrain);
	enemyMoveOnTerrainScript->setTerrain(groundTerrain);

	// 하드코딩 스타트지점 맞추기------------------------------------------------------------------------------
	float tempHeight = 0.0f;
	groundTerrain->getHeight(enemy->getTransform()->getWorldPosition(), &tempHeight);
	const Vector3 & startPoint = enemyBasicEnemyScript->getStartPoint();
	enemyBasicEnemyScript->setStartPoint(Vector3{ startPoint.getX() , tempHeight ,startPoint.getZ() });
	//---------------------------------------------------------------------------------------------------------

	Scene & scene = mainCamera->getScene();

	// 클래스 멤버함수의 함수포인터는 또 다른식으로 정의해줘야한다.
	// 일단 클래스 명으로 지정 / 넘길때도 &을 붙여서 넘겨줌 / 사용할때는 그 클래스의 객체 기준으로 사용
	d3d::EnterMsgLoop( &Scene::gameLoop/*&(Scene::getInstance()->gameLoop)*/, &scene, *mainCamera, gameUI);


	// void(*ptr_display)(void)

	a.exit();
	Locator::release();
	device->Release();




	return 0;
}