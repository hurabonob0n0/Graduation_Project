#include "Camera_Sky.h"
#include "GameInstance.h"

CCamera_Sky::CCamera_Sky() : CCamera()
{
}

HRESULT CCamera_Sky::Initialize()
{
    //m_VIBufferCom = CVIBuffer_Geos::Create(GETDEVICE,GETCOMMAND)
    //return E_NOTIMPL;
}

void CCamera_Sky::Tick(float fTimeDelta)
{
}

void CCamera_Sky::LateTick(float fTimeDelta)
{
}

void CCamera_Sky::Render()
{
}

CCamera_Sky* CCamera_Sky::Create()
{
    return nullptr;
}

void CCamera_Sky::Free()
{
}
