#pragma once
#include "ViewProjection.h"
#include "Pch.h"
#include "Sprite.h"
#include "WorldTransform.h"
#include "TextureManager.h"
#include "Sprite.h"
#include "input.h"
#include "Model.h"
#include "Collider.h"
#include "CollisionConfig.h"
#include "CollisionManager.h"

class Enemy:public Collider
{
public:

	Enemy();
	~Enemy();

	void Initialize(Model* model, const Vector3& position, const Vector3& velocity);
	void Update();
	void Draw(const ViewProjection& viewProjection);

	void Move(Vector3 velocity);

	void OnCollision() override;

	bool IsDead() const { return isDead_; }

private:
	WorldTransform worldTransform_;
	Model*model_=nullptr;
	uint32_t tex_ = 0;
	Input* input_ = nullptr;
	unique_ptr<Sprite>sprite_;
	ViewProjection viewProjection_;
	bool isDead_ = false;
};

