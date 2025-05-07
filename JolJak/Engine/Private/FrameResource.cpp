#include "FrameResource.h"

CFrameResource::CFrameResource()
{
}

void CFrameResource::Initialize(ID3D12Device* device)
{
    ThrowIfFailed(device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS(&m_CmdListAlloc)));

    m_PassCB = CUploadBuffer<PassConstants>::Create(device, 1, true);
    m_ObjectCB = CUploadBuffer<ObjectConstants>::Create(device, 1000, true);
    //m_MaterialCB = CUploadBuffer<MaterialConstants>::Create(device, 1000, true);
}

CFrameResource* CFrameResource::Create(ID3D12Device* device)
{
    CFrameResource* pInstance = new CFrameResource();
    pInstance->Initialize(device);
    return pInstance;
}

void CFrameResource::Free()
{
    Safe_Release(m_CmdListAlloc);
    Safe_Release(m_PassCB);
    Safe_Release(m_ObjectCB);
    //Safe_Release(m_MaterialCB);
}