#include "WinApp.h"
#pragma comment(lib,"winmm.lib")

WinApp *WinApp::GetInstance()
{
	static WinApp instance;
	return &instance;
}


void WinApp::Initialize()
{


	//MainWindow
	WinApp::GetInstance()->hwnd_= CreateWIND(
		WinApp::GetInstance()->kWindowWidth,
		WinApp::GetInstance()->kWindowHeight,
		L"SUINE",
		WinApp::GetInstance()->wc_);
	ShowWindow(WinApp::GetInstance()->hwnd_, SW_SHOW);
	timeBeginPeriod(1);
}

bool WinApp::WinMsg()
{
	if (PeekMessage(&WinApp::GetInstance()->msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&WinApp::GetInstance()->msg);
		DispatchMessage(&WinApp::GetInstance()->msg);
	}

	if (WinApp::GetInstance()->msg.message != WM_QUIT) {
		return true;
	}

	return false;
}

void WinApp::Finalize()
{
	CloseWindow(WinApp::GetInstance()->hwnd_);
}

HWND WinApp::CreateWIND(const int32_t kWidth, const int32_t kHeight, LPCTSTR title, WNDCLASS wc)
{
	wc.lpfnWndProc = WinApp::WindowProc;
	wc.lpszClassName = title;
	wc.hInstance = GetModuleHandle(nullptr);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	RegisterClass(&wc);

	RECT wrc = { 0,0,
	kWidth,
	kHeight };
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	return CreateWindow(
		wc.lpszClassName,
		title,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wrc.right - wrc.left,
		wrc.bottom - wrc.top,
		nullptr,
		nullptr,
		wc.hInstance,
		nullptr
	);
}

LRESULT WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
	{
		return true;
	}
	switch (msg)
	{
		//ウインドウが破棄
	case WM_DESTROY:
		//OSに対しアプリ終了を伝える
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}
