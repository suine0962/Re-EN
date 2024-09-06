#include "GameScene.h"
#include <iostream>

// コンストラクタ
GameScene::GameScene() : gameCloseFlag_(false), sceneNo_(0) {}

// デストラクタ
GameScene::~GameScene() {}

void GameScene::Init()
{
}

void GameScene::Update()
{
}

void GameScene::Draw()
{
}

void GameScene::Release()
{
}


// シーンが終了したかを確認する
bool GameScene::GameClose() const {
    return gameCloseFlag_;
}

// シーン番号を取得する
int GameScene::GetSceneNo() const {
    return sceneNo_;
}
