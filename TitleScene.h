#pragma once
#include "IScene.h"
#include "Player.h"

class TitleScene : public IScene {
public:
    // コンストラクタ
    TitleScene();

    // デストラクタ
    ~TitleScene();

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
    Player* player_ = nullptr;
    Model* model_ = nullptr;
    WorldTransform worldTransform_;
    ViewProjection view_;
};

