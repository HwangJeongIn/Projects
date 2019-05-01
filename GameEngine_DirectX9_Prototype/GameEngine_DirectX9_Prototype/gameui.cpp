#include "gameui.h"
#include "GameObject.h"
#include "Locator.h"
#include "Scene.h"
#include "Audio.h"
#include "Utility.h"

const string GameUI::filePathToLoadImageFiles = "../Qt/";

GameUI::GameUI(QWidget *parent)
	: QMainWindow(parent), objToAccessSystem(nullptr), maxPlayerHp(1000.0f), score(0.0f)
{
	ui.setupUi(this);
	currentPlayerHp = maxPlayerHp;
}

GameUI::~GameUI()
{

}

void GameUI::setUI()

{
	//label

	//ui.main_line->setStyleSheet("color: green; background: rgba( 10, 0, 50, 0);");
	//ui.main_line->setAutoFillBackground(true);

	//ui.label->setStyleSheet("color: green; background: none;");
	//ui.label->setAutoFillBackground(true);

	ui.lineEdit_1->setVisible(false);
	ui.lineEdit_2->setVisible(false);
	ui.lineEdit_3->setVisible(false);
	ui.lineEdit_4->setVisible(false);


	// 이미지 로드
	m_img[0].load((filePathToLoadImageFiles + "StartSceneImage.png").c_str());
	m_img[1].load((filePathToLoadImageFiles + "EndSceneImage.png").c_str());
	m_img[2].load((filePathToLoadImageFiles + "BossImage.png").c_str());

	ui.start_label->setPixmap(m_img[0].scaled(ui.start_label->width(), ui.start_label->height(), Qt::IgnoreAspectRatio/* Qt::KeepAspectRatio*/));
	ui.end_label->setPixmap(m_img[1].scaled(ui.end_label->width(), ui.end_label->height(), Qt::IgnoreAspectRatio/*Qt::KeepAspectRatio*/));
	ui.main_label_bossImage->setPixmap(m_img[2].scaled(ui.main_label_bossImage->width(), ui.main_label_bossImage->height(), Qt::IgnoreAspectRatio/* Qt::KeepAspectRatio*/));

	setMainSceneUI();
	//setAttribute(Qt::WA_PaintOnScreen, true); // 백버퍼에 그리는 대신 스크린에 그리도록 설정
}


/*
// global
QLabel *labelForDX;
QVBoxLayout *verticalLayout;
QPushButton *pushButton_sound;
QPushButton *pushButton_gizmos;
QPushButton *pushButton;
QPushButton *pushButton_2;
QPushButton *pushButton_3;


// main
QFrame *main_line_hp;
QLabel *main_label_score;
QLabel *main_label_bossImage;

// start
QLabel *startEnd_label_title;
QLabel *start_label;
QVBoxLayout *start_verticalLayout_button;
QPushButton *start_pushButton_start;
QPushButton *start_pushButton_load;

// end
QLabel *startEnd_label_title;
QLabel *end_label;
QVBoxLayout *end_verticalLayout_button;
QPushButton *end_pushButton_restart;
*/

float GameUI::getCurrentPlayerHpFactor()
{
	return currentPlayerHp / maxPlayerHp;
}

void GameUI::setCurrentPlayerHpFactor(float factor)
{
	if (factor < 0)
		factor = 0;
	if (factor > 1)
		factor = 1;

	currentPlayerHp = maxPlayerHp * factor;
	ui.main_line_hp->setFixedWidth(currentPlayerHp);
}

void GameUI::addScore(float value)
{
	if (value < 0) return;
	score += value;
}

void GameUI::setMainSceneUI()
{

	ui.checkBox_sound->setVisible(true);
	ui.checkBox_gizmos->setVisible(true);
	ui.labelForDX->setVisible(true);


	ui.main_line_hp->setVisible(true);
	ui.main_label_score->setVisible(true);
	ui.main_label_bossImage->setVisible(true);



	ui.startEnd_label_title->setVisible(false);

	ui.start_label->setVisible(false);
	ui.start_pushButton_start->setVisible(false);
	ui.start_pushButton_load->setVisible(false);

	ui.end_label->setVisible(false);
	ui.end_pushButton_restart->setVisible(false);


}

void GameUI::setStartSceneUI()

{
	ui.checkBox_sound->setVisible(true);
	ui.checkBox_gizmos->setVisible(true);
	ui.labelForDX->setVisible(true);


	ui.main_line_hp->setVisible(false);
	ui.main_label_score->setVisible(false);
	ui.main_label_bossImage->setVisible(false);



	ui.startEnd_label_title->setVisible(true);

	ui.start_label->setVisible(true);
	ui.start_pushButton_start->setVisible(true);
	ui.start_pushButton_load->setVisible(true);

	ui.end_label->setVisible(false);
	ui.end_pushButton_restart->setVisible(false);
}

void GameUI::setEndSceneUI()

{
	ui.checkBox_sound->setVisible(true);
	ui.checkBox_gizmos->setVisible(true);
	ui.labelForDX->setVisible(true);


	ui.main_line_hp->setVisible(false);
	ui.main_label_score->setVisible(false);
	ui.main_label_bossImage->setVisible(false);



	ui.startEnd_label_title->setVisible(true);

	ui.start_label->setVisible(false);
	ui.start_pushButton_start->setVisible(false);
	ui.start_pushButton_load->setVisible(false);

	ui.end_label->setVisible(true);
	ui.end_pushButton_restart->setVisible(true);

}


bool GameUI::initD3D(int width, int height, bool windowed, D3DDEVTYPE deviceType, IDirect3DDevice9 ** device)

{

	HWND hwnd = (HWND)ui.labelForDX->winId();

	HRESULT hr = 0;

	// Step 1: Create the IDirect3D9 object.

	IDirect3D9* d3d9 = 0;
	d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

	if (!d3d9)
	{
		::MessageBox(0, "Direct3DCreate9() - FAILED", 0, 0);
		return false;
	}

	// Step 2: Check for hardware vp.

	D3DCAPS9 caps;
	d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, deviceType, &caps);

	int vp = 0;
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	// Step 3: Fill out the D3DPRESENT_PARAMETERS structure.

	D3DPRESENT_PARAMETERS d3dpp;
	d3dpp.BackBufferWidth = width;
	d3dpp.BackBufferHeight = height;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hwnd;
	d3dpp.Windowed = windowed;
	d3dpp.EnableAutoDepthStencil = true;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.Flags = 0;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	// Step 4: Create the device.

	hr = d3d9->CreateDevice(
		D3DADAPTER_DEFAULT, // primary adapter
		deviceType,         // device type
		hwnd,               // window associated with device
		vp,                 // vertex processing
		&d3dpp,             // present parameters
		device);            // return created device

	if (FAILED(hr))
	{
		// try again using a 16-bit depth buffer
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

		hr = d3d9->CreateDevice(
			D3DADAPTER_DEFAULT,
			deviceType,
			hwnd,
			vp,
			&d3dpp,
			device);

		if (FAILED(hr))
		{
			d3d9->Release(); // done with d3d9 object
			::MessageBox(0, "CreateDevice() - FAILED", 0, 0);
			return false;
		}
	}

	d3d9->Release(); // done with d3d9 object

	setUI();


	return true;
}
void GameUI::start_pushButton_start()
{
	objToAccessSystem->getAudio().playEffectSound("BS_Effect_MainButton.mp3",true);


	Locator::provideScene(Locator::SystemType::RELEASETYPE, Locator::SceneType::MAIN);
	setMainSceneUI();
	// 씬을 초기상태로 되돌린다.
	//Scene & currentScene = objToAccessSystem->getScene();
	//currentScene.find


	//ui.main_label_score->setText("start_pushButton_start");
}

void GameUI::start_pushButton_load()
{
	objToAccessSystem->getAudio().playEffectSound("BS_Effect_MainButton.mp3", true);

	Locator::provideScene(Locator::SystemType::RELEASETYPE, Locator::SceneType::MAIN);
	setMainSceneUI();
	// 씬을 저장상태로 되돌린다.
	//Scene & currentScene = objToAccessSystem->getScene();
	//currentScene.find

	//ui.main_label_score->setText("start_pushButton_load");
}
void GameUI::end_pushButton_restart()
{
	objToAccessSystem->getAudio().playEffectSound("BS_Effect_MainButton.mp3", true);

	Locator::provideScene(Locator::SystemType::RELEASETYPE, Locator::SceneType::START);
	setStartSceneUI();
	//ui.main_label_score->setText("end_pushButton_restart");
}

void GameUI::checkBox_sound(bool value)
{
	objToAccessSystem->getAudio().playEffectSound("BS_Effect_Button.mp3", true);

	if (value)
	{
		Locator::provideAudio(Locator::SystemType::RELEASETYPE);
		//ui.main_line_hp->setFixedWidth(30);
		//ui.main_label_score->setText("pushButton_sound / true");
	}
	else
	{
		Locator::provideAudio(Locator::SystemType::NULLTYPE);
		//ui.main_line_hp->setFixedWidth(1000);
		//ui.main_label_score->setText("pushButton_sound / false");
	}
}
void GameUI::checkBox_gizmos(bool value)
{
	objToAccessSystem->getAudio().playEffectSound("BS_Effect_Button.mp3", true);

	if (value)
	{
		Gizmos::TrunOnGizmos();
	}
	else
	{
		Gizmos::TrunOffGizmos();
	}
}
