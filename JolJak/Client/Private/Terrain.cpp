#include "pch.h"
#include "Terrain.h"
#include "Client_Defines.h"
#include "GameInstance.h"

CTerrain::CTerrain() : CRenderObject()
{
}

HRESULT CTerrain::Initialize()
{
    __super::Initialize();
    m_VIBufferCom = (CVIBuffer_Terrain*)m_GameInstance->GetComponent("TerrainCom");

    //m_TransformCom->Set_State(CTransform::STATE_POSITION, { 0.f,0.f,30.f,1.f });

    return S_OK;
}

void CTerrain::Tick(float fTimeDelta)
{
    __super::Tick(fTimeDelta);
}

void CTerrain::LateTick(float fTimeDelta)
{
    __super::LateTick(fTimeDelta);
}

void CTerrain::Render()
{
    __super::Render();

    m_VIBufferCom->Render();
}

CTerrain* CTerrain::Create()
{
    CTerrain* pInstance = new CTerrain;
    pInstance->Initialize();
    return pInstance;
}

void CTerrain::Free()
{
    __super::Free();
}
