#pragma once
#include "ViewProjection.h"
#include "Pch.h"
#include "Sprite.h"
#include "WorldTransform.h"
#include "TextureManager.h"
#include "Sprite.h"
#include "input.h"
#include "Model.h"
#include "PlayerBullet.h"
#include "Collider.h"
#include "CollisionConfig.h"
#include "CollisionManager.h"
#include <list>

class Player:public Collider
{
public:

	Player();
	~Player();

	void Initilize();

	void Updata();

	void Draw(ViewProjection viewProjection);

	void Attack();

	const std::list<PlayerBullet*>& GetBullets() const { return bullets_; }

	void OnCollision() override;

	Vector3 GetWorldPosition() override;

private:
	WorldTransform worldTransform_;
	Model* model_=nullptr;
	uint32_t tex_ = 0;
	Input* input_ = nullptr;
	unique_ptr<Sprite>sprite_;
	ViewProjection viewProjection_;

	std::list<PlayerBullet*> bullets_;
};

