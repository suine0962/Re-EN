#pragma once

struct Matrix4x4 final
{
	float m[4][4];
};

//#include "Enemy.h"
//
//Enemy::Enemy()
//{
//}
//
//void Enemy::Initialize(Model* model, const Vector3& position, const Vector3& velocity)
//{
//	input_ = Input::GetInstance();
//
//	model_ = make_unique<Model>();
//	model_->CreateFromObj("colision");//ファイルを入れる
//
//	sprite_ = make_unique<Sprite>();
//	sprite_->Initialize(new SpriteBoxState, { 0,0 }, { 500, 500 });
//	sprite_->SetTexHandle(tex_);
//	sprite_->SetColor({ 1,1,1,0 });
//
//	viewProjection_.Initialize({ 0.2f,-0.6f,0.0f }, { 11.0f,5.0f,-15 });
//
//	worldTransform_.Initialize();
//	worldTransform_.scale = { 1,1,1 };
//
//
//
//}
//
//void Enemy::Updata()
//{
//
//
//}
//
//void Enemy::Draw(ViewProjection viewProjection)
//{
//	sprite_->Draw(worldTransform_);
//	model_->Draw(worldTransform_, viewProjection_);
//
//}
