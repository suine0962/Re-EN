// GameManager.h
#pragma once
#include <memory>
#include <array>
#include "IScene.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "ClearScene.h"

class GameManager {
public:
    GameManager();
    ~GameManager();

    int Run();

private:
    enum SceneType {
        TITLE,
        STAGE,
        CLEAR,
        SCENE_COUNT
    };

    std::array<std::unique_ptr<IScene>, SCENE_COUNT> sceneArr_;
    SceneType currentSceneNo_ = TITLE;
    SceneType prevSceneNo_ = TITLE;

    void ChangeScene(SceneType newScene);
};
