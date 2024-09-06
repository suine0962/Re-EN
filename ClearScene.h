#pragma once
#include "IScene.h"

class ClearScene : public IScene
{
public:

	ClearScene();

	~ClearScene();

    // シーンの初期化
    void Init() override;

    // シーンの更新
    void Update() override;

    // シーンの描画
    void Draw() override;

    // シーンのリソース解放
    void Release() override;

    // シーンが終了したか
    bool GameClose() const override;

    // シーン番号を取得
    int GetSceneNo() const override;

private:
    bool gameCloseFlag_;
    int sceneNo_;

};

