#include "Model.h"
#include "ViewProjection.h"
#include "Vector3.h"
#include "WorldTransform.h"
#include "Sprite.h"
#include "MatrixTransform.h"
#include "Collider.h"
#include "CollisionConfig.h"
#include "CollisionManager.h"

class PlayerBullet:public Collider {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initilize(Model* model, const Vector3& pos,const Vector3 & velocity_);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera"></param>
	void Draw(ViewProjection viewProjection);

	bool IsDead() const { return isDead_; }

	void OnCollision() override;

	Vector3 GetWorldPosition() override;

private:
	unique_ptr<Sprite>sprite_;
	WorldTransform worldtransform_;
	Model* model_ = nullptr;
	uint32_t textureHandle_;
	ViewProjection viewProjection_;
	Vector3 velocity_;

	// 弾
	static const int32_t kLifeTime = 60 * 4;
	// デスタイマー
	int32_t deathTimer = kLifeTime;
	// デスフラグ
	bool isDead_ = false;
};