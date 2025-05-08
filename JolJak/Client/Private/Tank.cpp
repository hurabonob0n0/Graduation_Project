#include "pch.h"
#include "Tank.h"
#include "Model.h"
#include "Client_Defines.h"
#include "GameInstance.h"



CTank::CTank() : CRenderObject()
{
}

HRESULT CTank::Initialize()
{
    __super::Initialize();
    m_VIBufferCom = (CModel*)m_GameInstance->GetComponent("TankModel");

    return S_OK;
}

void CTank::Tick(float fTimeDelta)
{
    __super::Tick(fTimeDelta);
}

void CTank::LateTick(float fTimeDelta)
{
    __super::LateTick(fTimeDelta);
}

void CTank::Render()
{
    __super::Render();

    //m_VIBufferCom->Render();
}

CTank* CTank::Create()
{
    CTank* pInstance = new CTank();
    pInstance->Initialize();
    return pInstance;
}

void CTank::Free()
{
    __super::Free();
    Safe_Release(m_VIBufferCom);
}
