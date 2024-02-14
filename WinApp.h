#pragma once
#include"Vsh.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

class WinApp
{
public:
	WinApp() = default;
	~WinApp() {};
	static WinApp *GetInstance();

	static void Initialize();

	static bool WinMsg();

	static void Finalize();

#pragma region get
	static HWND GetHwnd() { return WinApp::GetInstance()->hwnd_; }
	static WNDCLASS GetWc() { return WinApp::GetInstance()->wc_; }


	static int32_t GetkCilientWidth() { return WinApp::GetInstance()->kWindowWidth; }
	static int32_t GetkCilientHeight() { return WinApp::GetInstance()->kWindowHeight; }
#pragma endregion

private:

	static HWND CreateWIND(const int32_t kWidth, const int32_t kHeight,LPCTSTR title,WNDCLASS wc);

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	static const int32_t kWindowWidth = 1280;
	static const int32_t kWindowHeight = 720;

	static const int32_t kDebugToolWindowWidth = 320;
	static const int32_t kDebugToolWindowHeight = 130;

	MSG msg{};


	WNDCLASS wc_{};
	HWND hwnd_ = {};

	HWND DebugHwnd_{};


};


