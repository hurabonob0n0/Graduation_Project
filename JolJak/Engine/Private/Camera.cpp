#include "Camera.h"
#include "GameInstance.h"

CCamera::CCamera() : CGameObject()
{
}

HRESULT CCamera::Initialize()
{
	__super::Initialize();

	m_TransformCom->Set_State(CTransform::STATE_POSITION, { 0.f,0.f,-10.f,1.f });

	return S_OK;
}

void CCamera::Tick(float fTimeDelta)
{
	if (m_GameInstance->Key_Pressing('W'))
		m_TransformCom->Go_Straight(fTimeDelta * 100.f);

	if (m_GameInstance->Key_Pressing('S'))
		m_TransformCom->Go_Backward(fTimeDelta * 100.f);

	if (m_GameInstance->Key_Pressing('A'))
		m_TransformCom->Go_Left(fTimeDelta * 100.f);

	if (m_GameInstance->Key_Pressing('D'))
		m_TransformCom->Go_Right(fTimeDelta * 100.f);

	m_TransformCom->Turn({0.f,1.f,0.f,0.f}, fTimeDelta * (float)m_GameInstance->Get_Mouse_XDelta() * 0.5f);

	m_TransformCom->Turn(m_TransformCom->Get_State(CTransform::STATE_RIGHT), fTimeDelta * (float)m_GameInstance->Get_Mouse_YDelta() * 0.5f);

}

void CCamera::LateTick(float fTimeDelta)
{
	XMMATRIX proj = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(60.0f), // Field of View (radian ´ÜÀ§)
		2560.f/1440.f,               // Aspect ratio = width / height
		1.f,                     // Near clipping plane
		1000.f                       // Far clipping plane
	);

	PassConstants pc{};

	XMMATRIX view = m_TransformCom->Get_WorldMatrix_Inverse();//XMLoadFloat4x4(&mView);
	//XMMATRIX view = m_TransformCom->Get_WorldMatrix();

	XMMATRIX viewProj = XMMatrixMultiply(view, proj);
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
	XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
	XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

	XMStoreFloat4x4(&pc.View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&pc.InvView, XMMatrixTranspose(invView));
	XMStoreFloat4x4(&pc.Proj, XMMatrixTranspose(proj));
	pc.AmbientLight = { 0.25f, 0.25f, 0.35f, 1.0f };
	pc.Lights[0].Direction = { 0.57735f, -0.57735f, 0.57735f };
	pc.Lights[0].Strength = { 0.6f, 0.6f, 0.6f };
	pc.Lights[1].Direction = { -0.57735f, -0.57735f, 0.57735f };
	pc.Lights[1].Strength = { 0.3f, 0.3f, 0.3f };
	pc.Lights[2].Direction = { 0.0f, -0.707f, -0.707f };
	pc.Lights[2].Strength = { 0.15f, 0.15f, 0.15f };

	auto currPassCB = m_GameInstance->GetCurrentFrameResource()->m_PassCB;
	currPassCB->CopyData(0, pc);
}

CCamera* CCamera::Create()
{
	CCamera* pCamera = new CCamera;
	pCamera->Initialize();
	return pCamera;
}

void CCamera::Free()
{
}
