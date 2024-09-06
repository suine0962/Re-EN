// IScene.h
#pragma once

class IScene {
public:
    virtual ~IScene() = default;

    virtual void Init() = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual int GetSceneNo() const = 0;
    virtual bool GameClose() const = 0;
    virtual void Release() = 0;
};