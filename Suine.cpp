#include "Suine.h"


Suine*Suine::GetInstance()
{
	static Suine instance;

	return &instance;
}

void Suine::Initialize()
{
	Suine::GetInstance();

	WinApp::Initialize();
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
	WinApp::Finalize();
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
