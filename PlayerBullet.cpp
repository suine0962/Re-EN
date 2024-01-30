#include "PlayerBullet.h"

void PlayerBullet::Initilize(Model* model, const Vector3& pos, const Vector3& velocity)

{
	// NULLポインタチェック
	assert(model);

	model_ = model;
	velocity_ = velocity;
	// テクスチャ読み込み
	textureHandle_ = TextureManager::LoadTexture("Resources/uvChecker.png");

	sprite_ = make_unique<Sprite>();
	sprite_->Initialize(new SpriteBoxState, { 0,0 }, { 500, 500 });
	sprite_->SetTexHandle(textureHandle_);
	sprite_->SetColor({ 1,1,1,0 });

	worldtransform_.Initialize();
	// 引数で受け取った初期座標をセット
	worldtransform_.translate= pos;
	
	SetCollisionAttribute(CollisionConfig::kCollisionAttributePlayer);
	SetCollisionMask(~CollisionConfig::kCollisionAttributePlayer);

}


void PlayerBullet::Update()
{
	worldtransform_.UpdateMatrix();
	worldtransform_.translate =MatrixTransform::VectorAdd(worldtransform_.translate, velocity_);

	//時間経過で取り消し
	if (--deathTimer <= 0) {
		isDead_ = true;
	}
}

void PlayerBullet::Draw(ViewProjection viewProjection)
{
	model_->Draw(worldtransform_,viewProjection_);
}


void PlayerBullet::OnCollision() { isDead_ = true; }


Vector3 PlayerBullet::GetWorldPosition() {
	Vector3 worldPosition;

	worldPosition.x = worldtransform_.matWorld.m[3][0];
	worldPosition.y = worldtransform_.matWorld.m[3][1];
	worldPosition.z = worldtransform_.matWorld.m[3][2];

	return worldPosition;
}