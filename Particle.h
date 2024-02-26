#pragma once
#include "GraphicsPipelineManager.h"

#include "DirectXTex/DirectXTex/DirectXTex.h"
#include <string>
#include <format>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <dxcapi.h>
#include <random>

#include <wrl.h>
#include "Vector4.h"
#include"Vector3.h"
#include"Vector2.h"
#include "Matrix4x4.h"
#include "MatrixTransform.h"

#include "CreateResource.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "TextureManager.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxcompiler.lib")
class WinApp;
class DirectXCommon;
class TextureManager;

struct ParticleForGPU {
	Matrix4x4 WVP;
	Matrix4x4 World;
	//Vector4 color;
};

struct Emitter {
	WorldTransform transform; //!< エミッタのTransform
	uint32_t count; //!< 発生数
	float frequency; //!< 発生頻度
	float frequencyTime; //!< 頻度用時刻
};

struct Particle_param
{
	WorldTransform worldTransform_;
	WorldTransform uvTransform_;
	Vector4 color_{ 1,1,1,1 };
	bool isAlive = false;
	Vector3 Velocity{};
};
class Particle
{
public:
	struct ParticlePro { // プロパティ
		WorldTransform transform;
		Vector3 velocity;
		Vector4 color;
		float lifeTime;
		float currentTime;
	};


	Particle();
	~Particle();

	void Initialize(const Vector4& color);
	//void Update();
	void Draw(uint32_t texture, const Vector4& color, ViewProjection view);
	void Release();
	void SetTextureManager(TextureManager* textureManager) {
		textureManager_ = textureManager;
	}
	ParticlePro MakeNewParticle(std::mt19937& randomEngine);

	std::list<ParticlePro> Emission(const Emitter& emitter, std::mt19937& randEngine);
	D3D12_VERTEX_BUFFER_VIEW CreateBufferView();
private:
	const uint32_t kNunInstance = 10;//インスタンす数

	const static uint32_t kNumMaxInstance = 10; // インスタンス数
	// Instancing用のTransformMatrixリソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> instancingResorce = nullptr;
	GraphicsPipelineManager* pso_ = nullptr;
	Microsoft::WRL::ComPtr < ID3D12Resource> vertexResourceSprite_ = nullptr;
	WinApp* Winapp;
	DirectXCommon* directX;
	WorldTransform cameraTransform;
	ViewProjection viewPro;
	TextureManager* textureManager_ = nullptr;
	// 頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite_{};

	VertexData* vertexDataSprite_ = nullptr;

	// Sprite用のTransformationMatrix用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	Microsoft::WRL::ComPtr < ID3D12Resource> transformationMatrixResouceSprite;
	// データを書き込む
	//TransformationMatrix* transformationMatrixDataSprite = nullptr;
	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc{};

	D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU;



	//D3D12_DESCRIPTOR_RANGE descriptorRange_[1] = {};

	// RootParmeter作成。複数でっていできるので配列。今回は結果１つだけなので長さ1の配列
	//D3D12_ROOT_PARAMETER rootParamerters[1] = {};

	uint32_t* indexDataSprite;
	Microsoft::WRL::ComPtr < ID3D12Resource> indexResourceSprite;
	D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};


	// 実際に頂点リソースを作る
	Microsoft::WRL::ComPtr <ID3D12Resource> materialResource;
	// 頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW materialBufferView{};
	// 頂点リソースにデータを書き込む
	Material* materialData;
	std::list<ParticlePro> particles_;
	//ParticlePro particles_[kNumMaxInstance];
	std::list<WorldTransform>  transforms_;
	ParticleForGPU* instancingData = nullptr;
	// 平行光源用
	//Microsoft::WRL::ComPtr < ID3D12Resource> directionalLightResource;
	// データを書き込む
	//DirectionalLight* directionalLightData;
	//WorldTransform transformUv;



	// Δtを定義。とりあえず60fps固定してあるが、
	//実時間を計測して可変fpsで動かせるようにしておくとなお良い
	const float kDeltaTime = 1.0f / 60.0f;
	PSOProperty PSO = {};

};

