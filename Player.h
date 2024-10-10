#pragma once
#include "ViewProjection.h"
#include "Vsh.h"
#include "Sprite.h"
#include "WorldTransform.h"
#include "Sprite.h"
#include "input.h"
#include "Model.h"
#include "PlayerBullet.h"
#include "Collider.h"
#include "CollisionConfig.h"
#include "CollisionManager.h"
#include <list>
#include <algorithm>

#include <cassert>
#include "MatrixTransform.h"
#include "ImGuiManager.h"
#include "GameScene.h"
#include "VectorTransform.h"



/// <summary>
/// 自キャラ
/// </summary>
class Player : public Collider {
public:
	~Player();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name= "model">モデル</param>
	/// <param name= "textureHandle">テクスチャハンドル</param>
	void Initialize(Model* model, uint32_t& textureHandle, Vector3 position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update(const ViewProjection viewProjection);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name= "viewProjection">ビュープロジェクション（参照渡し）</param>
	void Draw(ViewProjection& viewProjection);

	//void DrawUI();

	/// <summary>
	/// 回転
	/// </summary>
	void Rotate();

	/// <summary>
	/// 攻撃
	/// </summary>
	void Attack();

	void OnCollision() override;

	Vector3 GetWorldPosition() override;

	const std::list<PlayerBullet*>& GetBullets() const { return bullets_; }

	//void SetParent(const WorldTransform* parent) { worldTransform_.parent_ = parent; }

	void SetReticle(const ViewProjection viewProjection);

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	// 自キャラ
	Player* player_ = nullptr;
	// キーボード入力
	Input* input_ = nullptr;

	WorldTransform worldTransform3DReticle_;
	WorldTransform worldTransform2DReticle_ = {};
	Sprite* sprite2DReticle_ = nullptr;

	// リスト
	std::list<PlayerBullet*> bullets_;

	ResourcePeroperty resource_ = {};
};