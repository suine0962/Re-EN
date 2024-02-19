#include "Suine.h"


Suine*Suine::GetInstance()
{
	static Suine instance;

	return &instance;
}

void Suine::Initialize()
{
	Suine::GetInstance();

	WinApp::GetInstance()->Initialize(L"SUINE");
	DirectXCommon::initialize();
	ShaderManager::Initialize();
	GraphicsPipelineManager::Initialize();
	TextureManager::Initialize();
	ImGuiManager::Initialize();
	DebugTools::Initialize();
}

void Suine::Finalize()
{
	TextureManager::Finalize();
	ImGuiManager::Finalize();
	DirectXCommon::Finalize();
	WinApp::GetInstance()->Finalize();
}

void Suine::BeginFlame()
{
	DirectXCommon::BeginFlame();
	ImGuiManager::BeginFlame();
}

void Suine::EndFlame()
{
	ImGuiManager::EndFlame();
	DirectXCommon::EndFlame();
}
