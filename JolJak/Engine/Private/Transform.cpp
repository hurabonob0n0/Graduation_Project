#include "Transform.h"

CTransform::CTransform() : CComponent()
{
}

CTransform::CTransform(const CTransform& rhs) : CComponent(rhs)
{
}


HRESULT CTransform::Initialize_Prototype()
{
    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
    return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{
    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
    return S_OK;
}


CTransform* CTransform::Create()
{
    CTransform* pInstance = new CTransform();
    pInstance->Initialize_Prototype();
    return pInstance;
}

CComponent* CTransform::Clone(void* pArg)
{
    CComponent* pInstance = new CTransform(*this);
    pInstance->Initialize(pArg);
    return pInstance;
}

HRESULT CTransform::Go_Straight(float fTimeDelta)
{
    XMVECTOR	vPosition = Get_State(STATE_POSITION);
    XMVECTOR	vLook = Get_State(STATE_LOOK);

    vPosition += XMVector3Normalize(vLook) * fTimeDelta;

    Set_State(STATE_POSITION, vPosition);

    return S_OK;
}

HRESULT CTransform::Go_Backward(float fTimeDelta)
{
    XMVECTOR	vPosition = Get_State(STATE_POSITION);
    XMVECTOR	vLook = Get_State(STATE_LOOK);

    vPosition -= XMVector3Normalize(vLook) * fTimeDelta;

    Set_State(STATE_POSITION, vPosition);

    return S_OK;
}

HRESULT CTransform::Go_Left(float fTimeDelta)
{
    XMVECTOR	vPosition = Get_State(STATE_POSITION);
    XMVECTOR	vLook = Get_State(STATE_RIGHT);

    vPosition -= XMVector3Normalize(vLook) * fTimeDelta;

    Set_State(STATE_POSITION, vPosition);

    return S_OK;
}

HRESULT CTransform::Go_Right(float fTimeDelta)
{
    XMVECTOR	vPosition = Get_State(STATE_POSITION);
    XMVECTOR	vLook = Get_State(STATE_RIGHT);

    vPosition += XMVector3Normalize(vLook) * fTimeDelta;

    Set_State(STATE_POSITION, vPosition);

    return S_OK;
}

void CTransform::Turn(XMVECTOR vAxis, float fTimeDelta)
{
    XMFLOAT3 vRight;
    XMFLOAT3 vUp;
    XMVECTOR vLook = Get_State(STATE_LOOK);

    XMMATRIX		RotationMatrix = XMMatrixRotationAxis(vAxis, fTimeDelta);

    XMVECTOR vWorldUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

    vLook = XMVector3Normalize(XMVector3TransformNormal(vLook, RotationMatrix));

    XMStoreFloat3(&vRight, XMVector3Normalize(XMVector3Cross(vWorldUp, vLook)));

    XMStoreFloat3(&vUp, XMVector3Normalize(XMVector3Cross(vLook, XMLoadFloat3(&vRight))));

    Set_State(STATE_RIGHT, XMLoadFloat3(&vRight));
    Set_State(STATE_UP, XMLoadFloat3(&vUp));
    Set_State(STATE_LOOK, vLook);

    /*Set_State(STATE_RIGHT, XMVector4Transform(vRight, RotationMatrix));
    Set_State(STATE_UP, XMVector4Transform(vUp, RotationMatrix));
    Set_State(STATE_LOOK, XMVector4Transform(vLook, RotationMatrix));*/

    //XMVECTOR		vRight = Get_State(STATE_RIGHT);
    //XMVECTOR		vUp = Get_State(STATE_UP);
    //XMVECTOR		vLook = Get_State(STATE_LOOK);

    //XMMATRIX		RotationMatrix = XMMatrixRotationAxis(vAxis, 10.f * fTimeDelta);

    //// XMVector3TransformNormal();
    //Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
    //Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
    //Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Rotation(XMVECTOR vAxis, float fRadian)
{
}

void CTransform::Free()
{
}

