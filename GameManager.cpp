// GameManager.cpp
#include "GameManager.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "TextureManager.h"
#include "GraphicsPipelineManager.h"
#include <Windows.h>
#include "Suine.h"

// コンストラクタ
GameManager::GameManager() {
    // 各シーンの初期化
    sceneArr_[TITLE] = std::make_unique<TitleScene>();
    sceneArr_[STAGE] = std::make_unique<GameScene>();
    sceneArr_[CLEAR] = std::make_unique<ClearScene>();
}

GameManager::~GameManager() {
    // シーンのリリース（ユニークポインタが自動でクリーンアップ）
}

int GameManager::Run() {

    Suine::Initialize();

    Input* input = Input::GetInstance();
    input->Initialize();

    Particle* particle = new Particle();
    
    DirectionalLight directionalLight;


    // 各シーンの初期化
    sceneArr_[currentSceneNo_]->Init();

    // ウィンドウの×ボタンが押されるまでループ
    while (true) {
        // Windowsのメッセージ処理
        if (WinApp::GetInstance()->ProcessMessage()) {

            break;

        }

        Suine::BeginFlame();

        input->Update();

        // シーンの更新
        prevSceneNo_ = currentSceneNo_;
        currentSceneNo_ = static_cast<SceneType>(sceneArr_[currentSceneNo_]->GetSceneNo());

        // シーン変更チェック
        if (prevSceneNo_ != currentSceneNo_) {
            ChangeScene(currentSceneNo_);
        }

        // 現在のシーンを更新
        sceneArr_[currentSceneNo_]->Update();

        // 現在のシーンを描画
        sceneArr_[currentSceneNo_]->Draw();

        Suine::EndFlame();

        // ゲーム終了チェック
        if (sceneArr_[currentSceneNo_]->GameClose()) {
            sceneArr_[currentSceneNo_]->Release();
            break;
        }
    }

    Suine::GetInstance()->Finalize();

    return 0;
}

void GameManager::ChangeScene(SceneType newScene) {
    // 古いシーンのリリース
    sceneArr_[prevSceneNo_]->Release();
    // 新しいシーンの初期化
    sceneArr_[newScene]->Init();
}