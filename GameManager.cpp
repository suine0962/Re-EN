#include "GameManager.h"
#include <Windows.h>
#include "WinApp.h"
#include "DirectXCommon.h"
#include "GraphicsPipelineManager.h"
#include "Input.h"
#include "ImGuiManager.h"
#include "TextureManager.h"
#include "Sprite.h"
#include "Grid.h"
#include "Model.h"

#include "MatrixTransform.h"
#include "Vsh.h"

// コンストラクタ
GameManager::GameManager() {
	// 各シーンの排列
	//sceneArr_[TITLE] = make_unique<TitleScene>();
	//sceneArr_[STAGE] = make_unique<GameScene>();
	//sceneArr_[CLEAR] = make_unique<ClearScene>();

	// 初期シーンの設定
	//sceneNo_ = TITLE; //GameManagerのクラスにISceneを継承させて触れるようにしているため正しいかは怪しい
	//input_ = Input::GetInstance();
}

GameManager::~GameManager() {}

const char kWindowTitle[] = "SUINE";

int GameManager::Run() {

	WinApp* winApp = WinApp::GetInstance();
	winApp->Initialize(L"CG4");

	DirectXCommon* DirctX = DirectXCommon::GetInstance();
	DirctX->initialize();


	TextureManager* textureManager = TextureManager::GetInstance();

	GraphicsPipelineManager* PSOManager = GraphicsPipelineManager::GetInstance();
	PSOManager->GetPso().Line;
	PSOManager->GetPso().particle;
	PSOManager->GetPso().shape;
	PSOManager->GetPso().Sprite;

	sceneArr_[currentSceneNo_]->Init();

	Input* input = Input::GetInstance();
	input->Initialize();

	// ウィンドウの×ボタンが押されるまでループ
	while (true)  // ゲームループ
	{
		// Windowsのメッセージ処理
		if (winApp->ProcessMessage()) {
			// ゲームループを抜ける
			break;
		}
		// ゲームの処理の開始
		DirctX->BeginFlame();
		input->Update();

		ImGui::Begin("kakunin");
		ImGui::Text("%d", IScene::GetSceneNo());
		ImGui::End();

		// シーンのチェック
		prevSceneNo_ = currentSceneNo_;
		currentSceneNo_ = sceneArr_[currentSceneNo_]->GetSceneNo();

		// シーン変更チェック
		if (prevSceneNo_ != currentSceneNo_) {
			sceneArr_[currentSceneNo_]->Init();
		}

		///
		/// ↓更新処理ここから
		///
		sceneArr_[currentSceneNo_]->Update(); // シーンごとの更新処理

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		sceneArr_[currentSceneNo_]->Draw();


		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		//スワップチェーン
		DirctX->EndFlame();
		// ESCキーが押されたらループを抜ける
		if (sceneArr_[currentSceneNo_]->GameClose()) {
			sceneArr_[currentSceneNo_]->Release();
			break;
		}
	}

	//出力ウィンドウへの文字出力
	OutputDebugStringA("Hello,DirectX!\n");


	/*------------------------------------------------------------

	-------------------------------------------------------------*/
	winApp->Finalize();
	//delete sWinAPI;
	DirctX->Finalize();

	return 0;
}
