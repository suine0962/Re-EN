#include "Player.h"

Player::Player()
{
	rotation_ = Quaternion();
}

void Player::Initilize()
{
	input_ = Input::GetInstance();

	model_ = new Model();
	model_->CreateFromObj("colision");//ファイルを入れる

	tex_ = TextureManager::LoadTexture("");//ファイルを入れる

	SetCollisionAttribute(CollisionConfig::kCollisionAttributePlayer);
	SetCollisionMask(~CollisionConfig::kCollisionAttributePlayer);

	sprite_ = make_unique<Sprite>();
	sprite_->Initialize(new SpriteBoxState, { 0,0 },{500, 500});
	sprite_->SetTexHandle(tex_);
	sprite_->SetColor({ 1,1,1,0 });

	viewProjection_.Initialize({ 0.2f,-0.6f,0.0f }, { 11.0f,5.0f,-15 });

	worldTransform_.Initialize();
	worldTransform_.scale = {1,1,1};

	

}

void Player::Updata()
{
	bullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
		});

	if(input_->PushKey(DIK_W))
	{
		worldTransform_.translate.y -= 5.0f;
	}
	if (input_->PushKey(DIK_S))
	{
		worldTransform_.translate.y += 5.0f;
	}
	if (input_->PushKey(DIK_A))
	{
		worldTransform_.translate.x -= 5.0f;
		RotatePlayer();
		
	}
	if (input_->PushKey(DIK_D))
	{
		worldTransform_.translate.x += 5.0f;
		RotatePlayer();
	}

	Player::Attack();

	for (PlayerBullet* bullet : bullets_) {
		bullet->Update();
	}

	worldTransform_.UpdateMatrix();
	viewProjection_.UpdateMatrix();
}

void Player::Draw(ViewProjection viewProjection)
{
	Matrix4x4 rotationMatrix = rotation_.toMatrix();

	model_->Draw(worldTransform_,viewProjection_);
	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw(viewProjection_);
	}
}

void Player::Attack()
{
	if (input_->TriggerKey(DIK_SPACE)) {

		const float kBulletSpeed = 1.0f;
		Vector3 velocity(0, 0, kBulletSpeed);

		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initilize(model_, GetWorldPosition(), velocity);
		bullets_.push_back(newBullet);

	}
}

void Player::PlayerTilt()
{
}


void Player::RotatePlayer()
{
	float angle = 1.0f * (3.14159265f / 180.0f); // 1度をラジアンに変換
	Quaternion deltaRotation = Quaternion::eulerToQuaternion(0.0f, angle, 0.0f);
	rotation_ = MathQuaternion::MultiplyQuaternions(deltaRotation,rotation_); // 回転を合成
	rotation_.normalize(); // クオータニオンを正規化
}
