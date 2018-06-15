#pragma once

#include "D3DUtil.h"
#include "GameTimer.h"

class D3DApp
{
public:
	D3DApp(HINSTANCE hinstance, std::string caption, DWORD requestedVP, D3DDEVTYPE deviceType);
	virtual ~D3DApp(void);

	void InitDirectX();
	void InitMainWindow();
	void CalculateFrameStats();
	
	virtual bool IsDeviceLost(); 
	virtual void OnLostDevice() {};
	virtual void OnResetDevice() {};

	virtual void UpdateScene(float dt) {};
	virtual void DrawScene() {};

	virtual INT  Run();

	HINSTANCE	GetAppInstance() const { return mhInstance; };
	HWND		GetMainWindow() const { return mhWnd; };

	LRESULT CALLBACK WndProc(UINT msg, WPARAM wParam, LPARAM lParam);

protected:
	HINSTANCE	mhInstance;
	HWND		mhWnd;
	DWORD		mRequestedVP;
	std::string mCaption;
	bool		mAppPaused;
	GameTimer	mTimer;

	D3DPRESENT_PARAMETERS	md3dPP;
	LPDIRECT3D9				md3dObject;
	D3DDEVTYPE				mDevType;
};

extern LPDIRECT3DDEVICE9 gd3dDevice;
extern D3DApp*			 gd3dApp;
