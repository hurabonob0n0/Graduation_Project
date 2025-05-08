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
    /*mat.DiffuseMapIndex = m_GameInstance->Add_Texture("M1A2_Fences_Base_color", CTexture::Create(L"../Bin/M1A2_Fences_Base_color.dds"));
    mat.NormalMapIndex = m_GameInstance->Add_Texture("M1A2_Fences_Normal", CTexture::Create(L"../Bin/M1A2_Fences_Normal.dds"));
    Set_MatIndex(m_GameInstance->Add_Material("M1A2_Fences_mat", mat));*/

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
