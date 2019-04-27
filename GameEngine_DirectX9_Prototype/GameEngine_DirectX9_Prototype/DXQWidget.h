#ifndef DXQWIDGET_H
#define DXQWIDGET_H

#include <d3dx9.h>
#include <string>

#include <QtWidgets/qwidget.h>
#include <QtCore/qtimer.h>

class DXQWidget : public QWidget
{
	Q_OBJECT
	//	Q_DISABLE_COPY(DXQWidget)

public:

	void frame()
	{

	}

	bool initD3D(HINSTANCE hInstance,
		int width, int height,
		bool windowed,
		D3DDEVTYPE deviceType,
		IDirect3DDevice9** device)
	{
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
		d3dpp.hDeviceWindow = (HWND)winId();
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
			(HWND)winId(),               // window associated with device
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
				(HWND)winId(),
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

		return true;
	}

	DXQWidget(QWidget *parent = 0, Qt::WindowFlags flags = 0);

	//paintEngine should return NULL for painting manually in the window
	inline QPaintEngine* paintEngine()
	{
		return NULL;
	}
	void StartRenderFunc(int millisec)
	{
		// 일정시간 마다 렌더링하기 위해서 슬롯으로 래핑한 frame함수를 호출한다.
		QObject::connect(&m_renderTimer, SIGNAL(timeout()), this, SLOT(frameFunc()));
		m_renderTimer.start(millisec);
	}

	private slots:
	void frameFunc()
	{
		/*(*m_renderFunc)();*/
		frame();
	}

private:
	/*RenderFuncType m_renderFunc;*/
	QTimer m_renderTimer;


};

#endif // DXWIDGET_H