#pragma once
#include "Base.h"

BEGIN(Engine)
class CVIBuffer_Geos;
END

BEGIN(Engine)
class ENGINE_DLL CGraphic_Device final : public CBase
{
private:
	CGraphic_Device();
	virtual ~CGraphic_Device() = default;

public:
    ID3D12Device* Get_Device() const { return m_d3dDevice; }
    ID3D12GraphicsCommandList* Get_CommandList() const { return m_CommandList; }
    ID3D12CommandQueue* Get_CommandQueue() const { return m_CommandQueue; }
    ID3D12CommandAllocator* Get_CommandAlloc() const { return m_DirectCmdListAlloc; }
    ID3D12Fence* Get_Fence() const { return m_Fence; }
    UINT64& Get_Fence_Value() { return m_CurrentFence; }

public:
	bool Initialize(HWND windowHandle);
    void CreateCommandObjects();
    void CreateSwapChain(HWND windowHandle);
    void CreateRtvAndDsvDescriptorHeaps();
    void OnResize();
    void FlushCommandQueue();

public:
    bool Draw_1();

public:
    bool Draw_2();

public:
    ID3D12Resource* CurrentBackBuffer() const;
    D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;
    D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;

public:
    void LogAdapters();
    void LogAdapterOutputs(IDXGIAdapter* adapter);
    void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);

private:
    bool m4xMsaaState = false;
    UINT m4xMsaaQuality = 0;

    IDXGIFactory4*              m_dxgiFactory = nullptr;
    IDXGISwapChain*             m_SwapChain = nullptr;
    ID3D12Device*               m_d3dDevice = nullptr;

    ID3D12Fence*                m_Fence = nullptr;
    UINT64                      m_CurrentFence = 0;

    ID3D12CommandQueue*         m_CommandQueue = nullptr;
    ID3D12CommandAllocator*     m_DirectCmdListAlloc = nullptr;
    ID3D12GraphicsCommandList*  m_CommandList = nullptr;

    static const int            SwapChainBufferCount = 2;
    int                         m_CurrBackBuffer = 0;
    ID3D12Resource*             m_SwapChainBuffer[SwapChainBufferCount]{nullptr};
    ID3D12Resource*             m_DepthStencilBuffer = nullptr;

    ID3D12DescriptorHeap*       m_RtvHeap = nullptr;
    ID3D12DescriptorHeap*       m_DsvHeap = nullptr;

    D3D12_VIEWPORT              m_ScreenViewport;
    D3D12_RECT                  m_ScissorRect;

    UINT                        m_RtvDescriptorSize = 0;
    UINT                        m_DsvDescriptorSize = 0;
    UINT                        m_CbvSrvUavDescriptorSize = 0;

    // 상속받은 클래스에서 반드시 아래 변수들의 값들을 커스터마이징 해야합니다.
    D3D_DRIVER_TYPE             m_d3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
    DXGI_FORMAT                 m_BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    DXGI_FORMAT                 m_DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    int                         m_ClientWidth = 2560;
    int                         m_ClientHeight = 1440;
    HWND                        m_hMainWnd;
    bool                        m_isWindowed = true;
    // TODO : 여기에 Resize할 크기들, 프레임, 윈도우 모드를 기록해놓고 바꿀 수 있도록 수정해야함. 추가


public:
    static CGraphic_Device* Create(HWND winHandle);
	virtual void Free() override;
};

END