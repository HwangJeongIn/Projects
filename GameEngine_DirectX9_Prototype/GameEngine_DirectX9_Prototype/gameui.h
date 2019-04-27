#ifndef GAMEUI_H
#define GAMEUI_H

#include <QtWidgets/QMainWindow>
#include "GeneratedFiles/ui_gameui.h"

#include <d3dx9.h>
#include <string>

using namespace std;


class GameUI : public QMainWindow
{
	Q_OBJECT

private :
	Ui::GameUIClass ui;
	QPixmap m_img[2];
	static const string filePathToLoadImageFiles;

public:
	GameUI(QWidget *parent = 0);
	~GameUI();

	void setUI()
	{


		// 이미지 로드
		m_img[0].load((filePathToLoadImageFiles + "temp.png").c_str());
		//m_img[0].load(":/GameUI/tree0.jpg");
		//m_img[0].load((filePathToLoadImageFiles +"tree0.jpg").c_str());
		m_img[1].load((filePathToLoadImageFiles + "temp.png").c_str());
		ui.start_label->setPixmap(m_img[0].scaled(ui.start_label->width(), ui.start_label->height(), Qt::KeepAspectRatio));
		ui.end_label->setPixmap(m_img[1].scaled(ui.end_label->width(), ui.end_label->height(), Qt::KeepAspectRatio));

		setMainSceneUI();
		//setAttribute(Qt::WA_PaintOnScreen, true); // 백버퍼에 그리는 대신 스크린에 그리도록 설정
	}


	void resetUI()
	{

	}

	void setMainSceneUI()
	{

		ui.main_pushButton_1->setVisible(true);
		ui.main_pushButton_2->setVisible(true);
		ui.main_line->setVisible(true);

		ui.start_label->setVisible(false);
		ui.start_pushButton_1->setVisible(false);

		ui.end_pushButton_1->setVisible(false);
		ui.end_label->setVisible(false);


	}
	void setStartSceneUI()
	{
		ui.main_pushButton_1->setVisible(false);
		ui.main_pushButton_2->setVisible(false);
		ui.main_line->setVisible(false);

		ui.start_label->setVisible(true);
		ui.start_pushButton_1->setVisible(true);

		ui.end_pushButton_1->setVisible(false);
		ui.end_label->setVisible(false);
	}
	void setEndSceneUI()
	{
		ui.main_pushButton_1->setVisible(false);
		ui.main_pushButton_2->setVisible(false);
		ui.main_line->setVisible(false);

		ui.start_label->setVisible(false);
		ui.start_pushButton_1->setVisible(false);

		ui.end_pushButton_1->setVisible(true);
		ui.end_label->setVisible(true);


	}

	void showUI()
	{
		//setAutoFillBackground(true);
		//ui.main_line->setAutoFillBackground(true);
		//ui.main_pushButton_1->setAutoFillBackground(true);
		//ui.main_pushButton_2->setAutoFillBackground(true);
		//auto palette = ui.main_line->palette();
		//palette.setColor(QPalette::Window, QColor(100, 100, 0, 0));
		//ui.main_line->setPalette(palette);
		//ui.main_line->setStyleSheet("background - color: none");

		/*
		ui.main_line->setStyleSheet("color: green; background: rgba( 10, 0, 50, 0);");
		ui.main_line->setAutoFillBackground(true);

		ui.main_pushButton_1->setStyleSheet("color: green; background: black;");
		ui.main_pushButton_1->setAutoFillBackground(true);

		ui.main_pushButton_2->setStyleSheet("color: green; background: black;");
		ui.main_pushButton_2->setAutoFillBackground(true);
		*/


		//ui.main_line->update();
		//ui.main_pushButton_1->update();
		//ui.main_pushButton_2->update();
		//ui.widgetForDX->update();

			//auto editorPalette = palette();
			//editorPalette.setColor(QPalette::Active, QPalette::Base, Qt::transparent);
			//editorPalette.setColor(QPalette::Inactive, QPalette::Base, Qt::transparent);
			//setPalette(editorPalette);
		//ui.main_line->setStyleSheet("background:transparent");
		//ui.main_line->setAttribute(Qt::WA_TranslucentBackground);
		//ui.main_line->setWindowFlags(Qt::FramelessWindowHint);

		//ui.main_line->setPalette((QPalette::WindowText, Qt::transparent));
		//ui.main_line->setStyleSheet("background-color: rgba( 255, 255, 255, 0% );");
		//ui.main_line->palette().setColor()
		//ui.main_line->setAutoFillBackground(false);
		//setAutoFillBackground(false);



		//setAttribute(Qt::WA_NoSystemBackground);
		//setAttribute(Qt::WA_NoBackground);


		//ui.widgetForDX->update();
		//ui.main_pushButton_1->close();
		//ui.main_pushButton_2->show();
		//ui.main_line->show();

		//ui.start_graphicsView->setEnabled(false);
		//ui.start_pushButton_1->setEnabled(false);

		//ui.end_pushButton_1->setEnabled(true);
		//ui.end_graphicsView->setEnabled(true);
	}

	bool initD3D
	(   int width, int height,
		bool windowed,
		D3DDEVTYPE deviceType,
		IDirect3DDevice9** device)
	{

		HWND hwnd = (HWND)ui.widgetForDX->winId();

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

private slots:
	void slot1();
	void slot2();


};

#endif // GAMEUI_H
