#include "Graphic_Device.h"
#include "VIBuffer_Geos.h"

CGraphic_Device::CGraphic_Device()
{
}

bool CGraphic_Device::Initialize(HWND windowHandle)
{
    ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&m_dxgiFactory)));            

    HRESULT hardwareResult = D3D12CreateDevice(                 // 만약 그래픽 카드가 여러개라면 설정해줘야함. 그래픽카드가 하나라고 가정한다.
        nullptr,
        D3D_FEATURE_LEVEL_11_0,
        IID_PPV_ARGS(&m_d3dDevice)
    );                                          

    /*if (FAILED(hardwareResult))                               // 실패할 경우 WRAP 디바이스를 사용합니다.
    {
        IDXGIAdapter* pWrapAdapter;
        ThrowIfFailed(m_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWrapAdapter)));

        ThrowIfFailed(D3D12CreateDevice(
            pWrapAdapter,
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&m_d3dDevice)));
    }*/                      

    ThrowIfFailed(m_d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE,
        IID_PPV_ARGS(&m_Fence)));

    m_RtvDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);                            //핸들의 크기를 미리 얻어놓는다. GPU마다 핸들의 크기는 다를 수 있다.
    m_DsvDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    m_CbvSrvUavDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    // 백 버퍼 포멧에 대해서 4X MSAA가 지원되는지 검사합니다.
    // 모든 Direct3D 11 디바이스 레벨은 모든 렌더 타겟 포멧에 4X MSAA를 지원합니다.
    // 그러므로 오직 퀄리티만 검사하면 됩니다.
    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
    msQualityLevels.Format = m_BackBufferFormat;
    msQualityLevels.SampleCount = 4;
    msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
    msQualityLevels.NumQualityLevels = 0;
    ThrowIfFailed(m_d3dDevice->CheckFeatureSupport(
        D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
        &msQualityLevels,
        sizeof(msQualityLevels)));

    m4xMsaaQuality = msQualityLevels.NumQualityLevels;
    assert(m4xMsaaQuality > 0 && "Unexpected MSAA quality level.");

#ifdef _DEBUG
    LogAdapters();                                                                  // 디버그 모드일 때, 어댑터 종류 나열하는 코드
#endif

    CreateCommandObjects();
    CreateSwapChain(windowHandle);
    CreateRtvAndDsvDescriptorHeaps();

    return true;
}

void CGraphic_Device::CreateCommandObjects()
{
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    ThrowIfFailed(m_d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CommandQueue)));

    ThrowIfFailed(m_d3dDevice->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS(&m_DirectCmdListAlloc)));

    ThrowIfFailed(m_d3dDevice->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        m_DirectCmdListAlloc,
        nullptr,
        IID_PPV_ARGS(&m_CommandList)));

    // 커맨드 리스트를 닫은 상태로 시작합니다.
    // 렌더링 하기에 앞서서 커맨드 리스트를 리셋하고 커맨드를 기록하는데,
    // 리셋을 하기 위해선 커맨드 리스트가 닫혀있는 상태여야 합니다.
    m_CommandList->Close();
}

void CGraphic_Device::CreateSwapChain(HWND windowHandle)
{
    // 스왑체인을 다시 생성하기 위해서 이전에 생성된 스왑 체인을 해제합니다.
    /*if (m_SwapChain) {
        m_SwapChain->Release();
        m_SwapChain = nullptr;
    }*/
    Safe_Release(m_SwapChain);


    DXGI_SWAP_CHAIN_DESC sd;
    sd.BufferDesc.Width = m_ClientWidth;
    sd.BufferDesc.Height = m_ClientHeight;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferDesc.Format = m_BackBufferFormat;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    sd.SampleDesc.Count = m4xMsaaState ? 4 : 1;
    sd.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = SwapChainBufferCount;
    sd.OutputWindow = m_hMainWnd = windowHandle;
    sd.Windowed = m_isWindowed;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    // 참고: 스왑 체인은 큐를 이용해서 플러시를 수행합니다.
    ThrowIfFailed(m_dxgiFactory->CreateSwapChain(
        m_CommandQueue,
        &sd,
        &m_SwapChain));
}

void CGraphic_Device::CreateRtvAndDsvDescriptorHeaps()
{
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
    rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtvHeapDesc.NodeMask = 0;
    ThrowIfFailed(m_d3dDevice->CreateDescriptorHeap(
        &rtvHeapDesc, IID_PPV_ARGS(&m_RtvHeap)));

    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dsvHeapDesc.NodeMask = 0;
    ThrowIfFailed(m_d3dDevice->CreateDescriptorHeap(
        &dsvHeapDesc, IID_PPV_ARGS(&m_DsvHeap)));
}

void CGraphic_Device::OnResize()
{
    assert(m_d3dDevice);
    assert(m_SwapChain);
    assert(m_DirectCmdListAlloc);

    // 어떤 리소스가 변경되기 전에 플러시합니다.
    FlushCommandQueue();

    ThrowIfFailed(m_CommandList->Reset(m_DirectCmdListAlloc, nullptr));

    // 다시 생성하기 전에 이전에 생성된 리소스들을 해제합니다.
    for (int i = 0; i < SwapChainBufferCount; ++i)        
        Safe_Release(m_SwapChainBuffer[i]);
    
    Safe_Release(m_DepthStencilBuffer);

    // 스왑체인의 크기를 변경합니다.
    ThrowIfFailed(m_SwapChain->ResizeBuffers(
        SwapChainBufferCount,
        m_ClientWidth, m_ClientHeight,
        m_BackBufferFormat,
        DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

    m_CurrBackBuffer = 0;

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart());
    for (UINT i = 0; i < SwapChainBufferCount; ++i)
    {
        ThrowIfFailed(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_SwapChainBuffer[i])));
        m_d3dDevice->CreateRenderTargetView(m_SwapChainBuffer[i], nullptr, rtvHeapHandle);
        rtvHeapHandle.Offset(1, m_RtvDescriptorSize);
    }

    // 뎁스/스텐실 버퍼와 뷰를 생성합니다.
    D3D12_RESOURCE_DESC depthStencilDesc;
    depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthStencilDesc.Alignment = 0;
    depthStencilDesc.Width = m_ClientWidth;
    depthStencilDesc.Height = m_ClientHeight;
    depthStencilDesc.DepthOrArraySize = 1;
    depthStencilDesc.MipLevels = 1;

    // 수정 2016년 11월 12일: SSAO는 뎁스 버퍼를 읽기 위해 SRV가 필요하다.
    // 그러므로 동일한 리소스에 두 개의 뷰를 생성해야 한다.
    //  1. SRV 포멧: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
    //  2. DSV 포멧: DXGI_FORMAT_D24_UNORM_S8_UINT
    // 그러므로 뎁스/스텐실을 생성할 때 타입 없는 포멧을 사용한다.
    depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

    depthStencilDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
    depthStencilDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
    depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE optClear;
    optClear.Format = m_DepthStencilFormat;
    optClear.DepthStencil.Depth = 1.0f;
    optClear.DepthStencil.Stencil = 0;
    ThrowIfFailed(m_d3dDevice->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &depthStencilDesc,
        D3D12_RESOURCE_STATE_COMMON,
        &optClear,
        IID_PPV_ARGS(&m_DepthStencilBuffer)));

    // 리소스의 포멧을 사용하여 밉맵 0에 해당하는 디스크립터를 생성한다.
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Format = m_DepthStencilFormat;
    dsvDesc.Texture2D.MipSlice = 0;
    m_d3dDevice->CreateDepthStencilView(m_DepthStencilBuffer, &dsvDesc, DepthStencilView());

    // 리소스를 초기 상태에서 뎁스 버퍼로 사용할 수 있는 상태로 변경한다.
    m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_DepthStencilBuffer,
        D3D12_RESOURCE_STATE_COMMON,
        D3D12_RESOURCE_STATE_DEPTH_WRITE));

    // 크기 변경을 위한 명령들을 실행시킨다.
    ThrowIfFailed(m_CommandList->Close());
    ID3D12CommandList* cmdLists[] = { m_CommandList};
    m_CommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

    // 크기 변경을 위한 명령들이 처리될때까지 기다린다.
    FlushCommandQueue();

    // 뷰포트를 업데이트한다.
    m_ScreenViewport.TopLeftX = 0;
    m_ScreenViewport.TopLeftY = 0;
    m_ScreenViewport.Width = static_cast<float>(m_ClientWidth);
    m_ScreenViewport.Height = static_cast<float>(m_ClientHeight);
    m_ScreenViewport.MinDepth = 0.0f;
    m_ScreenViewport.MaxDepth = 1.0f;

    m_ScissorRect = { 0, 0, m_ClientWidth, m_ClientHeight };
}

void CGraphic_Device::FlushCommandQueue()
{
    // 펜스 값을 증가시켜서 커맨드들이 새 펜스 포인트에 귀속되도록 합니다.
    m_CurrentFence++;

    // 커맨드들의 처리는 GPU에서 진행되기 때문에 언제 커맨드들이 처리됬는지를 CPU에서 알기 힘듭니다.
    // 그러므로 모든 커맨드가 처리됬을 때 새 펜스 지점을 설정하는 인스트럭션을 커맨드 큐에 추가합니다.
    // Signal()을 호출하기 전에 제출한 커맨드들이 처리되기 전까지 새 펜스 지점은 설정되지 않습니다.
    ThrowIfFailed(m_CommandQueue->Signal(m_Fence, m_CurrentFence));

    // GPU가 새 펜스 지점까지의 명령들을 완료할 때까지 기다립니다.
    if (m_Fence->GetCompletedValue() < m_CurrentFence)
    {
        HANDLE eventHandle = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);

        // GPU가 새 펜스 지점에 도달했으면 이벤트를 발동시킵니다.
        ThrowIfFailed(m_Fence->SetEventOnCompletion(m_CurrentFence, eventHandle));

        // GPU가 새 펜스를 설정하고 이벤트가 발동될때까지 기다립니다.
        WaitForSingleObject(eventHandle, INFINITE);
        CloseHandle(eventHandle);
    }
}

bool CGraphic_Device::Draw_1()
{
    //// 커맨드 기록을 위한 메모리를 재활용 합니다.
    //// 제출한 커맨드들이 GPU에서 모두 끝났을때 리셋할 수 있습니다.
    //ThrowIfFailed(m_DirectCmdListAlloc->Reset());

    //// ExecuteCommandList를 통해 커맨드 큐에 제출한 다음에 커맨드 리스트를 리셋할 수 있습니다.
    //ThrowIfFailed(m_CommandList->Reset(m_DirectCmdListAlloc, nullptr));

    // 리소스의 상태를 렌더링을 할 수 있도록 변경합니다.
    m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
        D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET));

    // 뷰포트와 시져를 설정한다. 커맨드 리스트가 리셋되고 나서 설정해야만 합니다.
    m_CommandList->RSSetViewports(1, &m_ScreenViewport);
    m_CommandList->RSSetScissorRects(1, &m_ScissorRect);

    // 백 버퍼와 뎁스 버퍼를 클리어 합니다.
    m_CommandList->ClearRenderTargetView(CurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
    m_CommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

    // 어디에 렌더링을 할지 설정합니다.
    m_CommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

    return true;
}
bool CGraphic_Device::Draw_2()
{

    // 리소스의 상태를 출력할 수 있도록 변경합니다.
    m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT));

    // 커맨드 기록을 종료합니다.
    ThrowIfFailed(m_CommandList->Close());

    // 커맨드 리스트의 실행을 위해 큐에 제출합니다.
    ID3D12CommandList* cmdLists[] = { m_CommandList };
    m_CommandQueue->ExecuteCommandLists(1, cmdLists);

    // 백 버퍼와 프론트 버퍼를 교체합니다.
    ThrowIfFailed(m_SwapChain->Present(0, 0));
    m_CurrBackBuffer = (m_CurrBackBuffer + 1) % SwapChainBufferCount;

    // 프레임의 커맨드들이 모두 처리될 때까지 기다립니다.
    // 이렇게 기다리는것은 비효율적이지만 예제를 간단하게 하기 위해서 기다립니다.
    // 나중에 기다리지 않고 렌더링을 하는법을 알아볼 것입니다.
    //FlushCommandQueue();
    return true;
}

ID3D12Resource* CGraphic_Device::CurrentBackBuffer() const
{
    return m_SwapChainBuffer[m_CurrBackBuffer];
}

D3D12_CPU_DESCRIPTOR_HANDLE CGraphic_Device::CurrentBackBufferView() const
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(
        m_RtvHeap->GetCPUDescriptorHandleForHeapStart(),
        m_CurrBackBuffer,
        m_RtvDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE CGraphic_Device::DepthStencilView() const
{
    return m_DsvHeap->GetCPUDescriptorHandleForHeapStart();
}

void CGraphic_Device::LogAdapters()
{
    UINT i = 0;
    IDXGIAdapter* adapter = nullptr;
    std::vector<IDXGIAdapter*> adapterList;
    while (m_dxgiFactory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_ADAPTER_DESC desc;
        adapter->GetDesc(&desc);

        std::wstring text = L"***Adapter: ";
        text += desc.Description;
        text += L"\n";

        OutputDebugString(text.c_str());

        adapterList.push_back(adapter);

        ++i;
    }

    for (size_t i = 0; i < adapterList.size(); ++i)
    {
        LogAdapterOutputs(adapterList[i]);
        ReleaseCom(adapterList[i]);
    }

}

void CGraphic_Device::LogAdapterOutputs(IDXGIAdapter* adapter)
{
    UINT i = 0;
    IDXGIOutput* output = nullptr;
    while (adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_OUTPUT_DESC desc;
        output->GetDesc(&desc);

        std::wstring text = L"***Output: ";
        text += desc.DeviceName;
        text += L"\n";

        OutputDebugString(text.c_str());

        LogOutputDisplayModes(output, m_BackBufferFormat);
        ReleaseCom(output);

        ++i;
    }
}

void CGraphic_Device::LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format)
{
    UINT count = 0;
    UINT flags = 0;

    // 디스플레이 모드의 개수를 얻기위해 nullptr과 함께 호출합니다.
    output->GetDisplayModeList(format, flags, &count, nullptr);

    std::vector<DXGI_MODE_DESC> modeList(count);
    output->GetDisplayModeList(format, flags, &count, &modeList[0]);

    for (auto& x : modeList)
    {
        UINT n = x.RefreshRate.Numerator;
        UINT d = x.RefreshRate.Denominator;
        std::wstring text =
            L"Width = " + std::to_wstring(x.Width) + L" " +
            L"Height = " + std::to_wstring(x.Height) + L" " +
            L"Refresh = " + std::to_wstring(n) + L"/" + std::to_wstring(d) +
            L"\n";

        OutputDebugString(text.c_str());
    }
}

CGraphic_Device* CGraphic_Device::Create(HWND winHandle)
{
    CGraphic_Device* pInstance = new CGraphic_Device();
    if (!pInstance->Initialize(winHandle))
    {
        MSG_BOX("Failed to Create : CGraphic_Device");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CGraphic_Device::Free()
{
    FlushCommandQueue();
    for (int i = 0; i < SwapChainBufferCount; ++i)
        Safe_Release(m_SwapChainBuffer[i]);

    Safe_Release(m_DepthStencilBuffer);

    Safe_Release(m_RtvHeap);
    Safe_Release(m_DsvHeap);

    Safe_Release(m_SwapChain);
    Safe_Release(m_CommandList);
    Safe_Release(m_DirectCmdListAlloc);
    Safe_Release(m_CommandQueue);
    Safe_Release(m_Fence);

    Safe_Release(m_d3dDevice);
    Safe_Release(m_dxgiFactory);
#if defined(DEBUG) | defined(_DEBUG)
    // D3D12 디버그 레이어를 활성화 합니다.
    {
        ID3D12Debug* debugController;
        //ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
            debugController->EnableDebugLayer();  // 디버그 레이어 활성화
            debugController->Release();
        }
        if (debugController != nullptr) debugController->Release();
    } // 메모리 누수 디버깅 체크용 코드
#endif
    // TODO : 메모리 해제하는 코드 추가해야함.
}
