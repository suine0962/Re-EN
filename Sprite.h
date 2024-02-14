#pragma once
#include"Vsh.h"
#include"WorldTransform.h"
#include"SpriteBoxState.h"

class Sprite
{
public:
	Sprite() {};
	~Sprite() { delete state_; };

	void Initialize(ISpriteState* state,Vector2 pos,Vector2 size);

	void Draw(WorldTransform worldTransform);
	
	//getter 
	const Vector2& GetAnchorPoint()const { return anchorPoint_; }
	const bool& GetIsFlipX()const { return isFlipX_; }
	const bool& GetIsFlipY()const { return isFlipY_; }
	const Vector2& GettextureLeftTop()const { return textureLeftTop_; }
	const Vector2& GettextureSize_()const { return textureSize_; }


	//setter
	void SetAnchorPoint(const Vector2& anchorPoint) { this->anchorPoint_ = anchorPoint;}
	void SetIsFlipX(const bool& isFlipX) { this->isFlipX_=isFlipX; }
	void SetIsFlipY(const bool&isFlipY) { this->isFlipY_ = isFlipY; }
	void SettextureLeftTop_(const Vector2& textureLeftTop) { this->textureLeftTop_; }
	void SettextureSize_(const Vector2& textureSize) { this->textureSize_; }


    // Set
	void SetTexHandle(uint32_t texHandle) { texHandle_ = texHandle; }
	void SetUvScale(Vector3 uvScale) { uvScale_ = uvScale; }
	void SetUvRotate(Vector3 uvRotate) { uvRotate_ = uvRotate; }
	void SetUvTranslate(Vector3 uvTranslate) { uvTranslate_ = uvTranslate; }
	void SetColor(Vector4 color) { color_ = color; }
#pragma endregion 

	//get
	WorldTransform GetWorldTransform() { return worldTransform_; }
	Vector4 GetColor() { return color_; }
	uint32_t GetTexHandle() { return texHandle_; }

	Vector3 GetuvScale() { return uvScale_; }
	Vector3 GetuvRotate() { return uvRotate_; }
	Vector3 GetuvTranslate() { return uvTranslate_; }


	/// <summary>
	/// GetSpritePos
	/// </summary>
	Vector2 GetPos() { return Pos_; }
	/// <summary>
	/// GetSpriteSize
	/// </summary>
	Vector2 GetSize() { return size_; }

	/// <summary>
	/// 色コードをVector4に変換関数
	/// </summary>
	static Vector4 ColorConversion(uint32_t rgbaValue);

#pragma endregion

private:
	Vector2 Pos_ = { 0,0};
	Vector2 size_ = {};
	Vector2 anchorPoint_ = { 0.0f,0.0f };

	Vector4 color_ = { 1,1,1,1 };
	WorldTransform worldTransform_ = {};
	uint32_t texHandle_ = 0;

	Vector3 uvScale_ = { 1,1,1 };
	Vector3 uvRotate_ = { 0,0,0 };
	Vector3 uvTranslate_ = { 0,0,0 };

	ISpriteState* state_ = {};

	VertexData vertexData[4] = {};

	//左右フリップ
	bool isFlipX_ = false;
	//上下フリップ
	bool isFlipY_ = false;

	//テクスチャ左上座標
	Vector2 textureLeftTop_ = { 0.0f,0.0f };
	//テクスチャ切り出しサイズ
	Vector2 textureSize_ = { 100.0f,100.0f };


};


