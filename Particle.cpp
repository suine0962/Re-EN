#include "Particle.h"
#include "MatrixTransform.h"

Particle::Particle() {};

void Particle::Initilize(Vector4 color)
{
	// Sprite用の頂点リソースを作る
	vertexResourceSprite_= CreateResources::CreateBufferResource(sizeof(VertexData) * 4);



}

void Particle::Draw(uint32_t texture, const Vector4& color, ViewProjection view)
{
	const uint32_t kNunInstance = 10;//インスタンす数

	//instancing用のTransformationMatrixリソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource>instancingResource =
		CreateResources::CreateBufferResource(sizeof(TransformationMatrix) * kNunInstance);
	//書き込むためのアドレスを取得
	TransformationMatrix* instancingData = nullptr;
	instancingResource->Map(0, nullptr, reinterpret_cast<void**>(&instancingData));

	//単位行列を書き込んでおく
	for (uint32_t index = 0; index < kNunInstance; ++index) {
		instancingData[index].WVP = MatrixTransform::MakeIdenttity4x4();
		instancingData[index].world =MatrixTransform:: MakeIdenttity4x4();
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc{};

	instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingSrvDesc.Buffer.FirstElement = 0;
	instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingSrvDesc.Buffer.NumElements = kNunInstance;
	instancingSrvDesc.Buffer.StructureByteStride = sizeof(TransformationMatrix);

	D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU = DirectXCommon::GetInstance()->GetSrvHeap().

}

void Particle::Release()
{
}

