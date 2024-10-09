#include "Player.h"


Player::~Player() {
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}

	delete sprite2DReticle_;
}

void Player::Initialize(Model* model, uint32_t& textureHandle, Vector3 position) {
	assert(model);

	model_ = new Model();

	model_ = model;

	
	textureHandle_ = textureHandle;

	worldTransform_.Initialize();

	// シングルトンインスタンスを取得する
	input_ = Input::GetInstance();

	SetCollisionAttribute(CollisionConfig::kCollisionAttributePlayer);
	SetCollisionMask(~CollisionConfig::kCollisionAttributePlayer);

	worldTransform_.translate = MatrixTransform::VectorAdd(worldTransform_.translate, position);
	worldTransform3DReticle_.Initialize();

	model_->CreateFromObj("Resource/awin");

	uint32_t textureReticle = TextureManager::LoadTexture("Resource/reticle.png");

	sprite2DReticle_->Initialize(new SpriteBoxState, { 640.0f, 360.0f },{500.0f,200.0f});
	sprite2DReticle_->SetTexHandle(textureReticle);
	sprite2DReticle_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });



}

void Player::Update(const ViewProjection viewProjection) {
	// falseになった弾を消す
	bullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
		});

	worldTransform_.TransfarMatrix(resource_.wvpResource,viewProjection);
	// キャラクターの移動ベクトル
	Vector3 move = { 0, 0, 0 };

	// キャラクターの移動速度
	const float kCharacterSpeed = 0.2f;

	//// 押した方向で移動ベクトルを変更（左右）
	//if (input_->PushKey(DIK_LEFT)) {
	//	move.x -= kCharacterSpeed;
	//}
	//else if (input_->PushKey(DIK_RIGHT)) {
	//	move.x += kCharacterSpeed;
	//}

	//// 押した方向で移動ベクトルを変更（上下）
	//if (input_->PushKey(DIK_UP)) {
	//	move.y += kCharacterSpeed;
	//}
	//else if (input_->PushKey(DIK_DOWN)) {
	//	move.y -= kCharacterSpeed;
	//}

	XINPUT_STATE Gamepad;
	if (Input::GetInstance()->GetJoystickState(Gamepad)) {
		move.x += (float)Gamepad.Gamepad.sThumbLX / SHRT_MAX * kCharacterSpeed;
		move.y += (float)Gamepad.Gamepad.sThumbLY / SHRT_MAX * kCharacterSpeed;
	}

	// ベクターの加算
	worldTransform_.translate = MatrixTransform::VectorAdd(worldTransform_.translate, move);
	// アフィン変換行列の作成
	worldTransform_.matWorld = MatrixTransform::AffineMatrix(
	   worldTransform_.scale, worldTransform_.rotation, worldTransform_.translate);

	// 行列更新
	worldTransform_.UpdateMatrix();

	Player::Rotate();

	Player::Attack();

	for (PlayerBullet* bullet : bullets_) {
		bullet->Update();
	}

	//SetReticle(viewProjection);

	float imputFloat3[3] = {
		worldTransform_.translate.x, worldTransform_.translate.y,
		worldTransform_.translate.z };

	

	// 移動限界座標
	const float kMoveLimitX = 34;
	const float kMoveLimitY = 18;

	// 範囲を超えない処理
	worldTransform_.translate.x = max(worldTransform_.translate.x, -kMoveLimitX);
	worldTransform_.translate.x = min(worldTransform_.translate.x, +kMoveLimitX);
	worldTransform_.translate.y = max(worldTransform_.translate.y, -kMoveLimitY);
	worldTransform_.translate.y = min(worldTransform_.translate.y, +kMoveLimitY);
}

// 回転
void Player::Rotate() {

	XINPUT_STATE GameController;
	Input::GetInstance()->GetJoystickState(GameController);
	float RotateSpeed=0.1f;

	if (GameController.Gamepad.bRightTrigger > 30.0f) {//LTボタンに設定する

		worldTransform_.rotation.y +=RotateSpeed;
		if (worldTransform_.rotation.y > 0.9)
		{
			RotateSpeed = 0.0f;
		}

	}
	
	// 速さ
	//const float kRotSpeed = 0.02f;
	//// 押した方向で移動ベクトル
	//if (input_->PushKey(DIK_A)) {
	//	worldTransform_.rotation_.y += kRotSpeed;
	//}
	//else if (input_->PushKey(DIK_D)) {
	//	worldTransform_.rotation_.y -= kRotSpeed;
	//}
}

// 攻撃
void Player::Attack() {
	XINPUT_STATE GameController;
	Input::GetInstance()->GetJoystickState(GameController);
	if (GameController.Gamepad.wButtons & XINPUT_GAMEPAD_B) {
		const float kBulletSpeed = 1.0f;
		Vector3 velocity(0, 0, kBulletSpeed);

		velocity = VectorTransform::TransformNormal(velocity, worldTransform_.matWorld);

		velocity = MatrixTransform::Subtract(
			{ worldTransform3DReticle_.matWorld.m[3][0], worldTransform3DReticle_.matWorld.m[3][1],
			 worldTransform3DReticle_.matWorld.m[3][2] },
			GetWorldPosition());
		velocity = MatrixTransform::VectorMultiply(kBulletSpeed,MatrixTransform::Normalize(velocity));

		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initilize(model_, GetWorldPosition(), velocity);
		bullets_.push_back(newBullet);
	}

	if (Input::GetInstance()->GetJoystickState(GameController))
	{
		return;
	}

}

void Player::Draw(ViewProjection& viewProjection_) {
	model_->Draw(worldTransform_, viewProjection_);
	//bullets_->Draw(viewProjection_);
	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw(viewProjection_);
	}

	//model_->Draw(worldTransform3DReticle_, viewProjection_);
}

//void Player::DrawUI() { sprite2DReticle_->Draw(); }

void Player::OnCollision() {

}

Vector3 Player::GetWorldPosition() {
	Vector3 worldPosition;

	worldPosition.x = worldTransform_.matWorld.m[3][0];
	worldPosition.y = worldTransform_.matWorld.m[3][1];
	worldPosition.z = worldTransform_.matWorld.m[3][2];

	return worldPosition;
}

//void Player::SetReticle(const ViewProjection viewProjection) {
//	const float kDistancePlayerTo3DReticle = 50.0f;
//	Vector3 offset = { 0, 0, 1.0f };
//
//	offset = TransformNormal(offset, worldTransform_.matWorld_);
//	offset = Multiply(kDistancePlayerTo3DReticle, Normalize(offset));
//
//	worldTransform3DReticle_.translation_ = Add(GetWorldPosition(), offset);
//	worldTransform3DReticle_.UpdateMatrix();
//
//	Vector3 positionReticle = {
//		worldTransform3DReticle_.matWorld_.m[3][0], worldTransform3DReticle_.matWorld_.m[3][1],
//		worldTransform3DReticle_.matWorld_.m[3][2] };
//
//	Matrix4x4 matViewport =
//		MakeViewPortMatrix(0.0f, 0.0f, WinApp::kWindowWidth, WinApp::kWindowHeight, 0.0f, 1.0f);
//
//	Matrix4x4 matViewProjectionViewport =
//		Multiply(Multiply(viewProjection.matView, viewProjection.matProjection), matViewport);
//	positionReticle = Transform(positionReticle, matViewProjectionViewport);
//	//sprite2DReticle_->SetPosition(Vector2(positionReticle.x, positionReticle.y));
//
//
//	 // マウス用　一時削除
//	/* POINT mousePosition;
//
//	 GetCursorPos(&mousePosition);
//
//	 HWND hwnd = WinApp::GetInstance()->GetHwnd();
//	 ScreenToClient(hwnd, &mousePosition);
//	 sprite2DReticle_->SetPosition({(float)mousePosition.x, (float)mousePosition.y});*/
//
//	XINPUT_STATE joyState;
//	Vector2 spritePosition = sprite2DReticle_->GetPosition();
//
//	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
//		spritePosition.x += (float)joyState.Gamepad.sThumbRX / SHRT_MAX * 5.0f;
//		spritePosition.y -= (float)joyState.Gamepad.sThumbRY / SHRT_MAX * 5.0f;
//
//		sprite2DReticle_->SetPosition(spritePosition);
//	}
//
//	Matrix4x4 matVPV = Multiply(Multiply(viewProjection.matView, viewProjection.matProjection), matViewport);
//	Matrix4x4 matInverseVPV = Inverse(matVPV);
//
//	Vector3 posNear = {
//		(float)sprite2DReticle_->GetPosition().x, (float)sprite2DReticle_->GetPosition().y, 0 };
//
//	Vector3 posFar = {
//		(float)sprite2DReticle_->GetPosition().x, (float)sprite2DReticle_->GetPosition().y, 1 };
//
//	posNear = Transform(posNear, matInverseVPV);
//	posFar = Transform(posFar, matInverseVPV);
//
//	Vector3 mouseDirection = Subtract(posFar, posNear);
//	mouseDirection = Normalize(mouseDirection);
//
//	const float kDistancetestObject = 100.0f;
//
//	worldTransform3DReticle_.translation_ =
//		Add(posNear, Multiply(kDistancetestObject, mouseDirection));
//	worldTransform3DReticle_.UpdateMatrix();
//
//	ImGui::Begin("Player");
//	ImGui::Text("Near:(%+.2f,%+.2f,%.2f)", posNear.x, posNear.y, posNear.z);
//	ImGui::Text("Far:(%+.2f,%+.2f,%.2f)", posFar.x, posFar.y, posFar.z);
//	ImGui::Text(
//		"3DRetixle:(%+.2f,%+.2f,%.2f)", worldTransform3DReticle_.translation_.x,
//		worldTransform3DReticle_.translation_.y, worldTransform3DReticle_.translation_.z);
//	ImGui::End();
//}