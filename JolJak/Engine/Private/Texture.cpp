#include "Texture.h"

CTexture::CTexture(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList) : CComponent_DC(pDevice,pCommandList)
{
}

CTexture::CTexture(CTexture& rhs) : CComponent_DC(rhs)
{
    m_Resource = rhs.m_Resource;
    Safe_AddRef(m_Resource);
}

void CTexture::Initialize_Prototype(const wchar_t* Filename)
{
    CreateDDSTextureFromFile12(m_Device, m_CommandList, Filename, m_Resource, m_UploadHeap);
}

void CTexture::Initialize()
{
}

CComponent* CTexture::Clone(void* pArg)
{
    CTexture* pInstance = new CTexture(*this);
    pInstance->Initialize();
    return pInstance;
}

CTexture* CTexture::Create(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, const wchar_t* Filename)
{
    CTexture* pInstance = new CTexture(pDevice, pCommandList);
    pInstance->Initialize_Prototype(Filename);
    return pInstance;
}
