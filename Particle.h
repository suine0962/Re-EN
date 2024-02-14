#pragma once
#include "CreateResource.h"
#include "Model.h"
#include "Sprite.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "ViewProjection.h"
#include "MatrixTransform.h"
#include "ImGuiManager.h"
#include "TextureManager.h"
//#include "ParticlePipelineManager.h"

#include "CreateResource.h"
#include <numbers>


struct ParticleGPU
{
	Matrix4x4 WVP;
	Matrix4x4 World;
	Vector4 color;
};

struct Emitter
{
	WorldTransform transform;//エミッターのtransform
	uint32_t count;//発生数
	float frequency;//発生頻度
	float frequencyTime;//発生頻度を決めるタイマー

};

class Particle
{
public: 
	Particle();
	~Particle();

	struct ParticleProperty
	{
		WorldTransform transform;
		Vector3 velocity;
		Vector4 color;
		float lifeTime;
		float currentTime;

	};



	void Initilize(Vector4 color);

	void Draw(uint32_t texture,const Vector4 &color,ViewProjection view);

	void Release();

	void SetTexManager(TextureManager* texManager)
	{
		textureManager_ = texManager;
	}

	ParticleProperty NewParticle();

private:
	GraphicsPipelineManager* particlePSO_=nullptr;
	TextureManager* textureManager_ = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite_{};
	Microsoft::WRL::ComPtr<ID3D12Resource>transformationMatrixSprite;

	VertexData* vertexDataSprite_=nullptr;
	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc{};

	D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU;

	Microsoft::WRL::ComPtr<ID3D12Resource>vertexResourceSprite_=nullptr;

};

