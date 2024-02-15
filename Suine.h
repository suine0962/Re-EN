#pragma once
#include"Vsh.h"
#include"WinApp.h"
#include"DirectXCommon.h"
#include"ShaderManager.h"
#include"GraphicsPipelineManager.h"
#include"Model.h"
#include"ImGuiManager.h"
#include"DebugTools.h"
#include"TextureManager.h"
#include"Sprite.h"
#include"input.h"

class Suine
{
public:
	Suine() {};
	~Suine() {};

	static Suine* GetInstance();

	static void Initialize();
	static void Finalize();

	static void BeginFlame();
	static void EndFlame();
private:

	D3DResourceLeakChecker leakCheck; 
	//Input* input = nullptr;
};

