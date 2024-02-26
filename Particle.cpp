#include "Particle.h"


Particle::Particle() {}

void Particle::Initialize(const Vector4& color)
{
	directX = DirectXCommon::GetInstance();
	
	textureManager_ = TextureManager::GetInstance();
	// Sprite用の頂点リソースを作る
	vertexResourceSprite_ = CreateResources::CreateBufferResource(sizeof(VertexData) * 4);
	
	//頂点バッファビューを作る
	vertexBufferViewSprite_.BufferLocation= vertexResourceSprite_->GetGPUVirtualAddress();

	// 使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferViewSprite_.SizeInBytes = sizeof(VertexData) * 4;
	// 1頂点あたりのサイズ
	vertexBufferViewSprite_.StrideInBytes = sizeof(VertexData);

	vertexResourceSprite_->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite_));

	// 1枚目の三角形
	vertexDataSprite_[0].position = { -1.0f,-1.0f,0.0f,1.0f };//左下
	vertexDataSprite_[0].texcoord = { 0.0f,1.0f };
	vertexDataSprite_[1].position = { -1.0f,1.0f,0.0f,1.0f }; // 左上
	vertexDataSprite_[1].texcoord = { 0.0f,0.0f };
	vertexDataSprite_[2].position = { 1.0f,-1.0f,0.0f,1.0f }; // 右下
	vertexDataSprite_[2].texcoord = { 1.0f,1.0f };

	vertexDataSprite_[3].position = { 1.0f,1.0f,0.0f,1.0f }; // 右上
	vertexDataSprite_[3].texcoord = { 1.0f,0.0f };


	// 実際に頂点リソースを作る
	materialResource =CreateResources::CreateBufferResource(sizeof(Material));

	/*materialBufferView = CreateBufferView();;*/
	// 頂点リソースにデータを書き込む
	materialData = nullptr;
	// 書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	// 色のデータを変数から読み込み
	materialData->color = color;
	materialData->uvTransform = MatrixTransform::MakeIdenttity4x4();
	//materialData->enableLighting = true;

	// Transform変数の初期化

	// Sprite用のTransformationMatrix用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	transformationMatrixResouceSprite = CreateResources::CreateBufferResource(sizeof(ParticleForGPU) * 10);
	// 書き込むためのアドレスを取得
	transformationMatrixResouceSprite->Map(0, nullptr, reinterpret_cast<void**>(&instancingData));
	// 単位行列を書き込んでおく

	for (uint32_t index = 0; index < kNumMaxInstance; ++index) {
		instancingData[index].WVP =MatrixTransform::MakeIdenttity4x4();
		instancingData[index].World = MatrixTransform::MakeIdenttity4x4();
		//instancingData[index].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	}



	indexResourceSprite = CreateResources::CreateBufferResource(sizeof(uint32_t) * 6);
	// リソースの先頭のアドレスから使う
	indexBufferViewSprite.BufferLocation = indexResourceSprite->GetGPUVirtualAddress();
	// 使用するリソースのサイズはインデックス6つ分のサイズ
	indexBufferViewSprite.SizeInBytes = sizeof(uint32_t) * 6;
	// インデックスはuint_tとする
	indexBufferViewSprite.Format = DXGI_FORMAT_R32_UINT;

	// インデックスリソースにデータを書き込む
	uint32_t* indexDataSprite = nullptr;
	indexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));
	indexDataSprite[0] = 0;
	indexDataSprite[1] = 1;
	indexDataSprite[2] = 2;
	indexDataSprite[3] = 1;
	indexDataSprite[4] = 3;
	indexDataSprite[5] = 2;


	//std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);
	//for (uint32_t index = 0; index < kNumMaxInstance; ++index) {
	//	//particles_[index] = MakeNewParticle(randomEngine);
	//	
	//}




	instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingSrvDesc.Buffer.FirstElement = 0;
	instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingSrvDesc.Buffer.NumElements = kNumMaxInstance;
	instancingSrvDesc.Buffer.StructureByteStride = sizeof(ParticleForGPU);

	D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU = directX->GetSrvHeap()->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU = directX->GetSrvHeap()->GetGPUDescriptorHandleForHeapStart();

	//// SRVを作成するDescriptorHeapの場所を決める
	//instancingSrvHandleCPU = sDirectXCommon->GetSrvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
	//instancingSrvHandleGPU = sDirectXCommon->GetSrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
	//// 先頭はImGuiが使っているのでその次を使う
	//instancingSrvHandleCPU.ptr += sDirectXCommon->GetDevice()->(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * TextureManager::index_;
	//instancingSrvHandleGPU.ptr += sDirectXCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * TextureManager::index_;
	// 

	//instancingSrvHandleCPU = directX->GetSrvHeap().Get().(sDirectXCommon->GetSrvDescriptorHeap().Get(), sDirectXCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV), TextureManager::index_);
	//instancingSrvHandleGPU = directX->GetDevice().GetGPUDescriptorHandle(sDirectXCommon->GetSrvDescriptorHeap().Get(), sDirectXCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV), TextureManager::index_);
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

	
	directX->GetDevice()->CreateShaderResourceView(instancingResorce.Get(), &instancingSrvDesc, instancingSrvHandleCPU);

}


void Particle::Draw(uint32_t texture, const Vector4& color, ViewProjection view)
{
	pso_ = GraphicsPipelineManager::GetInstance();
	Commands commands = DirectXCommon::GetInstance()->GetCommands();
	materialData->color = color;

	WorldTransform Transfroms[kNumMaxInstance];

	for (uint32_t index = 0; index < kNumMaxInstance; ++index)
	{
		Transfroms[index].scale = { 1.0f,1.0f,1.0f };
		Transfroms[index].rotation = { 0.0f,0.0f,0.0f };
		Transfroms[index].translate = { index * 0.1f,index * 0.1f,index * 0.1f };
	}



	for (uint32_t index = 0; index < kNumMaxInstance; ++index)
	{

		//materialData->uvTransform = MatrixTransform::AffineMatrix()

		Matrix4x4 WorldMatrix =
			MatrixTransform::AffineMatrix((Transfroms[index].scale), (Transfroms[index].rotation), (Transfroms[index].rotation));

		Matrix4x4 viewMatrix =
			MatrixTransform::AffineMatrix(cameraTransform.scale,cameraTransform.rotation,cameraTransform.translate);

		Matrix4x4 cameraMatrix =
			MatrixTransform::Inverse(viewMatrix);

		Matrix4x4 projectionMatrix =
			MatrixTransform::PerspectiveFovMatrix(0.45f,
				float(WinApp::GetkCilientWidth()) / float(WinApp::GetkCilientHeight()),0.1f,100.0f);

		Matrix4x4 worldViewProjectionMatrix = MatrixTransform::Multiply(WorldMatrix,
			MatrixTransform::Multiply(viewMatrix, projectionMatrix));

		instancingData[index].WVP = worldViewProjectionMatrix;
		instancingData[index].World = WorldMatrix;

	}


	commands.m_pList->SetGraphicsRootSignature(pso_->GetInstance()->GetPso().particle.rootSignature.Get());
	commands.m_pList->SetPipelineState(pso_->GetInstance()->GetPso().particle.GraphicsPipelineState.Get());//psoを設定
	commands.m_pList->IASetVertexBuffers(0, 1, &vertexBufferViewSprite_);
	commands.m_pList->IASetIndexBuffer(&indexBufferViewSprite);
	//形状を設定 psoに関しては設定しているものとは別.同じものを設定すると考えておけばいい
	commands.m_pList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//マテリアルCBufferの場所を設定
	commands.m_pList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	//TransformationCBufferの場所を設定
	commands.m_pList->SetGraphicsRootDescriptorTable(1, instancingSrvHandleGPU);
	//SRVのDescriptorTableの先頭を設定 2はrootParameter[2]である
	textureManager_->texCommand(texture);
	//directX->GetCommands().m_pList->SetGraphicsRootConstantBufferView(3,)
	commands.m_pList->DrawIndexedInstanced(6, kNumMaxInstance, 0, 0, 0);
	// 
	// Deraに必要な関数
	//資料18ページ
	// 
	//単位行列を書き込んでおく
	//for (uint32_t index = 0; index < kNunInstance; ++index) {
		//instancingData[index].WVP = MatrixTransform::Identity();
		//instancingData[index].World = MatrixTransform::Identity();
	//}

}

void Particle::Release()
{
}

