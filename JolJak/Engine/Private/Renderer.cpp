#include "Renderer.h"
#include "RenderObject.h"

CRenderer::CRenderer(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList) : CComponent_DC(pDevice,pCommandList)
{
}

void CRenderer::AddtoRenderObjects(CRenderObject* pRenderObject)
{
    Safe_AddRef(pRenderObject);
    pRenderObject->Set_ObjCBIndex(m_vRenderObjects.size());
    m_vRenderObjects.push_back(pRenderObject);
}

void CRenderer::ResetRenderObjects()
{
    for (auto& pRenderObject : m_vRenderObjects) {
        pRenderObject->Set_ObjCBIndex(-1);
        Safe_Release(pRenderObject);
    }
    m_vRenderObjects.clear();
}

HRESULT CRenderer::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CRenderer::Initialize(void* pArg)
{
    return S_OK;
}

void CRenderer::Render()
{
    for (auto& RO : m_vRenderObjects)
        RO->Render();
}

CRenderer* CRenderer::Create(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList)
{
    CRenderer* pInstance = new CRenderer(pDevice, pCommandList);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to create Renderer prototype.");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CRenderer::Clone(void* pArg)
{
    AddRef();
    return this;
}

void CRenderer::Free()
{
    ResetRenderObjects();
    __super::Free();
}
