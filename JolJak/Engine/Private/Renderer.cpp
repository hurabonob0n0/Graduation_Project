#include "Renderer.h"
#include "RenderObject.h"

CRenderer::CRenderer(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList) : CComponent_DC(pDevice,pCommandList)
{
}

void CRenderer::AddtoRenderObjects(RENDERGROUP RG,CRenderObject* pRenderObject)
{
    Safe_AddRef(pRenderObject);
    pRenderObject->Set_ObjCBIndex(m_vRenderObjects[RG].size());
    m_vRenderObjects[RG].push_back(pRenderObject);
}

void CRenderer::ResetRenderObjects()
{
    for (auto& pRenderObject : m_vRenderObjects[RG_PRIORITY]) {
        pRenderObject->Set_ObjCBIndex(-1);
        Safe_Release(pRenderObject);
    }
    m_vRenderObjects[RG_PRIORITY].clear();

    for (auto& pRenderObject : m_vRenderObjects[RG_NONLIGHT]) {
        pRenderObject->Set_ObjCBIndex(-1);
        Safe_Release(pRenderObject);
    }
    m_vRenderObjects[RG_NONLIGHT].clear();


    for (auto& pRenderObject : m_vRenderObjects[RG_NONBLEND]) {
        pRenderObject->Set_ObjCBIndex(-1);
        Safe_Release(pRenderObject);
    }
    m_vRenderObjects[RG_NONBLEND].clear();

    for (auto& pRenderObject : m_vRenderObjects[RG_BLEND]) {
        pRenderObject->Set_ObjCBIndex(-1);
        Safe_Release(pRenderObject);
    }
    m_vRenderObjects[RG_BLEND].clear();

    for (auto& pRenderObject : m_vRenderObjects[RG_UI]) {
        pRenderObject->Set_ObjCBIndex(-1);
        Safe_Release(pRenderObject);
    }
    m_vRenderObjects[RG_UI].clear();
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
    void Render_Priority();
	void Render_NonLight();
	void Render_NonBlend();
	void Render_Blend();
	void Render_UI();
}

void CRenderer::Render_Priority()
{
    for (auto& pGameObject : m_vRenderObjects[RG_PRIORITY])
    {
        pGameObject->Render();
    }
}

void CRenderer::Render_NonLight()
{
    for (auto& pGameObject : m_vRenderObjects[RG_NONLIGHT])
    {
        pGameObject->Render();
    }
}

void CRenderer::Render_NonBlend()
{
    for (auto& pGameObject : m_vRenderObjects[RG_NONBLEND])
    {
        pGameObject->Render();
    }
}

void CRenderer::Render_Blend()
{
    for (auto& pGameObject : m_vRenderObjects[RG_BLEND])
    {
        pGameObject->Render();
    }
}

void CRenderer::Render_UI()
{
    for (auto& pGameObject : m_vRenderObjects[RG_UI])
    {
        pGameObject->Render();
    }
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
