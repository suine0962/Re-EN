#pragma once
#include"IModelState.h"
#include"Model.h"
#include "Camera.h"
#include "DirectionalLight.h"

class ModelObjState :public IModelState
{
public:
	ModelObjState() {};
	~ModelObjState() {};

	void Initialize(Model* state)override;

	void Draw(Model* state, WorldTransform worldTransform, ViewProjection viewprojection)override;




private:

	SModelData LoadFile(Model *state,const string& directoryPath);

	MaterialData LoadMaterialTemplateFile(const string& directoryPath, const string& filename);


	SModelData ModelData_ = {};

	ResourcePeroperty resource_ = {};

	Microsoft::WRL::ComPtr < ID3D12Resource> CameraResource;

	Camera* cameraData;

	Microsoft::WRL::ComPtr < ID3D12Resource> directionalLightResource;

	DirectionalLight* directionalLightData;
};



