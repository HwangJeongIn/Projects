//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: cameraApp.cpp
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Demonstrates using the Camera class.
//         
//////////////////////////////////////////////////////////////////////////////////////////////////

// 에러코드 무시
//#pragma warning(disable:4244) // '초기화 중': 'float'에서 'int'(으)로 변환하면서 데이터가 손실될 수 있습니다.
//#pragma warning(disable:4018) // '<': signed 또는 unsigned가 일치하지 않습니다.

#include "d3dUtility.h"
#include "camera.h"
#include "Locator.h"
#include "Scene.h"

#include "Physics.h"
//
// Globals
//


const int Width  = 1280;
const int Height = 720;

Camera TheCamera(Camera::LANDOBJECT);

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

void Cleanup()
{
	// pass 0 for the first parameter to instruct cleanup.
	d3d::DrawBasicScene(0, 0.0f);
}
/*
bool Display(float timeDelta)
{
	if( Device )
	{
		//
		// Update: Update the camera.
		//

		if( ::GetAsyncKeyState('W') & 0x8000f )
			TheCamera.walk(4.0f * timeDelta);

		if( ::GetAsyncKeyState('S') & 0x8000f )
			TheCamera.walk(-4.0f * timeDelta);

		if( ::GetAsyncKeyState('A') & 0x8000f )
			TheCamera.strafe(-4.0f * timeDelta);

		if( ::GetAsyncKeyState('D') & 0x8000f )
			TheCamera.strafe(4.0f * timeDelta);

		if( ::GetAsyncKeyState('R') & 0x8000f )
			TheCamera.fly(4.0f * timeDelta);

		if( ::GetAsyncKeyState('F') & 0x8000f )
			TheCamera.fly(-4.0f * timeDelta);

		if( ::GetAsyncKeyState(VK_UP) & 0x8000f )
			TheCamera.pitch(1.0f * timeDelta);

		if( ::GetAsyncKeyState(VK_DOWN) & 0x8000f )
			TheCamera.pitch(-1.0f * timeDelta);

		if( ::GetAsyncKeyState(VK_LEFT) & 0x8000f )
			TheCamera.yaw(-1.0f * timeDelta);
			
		if( ::GetAsyncKeyState(VK_RIGHT) & 0x8000f )
			TheCamera.yaw(1.0f * timeDelta);

		if( ::GetAsyncKeyState('N') & 0x8000f )
			TheCamera.roll(1.0f * timeDelta);

		if( ::GetAsyncKeyState('M') & 0x8000f )
			TheCamera.roll(-1.0f * timeDelta);

		// Update the view matrix representing the cameras 
        // new position/orientation.
		D3DXMATRIX V;
		TheCamera.getViewMatrix(&V);
		Device->SetTransform(D3DTS_VIEW, &V);

		//
		// Render
		//

		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
		Device->BeginScene();

		d3d::DrawBasicScene(Device, 1.0f);

		Device->EndScene();
		Device->Present(0, 0, 0, 0);
	}
	return true;
}
*/
//
// WndProc
//
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


	if(!d3d::InitD3D(hinstance,
		Width, Height, true, D3DDEVTYPE_HAL, &device))
	{
		::MessageBox(0, "InitD3D() - FAILED", 0, 0);
		return 0;
	}
		
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
	



	/*
	Scene의 카메라와 기본 월드 세팅을 해준다.
	*/
	GameObject * mainCamera = GameObject::Instantiate("mainCamera", "MainCamera");
	mainCamera->addComponent<MainCamera>();
	mainCamera->getTransform()->setPosition(0, 0, -100);
	mainCamera->getTransform()->setRotation(0, 0, 0);



	// remover
	GameObject * remover = GameObject::Instantiate("remover", "Remover");
	//ground->addComponent<MeshRenderer>()->loadXFile("car.x");
	remover->addComponent<MeshRenderer>()->loadXFile("bigship1.x");
	RigidBody * removerRigidBody = remover->addComponent<RigidBody>();
	remover->getTransform()->setPosition(0, 0, 0);
	removerRigidBody->setBoxCollider(Vector3{ 4, 4, 4 });
	//removerRigidBody->turnOnIsTriggerFlag();
	//removerRigidBody->turnOnStaticFlag();

	// remover2
	GameObject * remover2 = GameObject::Instantiate("remover2", "Remover");
	//ground->addComponent<MeshRenderer>()->loadXFile("car.x");
	remover2->addComponent<MeshRenderer>()->loadXFile("bigship1.x");
	RigidBody * remover2RigidBody = remover2->addComponent<RigidBody>();
	remover2->getTransform()->setPosition(10, 0, 0);
	remover2RigidBody->setBoxCollider(Vector3{ 4, 4, 4 });
	remover2RigidBody->turnOnIsTriggerFlag();
	remover2RigidBody->turnOnStaticFlag();


	// ground
	GameObject * ground = GameObject::Instantiate("ground", "Ground");
	RigidBody * groundRigidBody = ground->addComponent<RigidBody>();
	ground->getTransform()->setPosition(0, -10, 0);
	ground->getTransform()->setRotation(0, 30, 0);
	groundRigidBody->setBoxCollider(Vector3{ 200,10,200 });
	groundRigidBody->turnOnStaticFlag();
	Terrain * groundTerrain = ground->addComponent<Terrain>();
	// adsfasfdSsabgaw.raw / coastMountain64.raw / Ash.raw / adsfsaf.raw
	groundTerrain->loadRawFile("coastMountain64.raw", 64, 64, 20, .5f);
	groundTerrain->loadTextureFromFile("grass.bmp");
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


	GameObject * car1 = GameObject::Instantiate("car1", "Car");
	car1->addComponent<MeshRenderer>()->loadXFile("car.x");
	car1->addComponent<PlayerScript>();
	RigidBody * car1RigidBody = car1->addComponent<RigidBody>();
	car1RigidBody->setSphereCollider(2);

	MoveOnTerrainScript * car1MoveOnTerrainScript = car1->addComponent<MoveOnTerrainScript>();
	car1MoveOnTerrainScript->setTerrain(groundTerrain);
	//car1RigidBody->setGravity(Vector3(0, 0, 0));
	//car1->addComponent<BoxCollider>();
	car1->getTransform()->setPosition(-15, 10, 0);
	car1->getTransform()->setRotation(0, 180, 0);

	// 1단계
	GameObject * car1Child1 = car1->addChild("bigShip1", "BigShip");
	car1Child1->addComponent<MeshRenderer>()->loadXFile("bigship1.x");
	car1Child1->getTransform()->setPosition(5, 3, 0);
	car1Child1->addComponent<MoveScript_C>();


	//GameObject * car1Child2 = car1->addChild("bigShip2", "BigShip");
	//car1Child2->addComponent<MeshRenderer>()->loadXFile("bigship1.x");
	//car1Child2->getTransform()->setPosition(10, 0, 0);

	// 2단계
	
	/*GameObject * car1Child1Child1 = car1Child1->addChild("car3", "Car");
	car1Child1Child1->addComponent<MeshRenderer>()->loadXFile("car.x");
	car1Child1Child1->getTransform()->setPosition(0, 5, 0);

	GameObject * car1Child1Child2 = car1Child1->addChild("car4", "Car");
	car1Child1Child2->addComponent<MeshRenderer>()->loadXFile("car.x");
	car1Child1Child2->getTransform()->setPosition(7, 0, 0);*/
	
	Scene & scene = mainCamera->getScene();

	// 클래스 멤버함수의 함수포인터는 또 다른식으로 정의해줘야한다.
	// 일단 클래스 명으로 지정 / 넘길때도 &을 붙여서 넘겨줌 / 사용할때는 그 클래스의 객체 기준으로 사용
	d3d::EnterMsgLoop( &Scene::gameLoop/*&(Scene::getInstance()->gameLoop)*/,scene );

	// void(*ptr_display)(void)
	Cleanup();

	Locator::release();
	device->Release();

	return 0;
}
