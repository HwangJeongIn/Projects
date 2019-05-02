
#include <conio.h>
#include "Locator.h"
#include "Scene.h"

#include "gameui.h"
#include <QtWidgets/qapplication.h>


//
// Globals
//


const int Width  = 1920;
const int Height = 1080;


int EnterMsgLoop(void(Scene::* ptr_display)(void), GameObject & objToAccessSystem)
{
	MSG msg;
	::ZeroMemory(&msg, sizeof(MSG));

	//static float lastTime = (float)timeGetTime(); 
	Locator::provideScene(Locator::SystemType::RELEASETYPE, Locator::SceneType::START);
	objToAccessSystem.getGameUI().setStartSceneUI();
	Scene * scene = &objToAccessSystem.getScene();


	while (msg.message != WM_QUIT)
	{



		if (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{

			::TranslateMessage(&msg);
			::DispatchMessage(&msg);

		}
		else
		{
			// 입력받은 키들을 모두 저장해준다.
			InputManager::UpdateFrameStart();

			// 탈출 // 임시
			// VK_ESCAPE
			if (InputManager::GetKeyDown(VK_ESCAPE))
			{
				return 0;
			}


			// 여기서 씬을 바꿔준다.
			// 1키 : main
			// 2키 : start
			// 3키 : end
			if (InputManager::GetKeyDown(0x30))
			{
				Locator::provideScene(Locator::SystemType::RELEASETYPE, Locator::SceneType::MAIN);
				objToAccessSystem.getGameUI().setMainSceneUI();

			}
			else if (InputManager::GetKeyDown(0x38))
			{
				Locator::provideScene(Locator::SystemType::RELEASETYPE, Locator::SceneType::START);
				objToAccessSystem.getGameUI().setStartSceneUI();

			}
			else if (InputManager::GetKeyDown(0x39))
			{
				Locator::provideScene(Locator::SystemType::RELEASETYPE, Locator::SceneType::END);
				objToAccessSystem.getGameUI().setEndSceneUI();

			}


			scene = &(objToAccessSystem.getScene());

			//float currTime  = (float)timeGetTime();
			//float timeDelta = (currTime - lastTime);// *0.001f;
			if (scene)
				(scene->*ptr_display)();

			//gameUI.showUI();
			//char t[100]{};
			////_itoa_s(timeDelta, t, 10);
			//sprintf(t, "%f", timeDelta);
			//Trace::Write("TAG_DEBUG", "deltaTime", t);
			//lastTime = currTime;

			// 이전프레임의 입력배열을 최신화 시켜준다.
			InputManager::UpdateFrameEnd();


		}
	}
	return msg.wParam;
}


D3DLIGHT9 InitDirectionalLight(D3DXVECTOR3* direction, D3DXCOLOR* color)
{
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));

	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Ambient = *color * 0.4f;
	light.Diffuse = *color;
	light.Specular = *color * 0.6f;
	light.Direction = *direction;

	return light;
}

D3DLIGHT9 InitPointLight(D3DXVECTOR3* position, D3DXCOLOR* color)
{
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));

	light.Type = D3DLIGHT_POINT;
	light.Ambient = *color * 0.4f;
	light.Diffuse = *color;
	light.Specular = *color * 0.6f;
	light.Position = *position;
	light.Range = 1000.0f;
	light.Falloff = 1.0f;
	light.Attenuation0 = 1.0f;
	light.Attenuation1 = 0.0f;
	light.Attenuation2 = 0.0f;

	return light;
}

D3DLIGHT9 InitSpotLight(D3DXVECTOR3* position, D3DXVECTOR3* direction, D3DXCOLOR* color)
{
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));

	light.Type = D3DLIGHT_SPOT;
	light.Ambient = *color * 0.4f;
	light.Diffuse = *color;
	light.Specular = *color * 0.6f;
	light.Position = *position;
	light.Direction = *direction;
	light.Range = 1000.0f;
	light.Falloff = 1.0f;
	light.Attenuation0 = 1.0f;
	light.Attenuation1 = 0.0f;
	light.Attenuation2 = 0.0f;
	light.Theta = 0.5f;
	light.Phi = 0.7f;

	return light;
}



void SetStartScene()
{


	// startScene
	Locator::provideScene(Locator::SystemType::RELEASETYPE, Locator::SceneType::START);

	// mainCamera
	GameObject * mainCamera = GameObject::Instantiate("mainCamera", "MainCamera");
	mainCamera->addComponent<MainCamera>();
	mainCamera->getTransform()->setWorldPosition(0, 40, -100);
	mainCamera->getTransform()->setWorldRotation(15, 0, 0);

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
}

MoveOnTerrainScript * GenerateBasicEnemy(const Vector3 & startPosition, bool isBoss = false)
{
	// enemy
	GameObject * enemy = nullptr;
	if (isBoss)
		enemy = GameObject::Instantiate("enemyBoss", "EnemyBoss");
	else
		enemy = GameObject::Instantiate("enemy", "Enemy");

	FbxModelRenderer * fbxModelRendererEnemy = enemy->addComponent<FbxModelRenderer>();
	fbxModelRendererEnemy->loadFbxFile("mummy_rig.fbx");
	fbxModelRendererEnemy->setUpdateFlag(false);
	if(isBoss)
		enemy->getTransform()->setLocalScale(50, 50, 50);
	else
		enemy->getTransform()->setLocalScale(25, 25, 25);

	enemy->getTransform()->setWorldPosition(startPosition);
	//BasicEnemyAnimationFSM * basicEnemyAnimationFSM = enemy->addComponent<BasicEnemyAnimationFSM>();

	// 스타트포인트 알아서 설정됨
	BasicEnemyScript * enemyBasicEnemyScript = enemy->addComponent<BasicEnemyScript>();

	RigidBody * enemyRigidBody = enemy->addComponent<RigidBody>();

	if (isBoss)
		enemyRigidBody->setSphereCollider(20);
	else
		enemyRigidBody->setSphereCollider(5);
	//enemyRigidBody->setGravity(Vector3(0, 0.00001, 0));
	enemyRigidBody->turnOnStaticFlag();
	//enemyRigidBody->turnOnIsTriggerFlag();

	GameObject * enemyRangeCollider = enemy->addChild("enemyRangeCollider", "EnemyRangeCollider");
	//enemyRangeCollider->getTransform()->setLocalPosition(0, 0, 0);
	//enemyRangeCollider->addComponent<MeshRenderer>()->loadXFile("car.x");

	RigidBody * enemyRangeColliderRigidBody = enemyRangeCollider->addComponent<RigidBody>();
	// 탐색 범위 40 // 안움직여야 하기 때문에 static객체 설정 + 뚫어야 하기 때문에 trigger객체 설정
	enemyRangeColliderRigidBody->setSphereCollider(150.0f);
	enemyRangeColliderRigidBody->turnOnIsTriggerFlag();
	//enemyRangeColliderRigidBody->setGravity(Vector3(0, 0, 0));
	enemyRangeColliderRigidBody->turnOnStaticFlag();
	enemyRangeCollider->addComponent<BasicEnemySearchRangeScript>();

	MoveOnTerrainScript * enemyMoveOnTerrainScript = enemy->addComponent<MoveOnTerrainScript>();

	if (isBoss)
		enemyMoveOnTerrainScript->setObjectHeight(150.0f);
	else
		enemyMoveOnTerrainScript->setObjectHeight(10.0f);


	if (isBoss)
		enemy->addComponent<DamageableScript>()->setMaxHp(100.0f);
	else
		enemy->addComponent<DamageableScript>()->setMaxHp(30.0f);

	return enemyMoveOnTerrainScript;
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
	groundTerrain->loadRawFile("terrain.raw", 129, 129, 10.0, .5f);
	//groundTerrain->loadRawFile("terrain.raw", 12, 12, 10.0, .5f);
	
	groundTerrain->loadTextureFromFile("grass.bmp");
	//return groundTerrain;

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



LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
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

	//GameUI gameUI;

	//////////////////////////////////////////////////////////////////////////
	//gameUI.initD3D(Width, Height, true, D3DDEVTYPE_HAL, &device);


	//gameUI.show();
	//layout->addWidget(pDxWidget);
	//w.centralWidget()->setLayout(layout);
	////w.setLayout(layout);

	//pDxWidget->initD3D();
	//pDxWidget->StartRenderFunc(30);
	//pDxWidget->show();

	//////////////////////////////////////////////////////////////////////////

	/*
	서비스 등록
	*/
	Locator::provideScene(Locator::SystemType::RELEASETYPE, Locator::SceneType::START);
	// 디바이스를 여기서 초기화시켜줘야한다 // 더미객체 하나가 startScene에 남아있다.
	GameObject * dummy = GameObject::Instantiate("dummy", "Dummy");
	Locator::provideGameUI(&device, dummy);

	Locator::provideAudio(Locator::SystemType::RELEASETYPE);
	Locator::providePhysics(Locator::SystemType::RELEASETYPE);
	Locator::provideFbxParser(Locator::SystemType::RELEASETYPE);
	Locator::provideDevice(device);

	// Gizmos클래스 초기화
	Gizmos::InitGizmos(device);

	// 디버깅용 txt파일을 로드
	Trace::LoadFileNames();
	Trace::Clear("TAG_DEBUG");



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
	D3DLIGHT9 light = InitDirectionalLight(&dir, &col);

	device->SetLight(0, &light);
	device->LightEnable(0, true);

	device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	device->SetRenderState(D3DRS_SPECULARENABLE, true);

	//device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);


	SetStartScene();
	SetEndScene();


	// mainScene
	Locator::provideScene(Locator::SystemType::RELEASETYPE, Locator::SceneType::MAIN);
	//Scene의 카메라와 기본 월드 세팅을 해준다.
	GameObject * mainCamera = GameObject::Instantiate("mainCamera", "MainCamera");
	mainCamera->addComponent<MainCamera>();

	//mainCamera->getTransform()->setWorldPosition(0, 20, 10);
	//mainCamera->getTransform()->setWorldRotation(0, 180, 0);
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

	// player children
	player->addChild(mainCamera);
	GameObject * bulletSpawner = GameObject::Instantiate("bulletSpawner", "BulletSpawner");
	BulletController * bulletSpawnerBulletController = bulletSpawner->addComponent<BulletController>();
	bulletSpawner->getTransform()->setLocalPosition(0, 6, 0);
	bulletSpawner->getTransform()->setLocalRotation(0, 0, 0);
	player->addChild(bulletSpawner);

	// player components
	player->addComponent<PlayerScript>();
	RigidBody * playerRigidBody = player->addComponent<RigidBody>();
	playerRigidBody->setSphereCollider(10);
	playerRigidBody->setGravity(Vector3(0, 5, 0));
	//playerRigidBody->turnOnStaticFlag();
	//playerRigidBody->turnOnIsTriggerFlag();

	FbxModelRenderer * fbxModelRendererPlayer = player->addComponent<FbxModelRenderer>();
	fbxModelRendererPlayer->loadFbxFile("akai_e_espiritu.fbx");
	PlayerAnimationFSM * playerAnimationFSM = player->addComponent<PlayerAnimationFSM>();

	MoveOnTerrainScript * playerMoveOnTerrainScript = player->addComponent<MoveOnTerrainScript>();
	//car1MoveOnTerrainScript->setTerrain(groundTerrain);

	playerMoveOnTerrainScript->setObjectHeight(3.0f);
	player->getTransform()->setWorldPosition(-544, 3, 374);
	player->getTransform()->setWorldRotation(0, 0, 0);

	player->addComponent<DamageableScript>()->setMaxHp(30000.0f);



	// Enemys
	MoveOnTerrainScript * enemy1MoveOnTerrainScript = GenerateBasicEnemy(Vector3(-202,3,-305) , true);
	MoveOnTerrainScript * enemy2MoveOnTerrainScript = GenerateBasicEnemy(Vector3(148,3,-375));
	MoveOnTerrainScript * enemy3MoveOnTerrainScript = GenerateBasicEnemy(Vector3(477,3,-475));
	MoveOnTerrainScript * enemy4MoveOnTerrainScript = GenerateBasicEnemy(Vector3(395,3,-288));
	MoveOnTerrainScript * enemy5MoveOnTerrainScript = GenerateBasicEnemy(Vector3(425,4,106));
	MoveOnTerrainScript * enemy6MoveOnTerrainScript = GenerateBasicEnemy(Vector3(457, 3, 450));



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
	//enemy1MoveOnTerrainScript->setTerrain(groundTerrain);
	enemy2MoveOnTerrainScript->setTerrain(groundTerrain);
	enemy3MoveOnTerrainScript->setTerrain(groundTerrain);
	enemy4MoveOnTerrainScript->setTerrain(groundTerrain);
	enemy5MoveOnTerrainScript->setTerrain(groundTerrain);
	enemy6MoveOnTerrainScript->setTerrain(groundTerrain);



	// 하드코딩 스타트지점 맞추기------------------------------------------------------------------------------
	//float tempHeight = 0.0f;
	//groundTerrain->getHeight(enemy->getTransform()->getWorldPosition(), &tempHeight);
	//const Vector3 & startPoint = enemyBasicEnemyScript->getStartPoint();
	//enemyBasicEnemyScript->setStartPoint(Vector3{ startPoint.getX() , tempHeight ,startPoint.getZ() });
	//---------------------------------------------------------------------------------------------------------




	// 클래스 멤버함수의 함수포인터는 또 다른식으로 정의해줘야한다.
	// 일단 클래스 명으로 지정 / 넘길때도 &을 붙여서 넘겨줌 / 사용할때는 그 클래스의 객체 기준으로 사용
	EnterMsgLoop( &Scene::gameLoop/*&(Scene::getInstance()->gameLoop)*/, *dummy);


	// void(*ptr_display)(void)

	a.exit();
	Locator::release();
	device->Release();




	return 0;
}