#include "ClearScene.h"

// コンストラクタ
ClearScene::ClearScene() : gameCloseFlag_(false), sceneNo_(0) {};

// デストラクタ
ClearScene::~ClearScene() {};



void ClearScene::Init()
{
}

void ClearScene::Update()
{
}

void ClearScene::Draw()
{
}

void ClearScene::Release()
{
}

// シーンが終了したかを確認する
bool ClearScene::GameClose() const {
	return gameCloseFlag_;
}

// シーン番号を取得する
int ClearScene::GetSceneNo() const {
	return sceneNo_;
}

