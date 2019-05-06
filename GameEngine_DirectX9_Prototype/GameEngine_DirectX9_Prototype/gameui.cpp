#include "gameui.h"
#include "GameObject.h"
#include "Locator.h"
#include "Scene.h"
#include "Audio.h"
#include "Utility.h"



const string GameUI::filePathToLoadImageFiles = "../Qt/";

GameUI::GameUI(QWidget *parent)
	: QMainWindow(parent), objToAccessSystem(nullptr), mainSceneGamePlayManager(nullptr), maxPlayerHp(1000.0f), score(0.0f), currentMain_widget_UIWindowFlag(false),
	countOfenemiesThatAreTargetingPlayer(0), 
	currentTextFileAndPos(TextFileType::TEXTFILE_NONE, 0), lineTag("LNE_"), endTag("END_"), eventTag("ENT_"), tagLength(4),
	numOfEvents(3)
{
	ui.setupUi(this);
	currentPlayerHp = maxPlayerHp;
	setActiveMainWidgetUIWindow(currentMain_widget_UIWindowFlag);


	// 파일을 읽기 위한 TextFileType / textFileName 테이블

	// 하드코딩으로 등록
	fileNameTable.insert(pair<TextFileType, string>(TextFileType::TEXTFILE_TUTORIAL, "_TutorialStage.txt"));
	fileNameTable.insert(pair<TextFileType, string>(TextFileType::TEXTFILE_ONE, "_OneStage.txt"));
	fileNameTable.insert(pair<TextFileType, string>(TextFileType::TEXTFILE_BOSS, "_BossStage.txt"));


	// 스타일 시트 정하기
	//ui.main_label_score->setStyleSheet("color: rgb(255, 129, 3); font-weight: bold; ");
	//ui.label_text->setStyleSheet("font-weight: bold; font: 18pt \"맑은 고딕\"; ");
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
QPushButton *start_pushButton_tutorial;

// end
QLabel *startEnd_label_title;
QLabel *end_label;
QVBoxLayout *end_verticalLayout_button;
QPushButton *end_pushButton_restart;
*/

//GamePlayManager * GameUI::getGamePlayManager()
//{
//	GameObject * mainCamera = objToAccessSystem->getScene().getMainCamera();
//	if (!mainCamera) return nullptr;
//	GamePlayManager * mainCameraGamePlayManager = mainCamera->getComponent<GamePlayManager>();
//	if (!mainCameraGamePlayManager) return nullptr;
//	return mainCameraGamePlayManager;
//}

GameUI::EventType GameUI::readTextFile(TextFileType textFileType, bool isBeginning)
{
	EventType resultEvent = EventType::EVENT_NONE;
	// 먼저 메시지를 줘야하기 때문에 게임플레이매니저부터 받아온다

	if (!mainSceneGamePlayManager) return resultEvent;
	//GamePlayManager * mainCameraGamePlayManager = getGamePlayManager();
	//if (!mainCameraGamePlayManager) return resultEvent;

	// 만약에 제대로 등록안되었다면 리턴
	auto it = fileNameTable.find(textFileType);
	if (it == fileNameTable.end()) return resultEvent;

	string textFileName = (*it).second;

	textFileStream.open(filePathToLoadImageFiles + textFileName);
	// 안열리면 리턴
	if (textFileStream.is_open() == false) return resultEvent;

	// 만약에 들어온 타입과 현재 읽던 타입이 같다면 > 이어서 // 단 isBeginning 플래그가 켜져있으면 처음부터
	// 만약 들어온타입과 현재 읽던 타입이 다르다면 > 처음부터
	if (currentTextFileAndPos.first != textFileType || isBeginning != false)
	{
		currentTextFileAndPos.second = 0;
	}

	// 현재 파일타입을 들어온타입으로 바꿔준다.
	currentTextFileAndPos.first = textFileType;

	// 현재 읽던 위치를 맞춰준다. 
	// 0으로 초기화되었을수도 있다(isBeginning 플래그 true / 다른타입의 텍스트파일을 읽으라는 표시가 있을때)
	textFileStream.seekg(currentTextFileAndPos.second);

	string line = "";
	// 한줄을 읽어주는데, 읽을게 없으면 리턴
	if (!getline(textFileStream, line)) return resultEvent;

	string tag = line.substr(0, tagLength);
	string info = "";

	if (tag == lineTag)
	{
		info = line.substr(tagLength, line.size() - tagLength);
		setTextWindowText(info.c_str());
		setActiveMainWidgetTextWindow(true);
		//mainCameraGamePlayManager->setUnreadMessage(true);
	}
	else if (tag == eventTag)
	{
		// 이벤트를 읽고 반환할 이벤트를 결정해준다.
		// 태그 기준으로 1개의 문자를 읽는데 이는 숫자문자이다.
		// 이 숫자문자를 숫자로 변환하여 이벤트와 1:1매칭시킨다.
		info = line.substr(tagLength, 1);
		int eventType = info[0] - '0';

		// 만약에 이벤트 수보다 크면 이벤트 타입은 NONE이 된다.
		if (eventType > numOfEvents)
			eventType = 0;

		resultEvent = (EventType)eventType;

		// 다음 LineTag가 나올때까지 읽는다
		// 작성할때 이벤트 + 내용으로 작성하면된다.
		while (getline(textFileStream, line))
		{
			tag = line.substr(0, tagLength);
			if (tag == lineTag)
			{
				info = line.substr(tagLength, line.size() - tagLength);
				break;
			}

		}
		setTextWindowText(info.c_str());
		setActiveMainWidgetTextWindow(true);
		//mainCameraGamePlayManager->setUnreadMessage(true);

	}
	else if (tag == endTag)
	{
		resultEvent = EVENT_T_ENDEVENT;
		// 텍스트를 변경하지 않고 그냥 활성화 시킨다.
		setActiveMainWidgetTextWindow(false);
		//mainCameraGamePlayManager->setUnreadMessage(false);
	}
	else
	{
		// 그외의 상황 // 태그가 없는 상황 // 마지막일수도있고 // 텍스트파일에 태그를 안넣은것일수도있다
		setActiveMainWidgetTextWindow(true);
		//mainCameraGamePlayManager->setUnreadMessage(true);
	}


	// 마지막으로 파일위치를 최신화시켜준다.
	currentTextFileAndPos.second = textFileStream.tellg();
	// 닫아준다.
	textFileStream.close();

	return resultEvent;
}

void GameUI::setTextWindowText(const char * info)
{
	if (!info) return;
	QString korStr = QString::fromLocal8Bit(info);
	ui.label_text->setText(korStr);
}


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
	string temp = "Score : ";
	temp += MyUtility::ToStr<float>(score);
	ui.main_label_score->setText(temp.c_str());
}

void GameUI::resetScore()
{
	score = 0;
	string temp = "Score : ";
	temp += MyUtility::ToStr<float>(score);
	ui.main_label_score->setText(temp.c_str());
}

void GameUI::setMainSceneUI()
{
	ui.labelForDX->setVisible(true);
	setActiveMainWidgetUIWindow(false);
	//ui.checkBox_sound->setVisible(true);
	//ui.checkBox_gizmos->setVisible(true);
	setActiveMainWidgetTextWindow(false);
	setActiveStartWidgetInfoWindow(false);


	ui.main_line_hp->setVisible(true);
	ui.main_label_score->setVisible(true);
	ui.main_label_bossImage->setVisible(false);



	ui.startEnd_label_title->setVisible(false);

	ui.start_label->setVisible(false);
	ui.start_pushButton_start->setVisible(false);
	ui.start_pushButton_tutorial->setVisible(false);
	ui.start_pushButton_info->setVisible(false);

	ui.end_label->setVisible(false);
	ui.end_pushButton_restart->setVisible(false);


}

void GameUI::setStartSceneUI()
{
	ui.labelForDX->setVisible(true);
	setActiveMainWidgetUIWindow(false);
	//ui.checkBox_sound->setVisible(true);
	//ui.checkBox_gizmos->setVisible(true);
	setActiveMainWidgetTextWindow(false);
	setActiveStartWidgetInfoWindow(false);


	ui.main_line_hp->setVisible(false);
	ui.main_label_score->setVisible(false);
	ui.main_label_bossImage->setVisible(false);



	ui.startEnd_label_title->setVisible(true);

	ui.start_label->setVisible(true);
	ui.start_pushButton_start->setVisible(true);
	ui.start_pushButton_tutorial->setVisible(true);
	ui.start_pushButton_info->setVisible(true);

	ui.end_label->setVisible(false);
	ui.end_pushButton_restart->setVisible(false);
}

void GameUI::setEndSceneUI()
{
	ui.labelForDX->setVisible(true);
	setActiveMainWidgetUIWindow(false);
	//ui.checkBox_sound->setVisible(true);
	//ui.checkBox_gizmos->setVisible(true);
	setActiveMainWidgetTextWindow(false);
	setActiveStartWidgetInfoWindow(false);


	ui.main_line_hp->setVisible(false);
	ui.main_label_score->setVisible(false);
	ui.main_label_bossImage->setVisible(false);



	ui.startEnd_label_title->setVisible(true);

	ui.start_label->setVisible(false);
	ui.start_pushButton_start->setVisible(false);
	ui.start_pushButton_tutorial->setVisible(false);
	ui.start_pushButton_info->setVisible(false);


	ui.end_label->setVisible(true);
	ui.end_pushButton_restart->setVisible(true);

}

void GameUI::setBossImage(bool flag)
{
	ui.main_label_bossImage->setVisible(flag);
}

void GameUI::setActiveMainWidgetUIWindow(bool flag)
{
	ui.main_widget_UIWindow->setVisible(flag);
	currentMain_widget_UIWindowFlag = flag;
}

void GameUI::toggleMainWidgetUIWindow()
{
	currentMain_widget_UIWindowFlag = !currentMain_widget_UIWindowFlag;
	ui.main_widget_UIWindow->setVisible(currentMain_widget_UIWindowFlag);
}

void GameUI::setActiveMainWidgetTextWindow(bool flag)
{
	ui.main_widget_textWindow->setVisible(flag);

	if (!mainSceneGamePlayManager) return;
	mainSceneGamePlayManager->setUnreadMessage(flag);

	//GamePlayManager * mainCameraGamePlayManager = getGamePlayManager();
	//if (!mainCameraGamePlayManager) return;
	//mainCameraGamePlayManager->setUnreadMessage(flag);


}

void GameUI::setActiveStartWidgetInfoWindow(bool flag)
{
	ui.start_widget_infoWindow->setVisible(flag);
}

void GameUI::addEnemyThatIsTargetingPlayer()
{
	if (!objToAccessSystem) return;

	++countOfenemiesThatAreTargetingPlayer;
}

void GameUI::removeEnemyThatIsNotTargetingPlayer()
{
	if (!objToAccessSystem) return;

	--countOfenemiesThatAreTargetingPlayer;
	// 원래 음악을 튼다.
	if (countOfenemiesThatAreTargetingPlayer <= 0)
	{
		countOfenemiesThatAreTargetingPlayer = 0;
		objToAccessSystem->getAudio().playBackGroundSound("BS_BackGround_MainScene.mp3");
	}

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
	if (!objToAccessSystem) return;
	objToAccessSystem->getAudio().playEffectSound("BS_Effect_MainButton.mp3",true);

	//objToAccessSystem->getScene().registerOtherSceneToChange(Locator::SceneType::MAIN);
	Locator::provideScene(Locator::SystemType::RELEASETYPE, Locator::SceneType::MAIN);
	setMainSceneUI();


	// 스테이지 1로 바꿔준다.
	// 플레이어를 스테이지 1으로 보낸다.

	if (!mainSceneGamePlayManager)return;
	mainSceneGamePlayManager->changeStage(GamePlayManager::StageType::STAGE_ONE);
	mainSceneGamePlayManager->movePlayerToStageStartPoint(GamePlayManager::StageType::STAGE_ONE);
	//GamePlayManager * mainCameraGamePlayManager = getGamePlayManager();
	//if (!mainCameraGamePlayManager) return;

	//mainCameraGamePlayManager->changeStage(GamePlayManager::StageType::STAGE_ONE);
	//mainCameraGamePlayManager->movePlayerToStageStartPoint(GamePlayManager::StageType::STAGE_ONE);
}

void GameUI::start_pushButton_tutorial()
{
	if (!objToAccessSystem) return;
	objToAccessSystem->getAudio().playEffectSound("BS_Effect_MainButton.mp3", true);


	//objToAccessSystem->getScene().registerOtherSceneToChange(Locator::SceneType::MAIN);
	Locator::provideScene(Locator::SystemType::RELEASETYPE, Locator::SceneType::MAIN);
	setMainSceneUI();


	// 스테이지 튜토리얼로 바꿔준다.
	// 플레이어를 스테이지 튜토리얼으로 보낸다.

	if (!mainSceneGamePlayManager)return;
	mainSceneGamePlayManager->changeStage(GamePlayManager::StageType::STAGE_TUTORIAL);
	mainSceneGamePlayManager->movePlayerToStageStartPoint(GamePlayManager::StageType::STAGE_TUTORIAL);

	//GamePlayManager * mainCameraGamePlayManager = getGamePlayManager();
	//if (!mainCameraGamePlayManager) return;

	//mainCameraGamePlayManager->changeStage(GamePlayManager::StageType::STAGE_TUTORIAL);
	//mainCameraGamePlayManager->movePlayerToStageStartPoint(GamePlayManager::StageType::STAGE_TUTORIAL);

}

//void GameUI::start_pushButton_tutorial()
//{
//	if (!objToAccessSystem) return;
//	objToAccessSystem->getAudio().playEffectSound("BS_Effect_MainButton.mp3", true);
//
//	Locator::provideScene(Locator::SystemType::RELEASETYPE, Locator::SceneType::MAIN);
//	setMainSceneUI();
//
//
//
//
//	// 씬을 저장상태로 되돌린다.
//	//Scene & currentScene = objToAccessSystem->getScene();
//	//currentScene.find
//
//	//ui.main_label_score->setText("start_pushButton_tutorial");
//}
void GameUI::end_pushButton_restart()
{
	if (!objToAccessSystem) return;
	objToAccessSystem->getAudio().playEffectSound("BS_Effect_MainButton.mp3", true);

	//objToAccessSystem->getScene().registerOtherSceneToChange(Locator::SceneType::START);
	Locator::provideScene(Locator::SystemType::RELEASETYPE, Locator::SceneType::START);
	setStartSceneUI();
	//ui.main_label_score->setText("end_pushButton_restart");
}

void GameUI::checkBox_sound(bool value)
{
	if (!objToAccessSystem) return;
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
	if (!objToAccessSystem) return;
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

void GameUI::checkBox_wireFrame(bool value)
{
	if (!objToAccessSystem) return;

	objToAccessSystem->getAudio().playEffectSound("BS_Effect_Button.mp3", true);
	// 디바이스를 받아서 설정해준다.
	if (value)
	{
		objToAccessSystem->getDevice().SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}
	else
	{
		objToAccessSystem->getDevice().SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}
}

void GameUI::checkBox_fbxRendering(bool value)
{
	if (!objToAccessSystem) return;

	objToAccessSystem->getAudio().playEffectSound("BS_Effect_Button.mp3", true);

	if (!mainSceneGamePlayManager) return;

	if (value)
		mainSceneGamePlayManager->setAllStage();
	else
		mainSceneGamePlayManager->resetAllStage();

	//GamePlayManager * mainCameraGamePlayManager = getGamePlayManager();
	//if (!mainCameraGamePlayManager) return;

	//if (value)
	//	mainCameraGamePlayManager->setAllStage();
	//else
	//	mainCameraGamePlayManager->resetAllStage();
}

void GameUI::checkBox_playerRendering(bool value)
{
	if (!objToAccessSystem) return;

	objToAccessSystem->getAudio().playEffectSound("BS_Effect_Button.mp3", true);


	if (!mainSceneGamePlayManager) return;


	mainSceneGamePlayManager->setActivePlayerRendering(value);


	// 먼저 메인카메라를 받는다.
	//GamePlayManager * mainCameraGamePlayManager = getGamePlayManager();
	//if (!mainCameraGamePlayManager) return;


	//mainCameraGamePlayManager->setActivePlayerRendering(value);

}

void GameUI::checkBox_currentStage(bool value)
{
	if (!objToAccessSystem) return;

	objToAccessSystem->getAudio().playEffectSound("BS_Effect_Button.mp3", true);
	// 먼저 메인카메라를 받는다.

	if (!mainSceneGamePlayManager) return;

	if (value)
		mainSceneGamePlayManager->setCurrentStage();
	else
		mainSceneGamePlayManager->resetCurrentStage();
}

void GameUI::pushButton_text()
{
	if (!objToAccessSystem) return;

	objToAccessSystem->getAudio().playEffectSound("BS_Effect_Button.mp3", true);
	setActiveMainWidgetTextWindow(false);


	//readTextFile(TextFileType::TEXTFILE_TUTORIAL);
}

void GameUI::start_pushButton_info()
{
	if (!objToAccessSystem) return;

	objToAccessSystem->getAudio().playEffectSound("BS_Effect_Button.mp3", true);

	setActiveStartWidgetInfoWindow(true);
}

void GameUI::start_pushButton_infoExit()
{
	if (!objToAccessSystem) return;

	objToAccessSystem->getAudio().playEffectSound("BS_Effect_Button.mp3", true);

	setActiveStartWidgetInfoWindow(false);
}
