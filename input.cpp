#include "Input.h"
#include "WinApp.h"
#include <Xinput.h>
#include "Vsh.h"
#include <math.h>
#include <iostream>
//Xinput.lib; Xinput9_1_0.lib
#pragma comment(lib, "Xinput.lib")

void ProcessStickInput(SHORT thumbX, SHORT thumbY, const char* stickName) {
	// スティックのデッドゾーンを設定
	const int DEADZONE = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;

	// デッドゾーンを考慮したスティック入力の処理
	if ((thumbX > DEADZONE || thumbX < -DEADZONE) ||
		(thumbY > DEADZONE || thumbY < -DEADZONE)) {

		float magnitude = sqrt(thumbX * thumbX + thumbY * thumbY);

		// 正規化して -1.0 ～ 1.0 の範囲に変換
		float normLX = thumbX / 32767.0f;
		float normLY = thumbY / 32767.0f;

		std::cout << stickName << " X: " << normLX << " Y: " << normLY << std::endl;
	}
}

void Input::Initialize() {
	WinApp* WinApp = WinApp::GetInstance();
	// DirectInputの初期化
	ComPtr<IDirectInput8> directInput = nullptr;
	result = DirectInput8Create(
		WinApp->GetWc().hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput, nullptr);
	assert(SUCCEEDED(result));

	// キーボードデバイスの生成
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));

	// 入力データ形式のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard); // 標準形式
	assert(SUCCEEDED(result));

	// 排他制御レベルのセット
	result = keyboard->SetCooperativeLevel(
		WinApp->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));

	DWORD dwResult;
	for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
	{
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));

		// Simply get the state of the controller from XInput.
		dwResult = XInputGetState(i, &state);

		if (dwResult == ERROR_SUCCESS) {
			std::cout << "Controller connected" << std::endl;

			// 左スティックの入力を処理
			ProcessStickInput(state.Gamepad.sThumbLX, state.Gamepad.sThumbLY, "Left Stick");

			// 右スティックの入力を処理
			ProcessStickInput(state.Gamepad.sThumbRX, state.Gamepad.sThumbRY, "Right Stick");

			// ボタンの状態を確認
			if (state.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
				std::cout << "A button pressed" << std::endl;
			}
			if (state.Gamepad.wButtons & XINPUT_GAMEPAD_B) {
				std::cout << "B button pressed" << std::endl;
			}
		}
		else {
			std::cout << "Controller not connected" << std::endl;
		}
	}


}

void Input::Update() {
	// 前回のキー入力を保存
	memcpy(preKeys, keys, sizeof(keys));

	// キーボード情報の取得開始
	keyboard->Acquire();

	keyboard->GetDeviceState(sizeof(keys), keys);
}



bool Input::PushKey(BYTE keyNumber)
{
	if (keys[keyNumber]) {
		return true;
	}
	return false;
}

bool Input::TriggerKey(BYTE keyNumber)
{
	if (keys[keyNumber] && preKeys[keyNumber] == 0) {
		return true;
	}
	return false;
}
Input* Input::GetInstance() {
	static Input instance;
	return &instance;
}


