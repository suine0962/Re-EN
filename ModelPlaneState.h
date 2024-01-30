#pragma once
#include"IModelState.h"
#include"Model.h"
#include"CreateResource.h"
#include"WorldTransform.h"
#include"GraphicsPipelineManager.h"

class ModelPlaneState: public IModelState
{
public:

	~ModelPlaneState() {};
	void Initialize(Model* state)override;

	void Draw(Model* state,WorldTransform worldTransform,ViewProjection viewprojection)override;


private:

	void CommandCall(uint32_t texHandle);

	const uint32_t VertexSize = 4;
	const uint32_t IndexSize = 6;
	ResourcePeroperty resource_ = {};


};

