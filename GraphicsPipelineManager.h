#pragma once
#include"LogManager.h"
#include"DirectXCommon.h"
#include"ShaderManager.h"


struct PSOProperty
{
	ComPtr<ID3D12PipelineState> GraphicsPipelineState = nullptr;
	ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	ComPtr<ID3DBlob> signatureBlob = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;

};

struct PSO
{
	PSOProperty shape;
	PSOProperty Line;
	PSOProperty Sprite;
	PSOProperty particle;
	PSOProperty Light;
};

class GraphicsPipelineManager
{
public:

	static GraphicsPipelineManager* GetInstance();

	static void Initialize();
    PSO GetPso() { return pso; }

private:

	static void CreatePSO();
	static  PSOProperty CreateShapePSO(ComPtr<ID3D12Device>device,Commands command,SShaderMode shader);
	static PSOProperty CreateLinePSO(ComPtr<ID3D12Device>device, Commands commands, SShaderMode shader);
	static PSOProperty CreateSpritePSO(ComPtr<ID3D12Device>device, Commands commands, SShaderMode shader);
	static PSOProperty CreateParticlePSO(ComPtr<ID3D12Device>device, Commands commands, SShaderMode shader);
	static PSOProperty CreateDirectionalLightPSO(ComPtr<ID3D12Device>device, Commands commands, SShaderMode shader);

	PSO pso = {};

};

