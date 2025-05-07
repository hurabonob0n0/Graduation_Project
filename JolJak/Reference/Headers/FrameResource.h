#pragma once
#include "Base.h"

BEGIN(Engine)

class CFrameResource : public CBase
{
public:
    CFrameResource();
    CFrameResource(const CFrameResource& rhs) = delete;
    CFrameResource& operator=(const CFrameResource& rhs) = delete;
    virtual~CFrameResource() = default;

public:
    void Initialize(ID3D12Device* device);

    // We cannot reset the allocator until the GPU is done processing the commands.
    // So each frame needs their own allocator.
    ID3D12CommandAllocator* m_CmdListAlloc;

    // We cannot update a cbuffer until the GPU is done processing the commands
    // that reference it.  So each frame needs their own cbuffers.
    CUploadBuffer<PassConstants>* m_PassCB = nullptr;
    CUploadBuffer<ObjectConstants>* m_ObjectCB = nullptr;
   // CUploadBuffer<MaterialConstants>* m_MaterialCB = nullptr;

    // Fence value to mark commands up to this fence point.  This lets us
    // check if these frame resources are still in use by the GPU.
    UINT64 m_Fence = 0;

public:
    static CFrameResource* Create(ID3D12Device* device);
    void Free();
};

END