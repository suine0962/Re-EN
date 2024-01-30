﻿#pragma once
#include"ISpriteState.h"
#include"CreateResource.h"
#include"Sprite.h"
#include"GraphicsPipelineManager.h"
#include"ShaderManager.h"
#include"TextureManager.h"

class SpriteBoxState:public ISpriteState
{
public:
	SpriteBoxState() {};
	~SpriteBoxState() {};

	void Initialize(Sprite* state)override;
	void Draw(Sprite* state, WorldTransform worldTransform)override;

private:
	void CommandCall(uint32_t texHandle);

	ResourcePeroperty resource_ = {};

	const uint32_t VertexSize = 4;
	const uint32_t IndexSize = 6;

};

