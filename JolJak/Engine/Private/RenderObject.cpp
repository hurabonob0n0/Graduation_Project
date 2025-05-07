#include "RenderObject.h"
#include "GameInstance.h"
#include "VIBuffer_Geos.h"


CRenderObject::CRenderObject() : CGameObject()
{
}

void CRenderObject::Set_ObjCBIndex(const UINT& index)
{
    m_objCBIndex = index;
}

HRESULT CRenderObject::Initialize()
{
    __super::Initialize(); 
    m_RendererCom = (CRenderer*)m_GameInstance->GetComponent("RendererCom", nullptr);

	m_TexCoordTransformCom = (CTransform*)m_GameInstance->GetComponent("TransformCom", nullptr);

    return S_OK;
}

void CRenderObject::Tick(float fTimeDelta)
{
}

void CRenderObject::LateTick(float fTimeDelta)
{
	m_RendererCom->AddtoRenderObjects(this);

	auto FrameResource = m_GameInstance->GetCurrentFrameResource();
	auto currObjectCB = FrameResource->m_ObjectCB;

	XMMATRIX world = m_TransformCom->Get_WorldMatrix();

	ObjectConstants objConstants;
	XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));

	currObjectCB->CopyData(m_objCBIndex, objConstants);
}

void CRenderObject::Render()
{
	UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));

	auto objectCB = m_GameInstance->GetCurrentFrameResource()->m_ObjectCB->Resource();

	D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress();

	objCBAddress += m_objCBIndex * objCBByteSize;

	m_GameInstance->Get_CommandList()->SetGraphicsRootConstantBufferView(0, objCBAddress);

    //m_VIBufferCom->Render();
}

void CRenderObject::Free()
{
	//Safe_Release(m_VIBufferCom);
    Safe_Release(m_RendererCom);
	Safe_Release(m_TexCoordTransformCom);
}
