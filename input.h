#pragma once

#define DIRECTINPUT_VERSION		0x0800  // DirectInputのバージョン指定
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")


#include <array>
#include <windows.h>
#include <Xinput.h>
#include <wrl.h>


class Input
{
public:
	/// 
	/// Default Method
	/// 
	static Input* GetInstance();

	// 初期化
	void Initialize();

	// 更新処理
	void Update();

	///
	/// Keyboard Method
	/// 
	// キーを押した瞬間
	bool TriggerKey(BYTE keyNumber) const;

	// キーを押している間
	bool PressKey(BYTE keyNumber)const;

	// キーを離した瞬間
	bool ReleaseKey(BYTE keyNumber)const;

	///
	/// GamePad Method
	/// 
	// ゲームパッドの状態を取得
	bool GetJoystickState(int32_t stickNo, XINPUT_STATE& state);

	// デッドゾーンを適用する関数
	SHORT ApplyDeadzone(SHORT inputValue);

private:
	/// 
	/// Keyboard
	/// 
	/// 
	Microsoft::WRL::ComPtr<IDirectInput8>directInput = nullptr;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboard_ = nullptr;
	std::array<BYTE, 256> key_;
	std::array<BYTE, 256> preKey_;

	///
	/// GamePad
	/// 
	// デッドゾーンの閾値
	const int DEADZONE_THRESHOLD = 8000;

	XINPUT_STATE joyState_;
	XINPUT_STATE preJoyState_;
};


