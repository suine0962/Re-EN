#include "Player.h"

Player::Player()
{
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
	}
	if (input_->PushKey(DIK_D))
	{
		worldTransform_.translate.x += 5.0f;
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

void Player::PlayerRowling()
{
	// Vector3で指定された回転角度 (度単位)
	Vector3 rotationEulerAngles(30.0f, 45.0f, 60.0f);

	// 度をラジアンに変換
	float pitch = rotationEulerAngles.x * (3.14159265f / 180.0f);
	float yaw = rotationEulerAngles.y * (3.14159265f / 180.0f);
	float roll = rotationEulerAngles.z * (3.14159265f / 180.0f);

	// 各軸に対する回転クォータニオンの生成
	Quaternion qPitch = MathQuaternion::MakeRotateAxisAngleQuaternion(Vector3(1, 0, 0), pitch);
	Quaternion qYaw = MathQuaternion::MakeRotateAxisAngleQuaternion(Vector3(0, 1, 0), yaw);
	Quaternion qRoll = MathQuaternion::MakeRotateAxisAngleQuaternion(Vector3(0, 0, 1), roll);

	// 総合的な回転クォータニオンを計算
	Quaternion combinedRotation =  MathQuaternion::MultiplyQuaternions
	(qYaw,MathQuaternion::MultiplyQuaternions(qPitch, qRoll));

	// オブジェクトに新しい回転クォータニオンを設定
	model_->SetUvRotateQuaternion(combinedRotation);

}




