#include "Graphic_Device.h"
#include "VIBuffer_Geos.h"

CGraphic_Device::CGraphic_Device()
{
}

bool CGraphic_Device::Initialize(HWND windowHandle)
{
    ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&m_dxgiFactory)));            

    HRESULT hardwareResult = D3D12CreateDevice(                 // ���� �׷��� ī�尡 ��������� �����������. �׷���ī�尡 �ϳ���� �����Ѵ�.
        nullptr,
        D3D_FEATURE_LEVEL_11_0,
        IID_PPV_ARGS(&m_d3dDevice)
    );                                          

    /*if (FAILED(hardwareResult))                               // ������ ��� WRAP ����̽��� ����մϴ�.
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

    m_RtvDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);                            //�ڵ��� ũ�⸦ �̸� �����´�. GPU���� �ڵ��� ũ��� �ٸ� �� �ִ�.
    m_DsvDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    m_CbvSrvUavDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    // �� ���� ���信 ���ؼ� 4X MSAA�� �����Ǵ��� �˻��մϴ�.
    // ��� Direct3D 11 ����̽� ������ ��� ���� Ÿ�� ���信 4X MSAA�� �����մϴ�.
    // �׷��Ƿ� ���� ����Ƽ�� �˻��ϸ� �˴ϴ�.
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
    LogAdapters();                                                                  // ����� ����� ��, ����� ���� �����ϴ� �ڵ�
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

    // Ŀ�ǵ� ����Ʈ�� ���� ���·� �����մϴ�.
    // ������ �ϱ⿡ �ռ��� Ŀ�ǵ� ����Ʈ�� �����ϰ� Ŀ�ǵ带 ����ϴµ�,
    // ������ �ϱ� ���ؼ� Ŀ�ǵ� ����Ʈ�� �����ִ� ���¿��� �մϴ�.
    m_CommandList->Close();
}

void CGraphic_Device::CreateSwapChain(HWND windowHandle)
{
    // ����ü���� �ٽ� �����ϱ� ���ؼ� ������ ������ ���� ü���� �����մϴ�.
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

    // ����: ���� ü���� ť�� �̿��ؼ� �÷��ø� �����մϴ�.
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

    // � ���ҽ��� ����Ǳ� ���� �÷����մϴ�.
    FlushCommandQueue();

    ThrowIfFailed(m_CommandList->Reset(m_DirectCmdListAlloc, nullptr));

    // �ٽ� �����ϱ� ���� ������ ������ ���ҽ����� �����մϴ�.
    for (int i = 0; i < SwapChainBufferCount; ++i)        
        Safe_Release(m_SwapChainBuffer[i]);
    
    Safe_Release(m_DepthStencilBuffer);

    // ����ü���� ũ�⸦ �����մϴ�.
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

    // ����/���ٽ� ���ۿ� �並 �����մϴ�.
    D3D12_RESOURCE_DESC depthStencilDesc;
    depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthStencilDesc.Alignment = 0;
    depthStencilDesc.Width = m_ClientWidth;
    depthStencilDesc.Height = m_ClientHeight;
    depthStencilDesc.DepthOrArraySize = 1;
    depthStencilDesc.MipLevels = 1;

    // ���� 2016�� 11�� 12��: SSAO�� ���� ���۸� �б� ���� SRV�� �ʿ��ϴ�.
    // �׷��Ƿ� ������ ���ҽ��� �� ���� �並 �����ؾ� �Ѵ�.
    //  1. SRV ����: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
    //  2. DSV ����: DXGI_FORMAT_D24_UNORM_S8_UINT
    // �׷��Ƿ� ����/���ٽ��� ������ �� Ÿ�� ���� ������ ����Ѵ�.
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

    // ���ҽ��� ������ ����Ͽ� �Ӹ� 0�� �ش��ϴ� ��ũ���͸� �����Ѵ�.
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Format = m_DepthStencilFormat;
    dsvDesc.Texture2D.MipSlice = 0;
    m_d3dDevice->CreateDepthStencilView(m_DepthStencilBuffer, &dsvDesc, DepthStencilView());

    // ���ҽ��� �ʱ� ���¿��� ���� ���۷� ����� �� �ִ� ���·� �����Ѵ�.
    m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_DepthStencilBuffer,
        D3D12_RESOURCE_STATE_COMMON,
        D3D12_RESOURCE_STATE_DEPTH_WRITE));

    // ũ�� ������ ���� ��ɵ��� �����Ų��.
    ThrowIfFailed(m_CommandList->Close());
    ID3D12CommandList* cmdLists[] = { m_CommandList};
    m_CommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

    // ũ�� ������ ���� ��ɵ��� ó���ɶ����� ��ٸ���.
    FlushCommandQueue();

    // ����Ʈ�� ������Ʈ�Ѵ�.
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
    // �潺 ���� �������Ѽ� Ŀ�ǵ���� �� �潺 ����Ʈ�� �ͼӵǵ��� �մϴ�.
    m_CurrentFence++;

    // Ŀ�ǵ���� ó���� GPU���� ����Ǳ� ������ ���� Ŀ�ǵ���� ó��������� CPU���� �˱� ����ϴ�.
    // �׷��Ƿ� ��� Ŀ�ǵ尡 ó������ �� �� �潺 ������ �����ϴ� �ν�Ʈ������ Ŀ�ǵ� ť�� �߰��մϴ�.
    // Signal()�� ȣ���ϱ� ���� ������ Ŀ�ǵ���� ó���Ǳ� ������ �� �潺 ������ �������� �ʽ��ϴ�.
    ThrowIfFailed(m_CommandQueue->Signal(m_Fence, m_CurrentFence));

    // GPU�� �� �潺 ���������� ��ɵ��� �Ϸ��� ������ ��ٸ��ϴ�.
    if (m_Fence->GetCompletedValue() < m_CurrentFence)
    {
        HANDLE eventHandle = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);

        // GPU�� �� �潺 ������ ���������� �̺�Ʈ�� �ߵ���ŵ�ϴ�.
        ThrowIfFailed(m_Fence->SetEventOnCompletion(m_CurrentFence, eventHandle));

        // GPU�� �� �潺�� �����ϰ� �̺�Ʈ�� �ߵ��ɶ����� ��ٸ��ϴ�.
        WaitForSingleObject(eventHandle, INFINITE);
        CloseHandle(eventHandle);
    }
}

void CGraphic_Device::FlushCommandQueue(UINT64 finalValue)
{
    m_CurrentFence++;

    // Ŀ�ǵ���� ó���� GPU���� ����Ǳ� ������ ���� Ŀ�ǵ���� ó��������� CPU���� �˱� ����ϴ�.
    // �׷��Ƿ� ��� Ŀ�ǵ尡 ó������ �� �� �潺 ������ �����ϴ� �ν�Ʈ������ Ŀ�ǵ� ť�� �߰��մϴ�.
    // Signal()�� ȣ���ϱ� ���� ������ Ŀ�ǵ���� ó���Ǳ� ������ �� �潺 ������ �������� �ʽ��ϴ�.
    ThrowIfFailed(m_CommandQueue->Signal(m_Fence, m_CurrentFence));

    // GPU�� �� �潺 ���������� ��ɵ��� �Ϸ��� ������ ��ٸ��ϴ�.
    if (m_Fence->GetCompletedValue() < m_CurrentFence)
    {
        HANDLE eventHandle = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);

        // GPU�� �� �潺 ������ ���������� �̺�Ʈ�� �ߵ���ŵ�ϴ�.
        ThrowIfFailed(m_Fence->SetEventOnCompletion(finalValue, eventHandle));

        // GPU�� �� �潺�� �����ϰ� �̺�Ʈ�� �ߵ��ɶ����� ��ٸ��ϴ�.
        WaitForSingleObject(eventHandle, INFINITE);
        CloseHandle(eventHandle);
    }
}

void CGraphic_Device::Reset_CommandList()
{
    ThrowIfFailed(m_CommandList->Reset(m_DirectCmdListAlloc, nullptr));
}

void CGraphic_Device::Execute_Commands()
{
    ThrowIfFailed(m_CommandList->Close());
    ID3D12CommandList* cmdLists[] = { m_CommandList };
    m_CommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);
}

bool CGraphic_Device::Draw_1()
{
    //// Ŀ�ǵ� ����� ���� �޸𸮸� ��Ȱ�� �մϴ�.
    //// ������ Ŀ�ǵ���� GPU���� ��� �������� ������ �� �ֽ��ϴ�.
    //ThrowIfFailed(m_DirectCmdListAlloc->Reset());

    //// ExecuteCommandList�� ���� Ŀ�ǵ� ť�� ������ ������ Ŀ�ǵ� ����Ʈ�� ������ �� �ֽ��ϴ�.
    //ThrowIfFailed(m_CommandList->Reset(m_DirectCmdListAlloc, nullptr));

    // ���ҽ��� ���¸� �������� �� �� �ֵ��� �����մϴ�.
    m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
        D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET));

    // ����Ʈ�� ������ �����Ѵ�. Ŀ�ǵ� ����Ʈ�� ���µǰ� ���� �����ؾ߸� �մϴ�.
    m_CommandList->RSSetViewports(1, &m_ScreenViewport);
    m_CommandList->RSSetScissorRects(1, &m_ScissorRect);

    // �� ���ۿ� ���� ���۸� Ŭ���� �մϴ�.
    m_CommandList->ClearRenderTargetView(CurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
    m_CommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

    // ��� �������� ���� �����մϴ�.
    m_CommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

    return true;
}
bool CGraphic_Device::Draw_2()
{

    // ���ҽ��� ���¸� ����� �� �ֵ��� �����մϴ�.
    m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT));

    // Ŀ�ǵ� ����� �����մϴ�.
    ThrowIfFailed(m_CommandList->Close());

    // Ŀ�ǵ� ����Ʈ�� ������ ���� ť�� �����մϴ�.
    ID3D12CommandList* cmdLists[] = { m_CommandList };
    m_CommandQueue->ExecuteCommandLists(1, cmdLists);

    // �� ���ۿ� ����Ʈ ���۸� ��ü�մϴ�.
    ThrowIfFailed(m_SwapChain->Present(0, 0));
    m_CurrBackBuffer = (m_CurrBackBuffer + 1) % SwapChainBufferCount;

    // �������� Ŀ�ǵ���� ��� ó���� ������ ��ٸ��ϴ�.
    // �̷��� ��ٸ��°��� ��ȿ���������� ������ �����ϰ� �ϱ� ���ؼ� ��ٸ��ϴ�.
    // ���߿� ��ٸ��� �ʰ� �������� �ϴ¹��� �˾ƺ� ���Դϴ�.
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

    // ���÷��� ����� ������ ������� nullptr�� �Բ� ȣ���մϴ�.
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
    // D3D12 ����� ���̾ Ȱ��ȭ �մϴ�.
    {
        ID3D12Debug* debugController;
        //ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
            debugController->EnableDebugLayer();  // ����� ���̾� Ȱ��ȭ
            debugController->Release();
        }
        if (debugController != nullptr) debugController->Release();
    } // �޸� ���� ����� üũ�� �ڵ�
#endif
    // TODO : �޸� �����ϴ� �ڵ� �߰��ؾ���.
}
