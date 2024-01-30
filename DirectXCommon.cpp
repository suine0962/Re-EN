#include "DirectXCommon.h"

DirectXCommon* DirectXCommon::GetInstance(){
	static DirectXCommon instance;
	return &instance;
}

void DirectXCommon::initialize()
{
#ifdef _DEBUG
	
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&DirectXCommon::GetInstance()->m_pDebugController))))
	{
		DirectXCommon::GetInstance()->m_pDebugController->EnableDebugLayer();
		DirectXCommon::GetInstance()->m_pDebugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif
	CreateFactory();
	CreateDevice();

#ifdef _DEBUG
	ComPtr<ID3D12InfoQueue> infoQueue = nullptr;

	if (SUCCEEDED(DirectXCommon::GetInstance()->m_pDevice_->QueryInterface(IID_PPV_ARGS(&infoQueue))))
	{
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
		D3D12_MESSAGE_ID denyIds[] =
		{
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		infoQueue->PushStorageFilter(&filter);
	}
#endif _DEBUG
	CreateCommands();
	CreateSwapChain();
	CreateDescritorHeap();
	CreateSwapChainResource();
    CreateRTV();
	CreateFence();
}

void DirectXCommon::Finalize()
{
	CloseHandle(DirectXCommon::GetInstance()->fenceEvent);
#ifdef _DEBUG
	DirectXCommon::GetInstance()->m_pDebugController->Release();
#endif
}

void DirectXCommon::BeginFlame()
{
	SwapChain swapChain = DirectXCommon::GetInstance()->swapChain;
	Commands commands = DirectXCommon::GetInstance()->commands;

	UINT backBufferIndex = swapChain.m_pSwapChain->GetCurrentBackBufferIndex();
	D3D12_RESOURCE_BARRIER barrier{};

	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = swapChain.m_pResource[backBufferIndex].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

	commands.m_pList.Get()->ResourceBarrier(1, &barrier);
	DirectXCommon::GetInstance()->barrier = barrier;
	
	//ClearScreen
	commands.m_pList.Get()->OMSetRenderTargets(1, &DirectXCommon::GetInstance()->rtv.rtvHandles[backBufferIndex], false, nullptr);
	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };
	commands.m_pList.Get()->ClearRenderTargetView(DirectXCommon::GetInstance()->rtv.rtvHandles[backBufferIndex], clearColor, 0, nullptr);

	//Depth
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = DirectXCommon::GetInstance()->m_pDsvDescripterHeap->GetCPUDescriptorHandleForHeapStart();
	commands.m_pList.Get()->OMSetRenderTargets(1, &DirectXCommon::GetInstance()->rtv.rtvHandles[backBufferIndex], false, &dsvHandle);
	commands.m_pList.Get()->ClearDepthStencilView(dsvHandle,D3D12_CLEAR_FLAG_DEPTH,1.0f,0,0,nullptr);
	ScissorViewCommand(WinApp::GetkCilientWidth(), WinApp::GetkCilientHeight());

	
}

void DirectXCommon::InitilizeFixFPS()
{
	reference_= std::chrono::steady_clock::now();
}

void DirectXCommon::UpdataFPS()
{
	const std::chrono::microseconds kminTime(uint64_t(1000000.0f / 60.0f));
	const std::chrono::microseconds kminCheckTime(uint64_t(1000000.0f / 65.0f));

	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	std::chrono::microseconds elapsed =
		std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

	//1/60秒より（わずかに短い時間）経っていない場合
	if (elapsed < kminTime)
	{
		//1/60秒経過するまで微小なスリープを繰り返す
		while (std::chrono::steady_clock::now()-reference_<kminTime)
		{
			//1秒μスリープ
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}
	//現在の時間を記録する
	reference_ = std::chrono::steady_clock::now();
}

void DirectXCommon::ViewChange()
{
	HRESULT hr;
	//ImGuiの描画
	ImGuiManager::BeginFlame();
	// これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex =swapChain.m_pSwapChain->GetCurrentBackBufferIndex();
	//ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
	//画面に描く処理はすべて終わり、画面に移すので、状態を遷移
// 今回はRenderTargetからPresentにする
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	// TramsitionBarrierを張る
	commands.m_pList->ResourceBarrier(1, &barrier);

	// コマンドリストの内容を確定させる。すべてのコマンドを積んでからcloseすること
	hr = commands.m_pList->Close();
	assert(SUCCEEDED(hr));

	// GPUにコマンドリストの実行を行わせる
	ID3D12CommandList* commandLists[] = { commands.m_pList.Get() };
	commands.m_pQueue->ExecuteCommandLists(1, commandLists);

	// GPUとOSに画面の交換を行うよう通知する
	swapChain.m_pSwapChain->Present(1, 0);

	//Fenceの値を更新
	fenceValue++;
	// GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
	commands.m_pQueue->Signal(m_pFence_.Get(), fenceValue);

	// fenceの値が指定したSignal値にたどり着いているか確認する
	// GetCompletedValueの初期値はFence作成時に渡した初期値
	if (m_pFence_->GetCompletedValue() < fenceValue)
	{
		//指定したSignalにたどりついていないので、たどり着くまで待つようにイベントを設定する
		m_pFence_->SetEventOnCompletion(fenceValue, fenceEvent);
		//イベント待つ
		WaitForSingleObject(fenceEvent, INFINITE);
	}
	// FPS固定
	UpdataFPS();


	//次のフレーム用のコマンドリストを準備
	hr = commands.m_pAllocator->Reset();
	assert(SUCCEEDED(hr));
	hr = commands.m_pList->Reset(commands.m_pAllocator.Get(), nullptr);
	assert(SUCCEEDED(hr));


}


D3D12_VIEWPORT DirectXCommon::viewportSetting(int32_t kClientWidth, int32_t kClientHeight)
{

	D3D12_VIEWPORT viewport = {};

	//クライアント領域のサイズを一緒にして画面全体に表示
	viewport.Width = float(kClientWidth);
	viewport.Height = float(kClientHeight);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;



	return viewport;
}

D3D12_RECT DirectXCommon::scissorRectSetting(int32_t kClientWidth, int32_t kClientHeight)
{
	//シザー矩形
	D3D12_RECT scissorRect{};

	//基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect.left = 0;
	scissorRect.right = kClientWidth;
	scissorRect.top = 0;
	scissorRect.bottom = kClientHeight;


	return scissorRect;

}

void DirectXCommon::ScissorViewCommand(const int32_t kClientWidth, const int32_t kClientHeight)
{
	D3D12_VIEWPORT viewport{};

	viewport = viewportSetting(kClientWidth, kClientHeight);

	//シザー矩形
	D3D12_RECT scissorRect{};
	scissorRect = scissorRectSetting(kClientWidth, kClientHeight);

	//コマンドを積む
	Commands commands = DirectXCommon::GetInstance()->commands;

	commands.m_pList->RSSetViewports(1, &viewport); //
	commands.m_pList->RSSetScissorRects(1, &scissorRect);

}

void DirectXCommon::EndFlame()
{
	Commands commands = DirectXCommon::GetInstance()->commands;
	D3D12_RESOURCE_BARRIER barrier = DirectXCommon::GetInstance()->barrier;
	ComPtr<ID3D12Fence> fence = DirectXCommon::GetInstance()->m_pFence_;
	SwapChain swapChain = DirectXCommon::GetInstance()->swapChain;


	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	
	commands.m_pList->ResourceBarrier(1, &barrier);
	HRESULT hr = commands.m_pList->Close();
	assert(SUCCEEDED(hr));
	//コマンド実行
	ID3D12CommandList* commandLists[] = { commands.m_pList.Get() };
	commands.m_pQueue->ExecuteCommandLists(1, commandLists);

	swapChain.m_pSwapChain->Present(1, 0);
    DirectXCommon::GetInstance()->fenceValue++;
	//Event
	commands.m_pQueue->Signal(DirectXCommon::GetInstance()->m_pFence_.Get(), DirectXCommon::GetInstance()->fenceValue);
	if (DirectXCommon::GetInstance()->m_pFence_->GetCompletedValue() < DirectXCommon::GetInstance()->fenceValue) {
		HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		assert(fenceEvent != nullptr);
		DirectXCommon::GetInstance()->m_pFence_->SetEventOnCompletion(DirectXCommon::GetInstance()->fenceValue, fenceEvent);
		WaitForSingleObject(fenceEvent, INFINITE);
		CloseHandle(fenceEvent);
	}
	//コマンドリセット
	hr = commands.m_pAllocator->Reset();
	assert(SUCCEEDED(hr));
	hr = commands.m_pList->Reset(commands.m_pAllocator.Get(), nullptr);
	assert(SUCCEEDED(hr));
}

ComPtr<ID3D12DescriptorHeap>  DirectXCommon::CreateDescripterHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible)
{
	ComPtr<ID3D12DescriptorHeap> descriptHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	DirectXCommon::GetInstance()->m_pDevice_->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptHeap));
	return descriptHeap;
}

ComPtr<ID3D12Resource>DirectXCommon::CreateDepthStencilTextureResource()
{
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = WinApp::GetInstance()->GetkCilientWidth();
	resourceDesc.Height = WinApp::GetInstance()->GetkCilientHeight();
	resourceDesc.MipLevels = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	ComPtr<ID3D12Resource>resource = nullptr;
	DirectXCommon::GetInstance()->m_pDevice_->CreateCommittedResource(
		&heapProperties,				
		D3D12_HEAP_FLAG_NONE,			
		&resourceDesc,					
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,				
		IID_PPV_ARGS(&resource));		

	return resource;
}

void DirectXCommon::CreateFactory()
{

	//DXGIファクトリーの生成
	ComPtr<IDXGIFactory7>dxgiFactory = nullptr;
	ComPtr<	IDXGIAdapter4>useAdapter = nullptr;

	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	assert(SUCCEEDED(hr));

	for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i,
		DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) !=
		DXGI_ERROR_NOT_FOUND; i++)
	{
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr));

		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE))
		{
		   LogManager::Log(LogManager::ConvertString(std::format(L"Use Adapater:{}\n", adapterDesc.Description)));
			break;
		}
		useAdapter = nullptr;
	}
	assert(useAdapter != nullptr);

	DirectXCommon::GetInstance()->m_pDxgiFactory_ = dxgiFactory;
	DirectXCommon::GetInstance()->m_pUseAdapter_ = useAdapter;

}

void DirectXCommon::CreateDevice()
{
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
	};

	const char* featureLevelStrings[] = { "12.2","12.1","12.0" };

	for (size_t i = 0; i < _countof(featureLevels); i++)
	{
		HRESULT hr = D3D12CreateDevice(
			DirectXCommon::GetInstance()->m_pUseAdapter_.Get(),
			featureLevels[i],
			IID_PPV_ARGS(&DirectXCommon::GetInstance()->m_pDevice_));
		if (SUCCEEDED(hr))
		{
			LogManager::Log(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
			break;
		}
	}

	assert(DirectXCommon::GetInstance()->m_pDevice_ != nullptr);
}

void DirectXCommon::CreateCommands()
{
	ComPtr<ID3D12Device> device = DirectXCommon::GetInstance()->m_pDevice_;
	Commands commands = {};

	commands.m_pQueue = nullptr;
	HRESULT hr = device->CreateCommandQueue(&commands.QueueDesc, IID_PPV_ARGS(&commands.m_pQueue));
	assert(SUCCEEDED(hr));

	commands.m_pAllocator = nullptr;
	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commands.m_pAllocator));
	assert(SUCCEEDED(hr));

	commands.m_pList = nullptr;
	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		commands.m_pAllocator.Get(), nullptr,
		IID_PPV_ARGS(&commands.m_pList));
	assert(SUCCEEDED(hr));

	DirectXCommon::GetInstance()->commands = commands;
	DirectXCommon::GetInstance()->m_pDevice_ = device;
}

void DirectXCommon::CreateSwapChain()
{
	SwapChain swapChain = {};
	HWND hwnd_ = WinApp::GetInstance()->GetHwnd();
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};

	swapChainDesc.Width = WinApp::GetInstance()->GetkCilientWidth();
	swapChainDesc.Height = WinApp::GetInstance()->GetkCilientHeight();
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	DirectXCommon::GetInstance()->m_pDxgiFactory_.Get()->
		CreateSwapChainForHwnd(DirectXCommon::GetInstance()->commands.m_pQueue.Get(), hwnd_, &swapChainDesc,
		nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(&DirectXCommon::GetInstance()->swapChain));

	DirectXCommon::GetInstance()->swapChain.swapChainDesc = swapChainDesc;
}

void DirectXCommon::CreateDescritorHeap()
{
	DirectXCommon::GetInstance()->rtv.m_pDescritorHeap =
		CreateDescripterHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);
	DirectXCommon::GetInstance()->m_pSrvDescriptorHeap =
		CreateDescripterHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);
	DirectXCommon::GetInstance()->m_pDsvDescripterHeap =
		CreateDescripterHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

	DirectXCommon::GetInstance()->m_pDepthResource =
		CreateDepthStencilTextureResource();

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	DirectXCommon::GetInstance()->m_pDevice_->CreateDepthStencilView(
		DirectXCommon::GetInstance()->m_pDepthResource.Get(),&dsvDesc,
		DirectXCommon::GetInstance()->m_pDsvDescripterHeap->GetCPUDescriptorHandleForHeapStart()
	);
}

void DirectXCommon::CreateSwapChainResource()
{
	HRESULT hr = DirectXCommon::GetInstance()->swapChain.m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&DirectXCommon::GetInstance()->swapChain.m_pResource[0]));
	assert(SUCCEEDED(hr));

	hr = DirectXCommon::GetInstance()->swapChain.m_pSwapChain->GetBuffer(1, IID_PPV_ARGS(&DirectXCommon::GetInstance()->swapChain.m_pResource[1]));
	assert(SUCCEEDED(hr));
}

void DirectXCommon::CreateRTV()
{
	RTV rtv = DirectXCommon::GetInstance()->rtv;
	ComPtr<ID3D12Device> device = DirectXCommon::GetInstance()->m_pDevice_;
	SwapChain swapChain = DirectXCommon::GetInstance()->swapChain;

	rtv.rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtv.rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtv.rtvStartHandle = rtv.m_pDescritorHeap->GetCPUDescriptorHandleForHeapStart();
	rtv.rtvHandles[0] = rtv.rtvStartHandle;
	device->CreateRenderTargetView(swapChain.m_pResource[0].Get(), &rtv.rtvDesc,rtv.rtvHandles[0]);
	rtv.rtvHandles[1].ptr = rtv.rtvHandles[0].ptr + device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	device->CreateRenderTargetView(swapChain.m_pResource[1].Get(), &rtv.rtvDesc, rtv.rtvHandles[1]);

	DirectXCommon::GetInstance()->rtv = rtv;
}

void DirectXCommon::CreateFence()
{
	ComPtr<ID3D12Fence> fence = nullptr;
	uint64_t fenceValue = {};
	HANDLE fenceEvent = {};
	fenceValue = 0;

	DirectXCommon::GetInstance()->m_pDevice_->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent != nullptr);

	DirectXCommon::GetInstance()->m_pFence_ = fence;
	DirectXCommon::GetInstance()->fenceEvent = fenceEvent;
	DirectXCommon::GetInstance()->fenceValue = fenceValue;
}



