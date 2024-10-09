#include "TitleScene.h"
#include <iostream>

// コンストラクタ
TitleScene::TitleScene() : gameCloseFlag_(false), sceneNo_(0) {}

// デストラクタ
TitleScene::~TitleScene() {}

// 初期化処理
void TitleScene::Init() {
    // タイトルシーンの初期化処理
    model_ = new Model();
    uint32_t texHandle_ = TextureManager::LoadTexture("Resource/awin/tex.png");
    player_ = new Player();
    Vector3 PlayerPosition = { 0,0,30 };


    player_->Initialize(model_, texHandle_, PlayerPosition);
}

// 更新処理
void TitleScene::Update() {
    // タイトルシーンの更新処理
  
    player_->Update(view_);

}

// 描画処理
void TitleScene::Draw() {
    // タイトルシーンの描画処理
    player_->Draw(view_);
}

// リソースの解放処理
void TitleScene::Release() {
    // タイトルシーンの解放処理
   
}

// シーンが終了したかを確認する
bool TitleScene::GameClose() const {
    return gameCloseFlag_;
}

// シーン番号を取得する
int TitleScene::GetSceneNo() const {
    return sceneNo_;
}
