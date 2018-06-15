#include "D3DApp.h"
#include <sstream>

#define WIDTH  640
#define HEIGHT 480

LPDIRECT3DDEVICE9 gd3dDevice = 0;
D3DApp*			  gd3dApp	 = 0;

const char szClassName[] = "DX APPLICATION DEMO";

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) 
{
	if (gd3dApp)
	{
		return gd3dApp->WndProc(msg, wParam, lParam);
	}
	else
	{
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

D3DApp::D3DApp(HINSTANCE hInstance, std::string caption, DWORD requestedVP, D3DDEVTYPE devType)
{
	mhInstance	 = hInstance;
	mCaption	 = caption;
	mRequestedVP = requestedVP;
	mDevType	 = devType;
	mAppPaused	 = false;

	InitMainWindow();
	InitDirectX();
}

void D3DApp::InitMainWindow()
{
	WNDCLASS wc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hbrBackground	= (HBRUSH)(COLOR_WINDOW + 1);
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	wc.lpfnWndProc		= MainWndProc;
	wc.lpszClassName	= szClassName;
	wc.lpszMenuName		= NULL;
	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.hInstance		= mhInstance;

	if (!RegisterClass(&wc)) 
	{
		MessageBox(NULL, "Class Register Error", "Failed to register window class", MB_OK);
		PostQuitMessage(-1);
	}

	RECT R = {0, 0, WIDTH, HEIGHT};
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	mhWnd = CreateWindow(szClassName, mCaption.c_str(), WS_OVERLAPPED| WS_SYSMENU | WS_MINIMIZEBOX, 100, 100, R.right, R.bottom, NULL, NULL, mhInstance, NULL);
	
	if (!mhWnd)
	{
		MessageBox(NULL, "Window Create Error", "Failed to create window", MB_OK);
		PostQuitMessage(-1);
	}

	UpdateWindow(mhWnd);
	ShowWindow(mhWnd, SW_SHOW);
}

void D3DApp::InitDirectX()
{
	md3dObject = Direct3DCreate9(D3D_SDK_VERSION);
	if (!md3dObject)
	{
		MessageBox(NULL, "DirectX Create Error", "Cannot Create DirectX Object", MB_OK);
		PostQuitMessage(-1);
	}

	D3DDISPLAYMODE mode;
	HR(md3dObject->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode));
	HR(md3dObject->CheckDeviceType(D3DADAPTER_DEFAULT, mDevType, mode.Format, mode.Format, true));

	D3DCAPS9 caps;
	HR(md3dObject->GetDeviceCaps(D3DADAPTER_DEFAULT, mDevType, &caps));
	
	DWORD devBehaviorFlags = 0;
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		devBehaviorFlags |= mRequestedVP;
	else
		devBehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	if (caps.DevCaps & D3DDEVCAPS_PUREDEVICE && mRequestedVP & D3DCREATE_HARDWARE_VERTEXPROCESSING)
		devBehaviorFlags |= D3DCREATE_PUREDEVICE;

	ZeroMemory(&md3dPP, sizeof(D3DPRESENT_PARAMETERS));

	md3dPP.AutoDepthStencilFormat	= D3DFMT_D24S8;
	md3dPP.BackBufferCount			= 1;
	md3dPP.BackBufferFormat			= D3DFMT_UNKNOWN;
	md3dPP.EnableAutoDepthStencil	= true;
	md3dPP.hDeviceWindow			= mhWnd;
	md3dPP.PresentationInterval		= D3DPRESENT_INTERVAL_IMMEDIATE;
	md3dPP.SwapEffect				= D3DSWAPEFFECT_DISCARD;
	md3dPP.Windowed					= true;

	HR(md3dObject->CreateDevice(D3DADAPTER_DEFAULT, mDevType, mhWnd, devBehaviorFlags, &md3dPP, &gd3dDevice));
}

D3DApp::~D3DApp(void)
{
}

bool D3DApp::IsDeviceLost()
{
	HRESULT hr = gd3dDevice->TestCooperativeLevel();
	if (hr == D3DERR_DEVICELOST)
	{
		Sleep(20);
		return true;
	}
	else if (hr == D3DERR_DRIVERINTERNALERROR)
	{
		MessageBox(NULL, "Device Error", "Encountered an internal error, Terminating...", MB_OK);
		PostQuitMessage(-2);
		return true;
	}
	else if (hr ==  D3DERR_DEVICENOTRESET)
	{
		OnLostDevice();
		HR(gd3dDevice->Reset(&md3dPP));
		OnResetDevice();
	}
	return false;
}

INT D3DApp::Run()
{
	MSG msg; msg.message = WM_NULL;

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			mTimer.Tick();
			if (mAppPaused)
			{
				Sleep(20);
				continue;
			}
			else if (!IsDeviceLost())
			{
				CalculateFrameStats();
				UpdateScene(mTimer.DelatTime());
				DrawScene();
			}
		}
	}
	return (INT) msg.wParam;
}

LRESULT CALLBACK D3DApp::WndProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_CLOSE:
		DestroyWindow(mhWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(mhWnd, msg, wParam, lParam);
	}
	return 0;
}

void D3DApp::CalculateFrameStats()
{
	static int	 frameCount = 0;
	static float timeElapsed = 0.0f;

	frameCount++;
	if (timeElapsed >= 1.0f)
	{
		float fps  = (float) frameCount;
		float mspf = 1000.0f / fps;

		std::ostringstream outs;
		outs.precision(6);

		outs << mCaption << " "
			<< "FPS: " << fps << " "
			<< "Frame Time: " << mspf << "ms";
		SetWindowText(mhWnd, outs.str().c_str());
		
		frameCount	= 0;
		timeElapsed = 0.0f;
	}
	timeElapsed += mTimer.DelatTime();
}
