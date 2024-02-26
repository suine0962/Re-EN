#include "Particle.h"


Particle::Particle() {}

void Particle::Initialize(const Vector4& color)
{
	

	// Sprite用の頂点リソースを作る
	vertexResourceSprite_ = CreateResources::CreateBufferResource(sizeof(VertexData) * 4);
	
	//頂点バッファビューを作る
	vertexBufferViewSprite_.BufferLocation= vertexResourceSprite_->GetGPUVirtualAddress();


	//instancing用のTransformationMatrixリソースを作る
	instancingResorce =
		CreateResources::CreateBufferResource(sizeof(TransformationMatrix) * kNunInstance);
	//書き込むためのアドレスを取得
	TransformationMatrix* instancingData = nullptr;
	instancingResorce->Map(0, nullptr, reinterpret_cast<void**>(&instancingData));


	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc{};

	instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingSrvDesc.Buffer.FirstElement = 0;
	instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingSrvDesc.Buffer.NumElements = kNunInstance;
	instancingSrvDesc.Buffer.StructureByteStride = sizeof(TransformationMatrix);

	D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU = DirectXCommon::GetInstance()->GetSrvHeap()->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU = DirectXCommon::GetInstance()->GetSrvHeap()->GetGPUDescriptorHandleForHeapStart();




	DirectXCommon::GetInstance()->GetDevice()->CreateShaderResourceView(instancingResorce.Get(), &instancingSrvDesc, instancingSrvHandleCPU);

	

}


void Particle::Draw(uint32_t texture, const Vector4& color, ViewProjection view)
{
	
	//頂点バッファビュー　
	// Deraに必要な関数
	//資料18ページ
	// 
	//単位行列を書き込んでおく
	for (uint32_t index = 0; index < kNunInstance; ++index) {
		instancingData[index].WVP = MatrixTransform::Identity();
		instancingData[index].World = MatrixTransform::Identity();
	}

}

void Particle::Release()
{
}

