#include "input.h"
#include <cassert>
#include "WinApp.h"

Input* Input::GetInstance() {
	static Input instance;

	return &instance;
}

void Input::Initialize()
{
	//DirectInputのインスタンス生成
	HRESULT result;

	Microsoft::WRL::ComPtr<IDirectInput8>directInput = nullptr;
	result = DirectInput8Create(WinApp::GetInstance()->GetWc().hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput, nullptr);
	assert(SUCCEEDED(result));

	//キーボードデバイス生成
	Microsoft::WRL::ComPtr<IDirectInputDevice8>keyboard_;
	result = directInput->CreateDevice(GUID_SysKeyboard,&keyboard_,NULL);
	assert(SUCCEEDED(result));

	//入力データ形式のセット
	result = keyboard_->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));

	//排他制御レベルのセット
	result = keyboard_->SetCooperativeLevel(WinApp::GetInstance()->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));


}

void Input::Update()
{
	preKey_ = key_;

	//キーボード情報の取得
	keyboard_->Acquire();

	//全キーの入力状態を取得する
	BYTE key[256] = {};
	keyboard_->GetDeviceState(sizeof(key), key);

}

bool Input::TriggerKey(BYTE keyNumber) const {
	if (!preKey_[keyNumber] && key_[keyNumber]) {
		return true;
	}
	else {
		return false;
	}
}

bool Input::PressKey(BYTE keyNumber)const {
	if (key_[keyNumber]) {
		return true;
	}
	else {
		return false;
	}
}

bool Input::ReleaseKey(BYTE keyNumber)const {
	if (preKey_[keyNumber] && !key_[keyNumber]) {
		return true;
	}
	else {
		return false;
	}
}

//bool Input::GetJoystickState(int32_t stickNo, XINPUT_STATE& state) {
//	//DWORD result = XInputGetState(stickNo, &state);
//	//result == ERROR_SUCCESS;
//	return 0;
//}

// デッドゾーンを適用する関数
//SHORT Input::ApplyDeadzone(SHORT inputValue) {
//	if (abs(inputValue) < DEADZONE_THRESHOLD) {
//		return 0; // デッドゾーン内の入力は無視
//	}
//	// デッドゾーン外の入力はそのまま返す
//	return inputValue;
//}



