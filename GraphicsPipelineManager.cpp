#include "GraphicsPipelineManager.h"

GraphicsPipelineManager* GraphicsPipelineManager::GetInstance()
{
	static GraphicsPipelineManager instance;
	return &instance;
}

void GraphicsPipelineManager::Initialize()
{
	CreatePSO();


}

void GraphicsPipelineManager::CreatePSO()
{
	ComPtr<ID3D12Device> device = DirectXCommon::GetInstance()->GetDevice();
	Commands commands = DirectXCommon::GetInstance()->GetCommands();
	SShaders shader = ShaderManager::Getinstance()->GetShader();
	PSO pso;

	pso.shape=CreateShapePSO(device.Get(), commands, shader.shape);
	LogManager::CompliteLog("CreateShapePSO");

	pso.Line = CreateLinePSO(device.Get(), commands, shader.shape);
	LogManager::CompliteLog("CreateLinePSO");

	pso.Sprite = CreateSpritePSO(device.Get(), commands, shader.sprite);
	LogManager::CompliteLog("CreateSpritePSO");

	pso.particle = CreateParticlePSO(device.Get(), commands, shader.particle);
	LogManager::CompliteLog("CreateParticlePSO");

	pso.Light = CreateDirectionalLightPSO(device.Get(), commands, shader.Light);
	LogManager::CompliteLog("CreateDirectionalLightPSO");

	GraphicsPipelineManager::GetInstance()->pso = pso;
}

PSOProperty GraphicsPipelineManager::CreateShapePSO(ComPtr<ID3D12Device> device, Commands command, SShaderMode shader)
{
	PSOProperty result;


	//RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};

	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//Material設定
	D3D12_ROOT_PARAMETER rootParameters[2] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].Descriptor.ShaderRegister = 0;
	//VertexのTransform
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].Descriptor.ShaderRegister = 0;


	descriptionRootSignature.pParameters = rootParameters;
	descriptionRootSignature.NumParameters = _countof(rootParameters);

	//シリアライズしてバイナリにする

	HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &result.signatureBlob, &result.errorBlob);
	if (FAILED(hr))
	{
		LogManager::Log(reinterpret_cast<char*>(result.errorBlob->GetBufferPointer()));
		assert(false);
	}

	//バイナリを元に生成

	hr = device->CreateRootSignature(
		0,
		result.signatureBlob->GetBufferPointer(),
		result.signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&result.rootSignature)
	);
	assert(SUCCEEDED(hr));

	//InputLayoutの設定
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[1] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);


	//BlendStateの設定を行う
	D3D12_BLEND_DESC blendDesc{};
	//すべての色要素を書き込む
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = blendDesc.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	blenddesc.BlendEnable = true;

	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	//半透明合成設定
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	

	//RasterrizerStateぼ設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};

	//裏面（時計回り）を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_FRONT;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;


	D3D12_DEPTH_STENCIL_DESC despthStencilDesc{};
	despthStencilDesc.DepthEnable = true;
	despthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	despthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;


	//PSOの生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};

	graphicsPipelineStateDesc.pRootSignature = result.rootSignature.Get(); //RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc; //InputLayout
	graphicsPipelineStateDesc.VS = { shader.vertexBlob->GetBufferPointer(),
	shader.vertexBlob->GetBufferSize() }; //VertexShader
	graphicsPipelineStateDesc.PS = { shader.pixelBlob->GetBufferPointer(),
	shader.pixelBlob->GetBufferSize() }; //PixeShader
	graphicsPipelineStateDesc.BlendState = blendDesc; //BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc; //RasterizerState

	graphicsPipelineStateDesc.DepthStencilState = despthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	//利用するトポロジ(形状)のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//どのように画面に色を打ち込むかの設定(気にしなくて良い)
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&result.GraphicsPipelineState));
	assert(SUCCEEDED(hr));



	return result;
}

PSOProperty GraphicsPipelineManager::CreateLinePSO(ComPtr<ID3D12Device>device, Commands commands, SShaderMode shader)
{
	PSOProperty result;


	//RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};

	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//Material設定
	D3D12_ROOT_PARAMETER rootParameters[2] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].Descriptor.ShaderRegister = 0;
	//VertexのTransform
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].Descriptor.ShaderRegister = 0;


	descriptionRootSignature.pParameters = rootParameters;
	descriptionRootSignature.NumParameters = _countof(rootParameters);

	//シリアライズしてバイナリにする

	HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &result.signatureBlob, &result.errorBlob);
	if (FAILED(hr))
	{
		LogManager::Log(reinterpret_cast<char*>(result.errorBlob->GetBufferPointer()));
		assert(false);
	}

	//バイナリを元に生成

	hr = device->CreateRootSignature(
		0,
		result.signatureBlob->GetBufferPointer(),
		result.signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&result.rootSignature)
	);
	assert(SUCCEEDED(hr));

	//InputLayoutの設定
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[1] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);


	//BlendStateの設定を行う
	D3D12_BLEND_DESC blendDesc{};
	//すべての色要素を書き込む
	//blendDesc.RenderTarget[0].RenderTargetWriteMask =
	//	D3D12_COLOR_WRITE_ENABLE_ALL;
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = blendDesc.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	blenddesc.BlendEnable = true;
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	//半透明合成設定
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	//RasterrizerStateぼ設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};

	//裏面（時計回り）を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_FRONT;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;


	D3D12_DEPTH_STENCIL_DESC despthStencilDesc{};
	despthStencilDesc.DepthEnable = true;
	despthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	despthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;


	//PSOの生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};

	graphicsPipelineStateDesc.pRootSignature = result.rootSignature.Get(); //RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc; //InputLayout
	graphicsPipelineStateDesc.VS = { shader.vertexBlob->GetBufferPointer(),
	shader.vertexBlob->GetBufferSize() }; //VertexShader
	graphicsPipelineStateDesc.PS = { shader.pixelBlob->GetBufferPointer(),
	shader.pixelBlob->GetBufferSize() }; //PixeShader
	graphicsPipelineStateDesc.BlendState = blendDesc; //BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc; //RasterizerState

	graphicsPipelineStateDesc.DepthStencilState = despthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	//利用するトポロジ(形状)のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

	//どのように画面に色を打ち込むかの設定(気にしなくて良い)
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&result.GraphicsPipelineState));
	assert(SUCCEEDED(hr));



	return result;
}

PSOProperty GraphicsPipelineManager::CreateSpritePSO(ComPtr<ID3D12Device> device, Commands commands, SShaderMode shader)
{
	PSOProperty SpritePSO;
	
	//RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};

	//descriptionRootSignature = CreateDescriptRootSignature();
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//Material設定
	D3D12_ROOT_PARAMETER rootParameters[3] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].Descriptor.ShaderRegister = 0;

	//VertexのTransform
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].Descriptor.ShaderRegister = 0;


	//DescriptorRanged
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//rootPrameterに入れる
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);


	//Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;

	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplers[0].ShaderRegister = 0;
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);


	descriptionRootSignature.pParameters = rootParameters;
	descriptionRootSignature.NumParameters = _countof(rootParameters);

	//シリアライズしてバイナリにする


	HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &SpritePSO.signatureBlob, &SpritePSO.errorBlob);
	if (FAILED(hr))
	{
		LogManager::Log(reinterpret_cast<char*>(SpritePSO.errorBlob->GetBufferPointer()));
		assert(false);
	}

	//バイナリを元に生成

	hr = device->CreateRootSignature(0, SpritePSO.signatureBlob->GetBufferPointer(),
		SpritePSO.signatureBlob->GetBufferSize(), IID_PPV_ARGS(&SpritePSO.rootSignature));
	assert(SUCCEEDED(hr));





	//InputLayoutの設定
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;


	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);


	//BlendStateの設定を行う
	D3D12_BLEND_DESC blendDesc{};
	//すべての色要素を書き込む
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = blendDesc.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	blenddesc.BlendEnable = true;
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	//半透明合成設定
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;


	//RasterrizerStateぼ設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};

	//裏面（時計回り）を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	D3D12_DEPTH_STENCIL_DESC despthStencilDesc{};
	despthStencilDesc.DepthEnable = true;
	despthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	despthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;



	//PSOの生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};

	graphicsPipelineStateDesc.pRootSignature = SpritePSO.rootSignature.Get(); //RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc; //InputLayout
	graphicsPipelineStateDesc.VS = { shader.vertexBlob->GetBufferPointer(),
	shader.vertexBlob->GetBufferSize() }; //VertexShader
	graphicsPipelineStateDesc.PS = { shader.pixelBlob->GetBufferPointer(),
	shader.pixelBlob->GetBufferSize() }; //PixeShader
	graphicsPipelineStateDesc.BlendState = blendDesc; //BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc; //RasterizerState
	graphicsPipelineStateDesc.DepthStencilState = despthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;


	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	//利用するトポロジ(形状)のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//どのように画面に色を打ち込むかの設定(気にしなくて良い)
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&SpritePSO.GraphicsPipelineState));
	assert(SUCCEEDED(hr));

	return SpritePSO;
}



PSOProperty GraphicsPipelineManager::CreateParticlePSO(ComPtr<ID3D12Device> device, Commands commands, SShaderMode shader)
{
	PSOProperty ParticlePSO;

	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};

	D3D12_DESCRIPTOR_RANGE descriptorRange2_[1] = {};

	//descriptionRootSignature = CreateDescriptRootSignature();
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//Material設定
	D3D12_ROOT_PARAMETER rootParameters[3] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].Descriptor.ShaderRegister = 0;

	descriptorRange2_[0].BaseShaderRegister = 0;//0から始まる
	descriptorRange2_[0].NumDescriptors = 1;//数は一つ
	descriptorRange2_[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	descriptorRange2_[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

	//DescriptorRanged
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	//VertexのTransform
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameters[1].DescriptorTable.NumDescriptorRanges =_countof(descriptorRange);


	//rootPrameterに入れる
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange2_;
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange2_);

	/*rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[3].Descriptor.ShaderRegister = 1;*/


	//Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;

	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplers[0].ShaderRegister = 0;
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	descriptionRootSignature.pParameters = rootParameters;
	descriptionRootSignature.NumParameters = _countof(rootParameters);
	descriptionRootSignature.pStaticSamplers= staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	//シリアライズしてバイナリにする


	HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &ParticlePSO.signatureBlob, &ParticlePSO.errorBlob);
	if (FAILED(hr))
	{
		LogManager::Log(reinterpret_cast<char*>(ParticlePSO.errorBlob->GetBufferPointer()));
		assert(false);
	}

	//バイナリを元に生成

	hr = device->CreateRootSignature(0, ParticlePSO.signatureBlob->GetBufferPointer(),
		ParticlePSO.signatureBlob->GetBufferSize(), IID_PPV_ARGS(&ParticlePSO.rootSignature));
	assert(SUCCEEDED(hr));





	//InputLayoutの設定
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset =
		D3D12_APPEND_ALIGNED_ELEMENT;


	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);


	//BlendStateの設定を行う
	D3D12_BLEND_DESC blendDesc{};
	//すべての色要素を書き込む
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = blendDesc.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;

	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	//RasterrizerStateぼ設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};

	//裏面（時計回り）を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	D3D12_DEPTH_STENCIL_DESC despthStencilDesc{};
	despthStencilDesc.DepthEnable = true;
	despthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	despthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;



	//PSOの生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};

	graphicsPipelineStateDesc.pRootSignature =ParticlePSO.rootSignature.Get(); //RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc; //InputLayout
	graphicsPipelineStateDesc.VS = { shader.vertexBlob->GetBufferPointer(),
	shader.vertexBlob->GetBufferSize() }; //VertexShader
	graphicsPipelineStateDesc.PS = { shader.pixelBlob->GetBufferPointer(),
	shader.pixelBlob->GetBufferSize() }; //PixeShader
	graphicsPipelineStateDesc.BlendState = blendDesc; //BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc; //RasterizerState

	//DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = despthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;


	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;;

	//利用するトポロジ(形状)のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//どのように画面に色を打ち込むかの設定(気にしなくて良い)
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&ParticlePSO.GraphicsPipelineState));
	assert(SUCCEEDED(hr));

	return ParticlePSO;
}

PSOProperty GraphicsPipelineManager::CreateDirectionalLightPSO(ComPtr<ID3D12Device> device, Commands commands, SShaderMode shader)
{
	PSOProperty DirectionalLightPSO;

	//RootSignature
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};

	//descriptionRootSignature = CreateDescriptRootSignature();
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//Material
	D3D12_ROOT_PARAMETER rootParameters[5] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].Descriptor.ShaderRegister = 0;

	//VertexTransform
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].Descriptor.ShaderRegister = 0;


	//DescriptorRanged
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//rootPrameter
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);

	//rootPrameter
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[3].Descriptor.ShaderRegister = 1;

	//rootPrameter
	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[4].Descriptor.ShaderRegister = 2;


	//Sampler
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;

	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplers[0].ShaderRegister = 0;
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);


	descriptionRootSignature.pParameters = rootParameters;
	descriptionRootSignature.NumParameters = _countof(rootParameters);

	


	HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &DirectionalLightPSO.signatureBlob, &DirectionalLightPSO.errorBlob);
	if (FAILED(hr))
	{
		LogManager::Log(reinterpret_cast<char*>(DirectionalLightPSO.errorBlob->GetBufferPointer()));
		assert(false);
	}


	hr = device->CreateRootSignature(0, DirectionalLightPSO.signatureBlob->GetBufferPointer(),
		DirectionalLightPSO.signatureBlob->GetBufferSize(), IID_PPV_ARGS(&DirectionalLightPSO.rootSignature));
	assert(SUCCEEDED(hr));


	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);


	//BlendState
	D3D12_BLEND_DESC blendDesc{};
	//
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;


	//RasterrizerState
	D3D12_RASTERIZER_DESC rasterizerDesc{};

	//
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	D3D12_DEPTH_STENCIL_DESC despthStencilDesc{};
	despthStencilDesc.DepthEnable = true;
	despthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	despthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	//PSO作成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};

	graphicsPipelineStateDesc.pRootSignature = DirectionalLightPSO.rootSignature.Get(); //RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc; //InputLayout
	graphicsPipelineStateDesc.VS = { shader.vertexBlob->GetBufferPointer(),
	shader.vertexBlob->GetBufferSize() }; //VertexShader
	graphicsPipelineStateDesc.PS = { shader.pixelBlob->GetBufferPointer(),
	shader.pixelBlob->GetBufferSize() }; //PixeShader
	graphicsPipelineStateDesc.BlendState = blendDesc; //BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc; //RasterizerState
	graphicsPipelineStateDesc.DepthStencilState = despthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//RTV
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	//
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&DirectionalLightPSO.GraphicsPipelineState));
	assert(SUCCEEDED(hr));

	return DirectionalLightPSO;
}





