#include "pch.h"
#include "BoxObj.h"
#include "GameInstance.h"
#include "VIBuffer_Terrain.h"
#include "ClientPacketHandler.h"
#include "ServiceManager.h"

CBoxObj::CBoxObj() : CRenderObject()
{
}

HRESULT CBoxObj::Initialize()
{
    __super::Initialize();
    m_VIBufferCom = (CVIBuffer_Geos*)m_GameInstance->GetComponent("BaseGeosCom");

    m_TransformCom->Set_State(CTransform::STATE_POSITION, { 0.f,0.f,30.f,1.f });

    return S_OK;
}

void CBoxObj::Tick(float fTimeDelta)
{
    _float4 Position;


    if (_myPlayer) {


        if (m_GameInstance->Key_Pressing(VK_UP)) {
            m_TransformCom->Go_Straight(fTimeDelta * 5.f);
            XMStoreFloat4(&Position, m_TransformCom->Get_State(CTransform::STATE_POSITION));
            SendBufferRef sendBuffer = ClientPacketHandler::Make_C_MOVE(Position.x, Position.y, Position.z);
            ServiceManager::GetInstace().GetService()->Broadcast(sendBuffer);
        }




        if (m_GameInstance->Key_Pressing(VK_DOWN)) {
            m_TransformCom->Go_Backward(fTimeDelta * 5.f);
            XMStoreFloat4(&Position, m_TransformCom->Get_State(CTransform::STATE_POSITION));
            SendBufferRef sendBuffer = ClientPacketHandler::Make_C_MOVE(Position.x, Position.y, Position.z);
            ServiceManager::GetInstace().GetService()->Broadcast(sendBuffer);
        }


        if (m_GameInstance->Key_Pressing(VK_RIGHT)) {
            m_TransformCom->Go_Right(fTimeDelta * 5.f);
            XMStoreFloat4(&Position, m_TransformCom->Get_State(CTransform::STATE_POSITION));
            SendBufferRef sendBuffer = ClientPacketHandler::Make_C_MOVE(Position.x, Position.y, Position.z);
            ServiceManager::GetInstace().GetService()->Broadcast(sendBuffer);

        }
        if (m_GameInstance->Key_Pressing(VK_LEFT)) {
            m_TransformCom->Go_Left(fTimeDelta * 5.f);
            XMStoreFloat4(&Position, m_TransformCom->Get_State(CTransform::STATE_POSITION));
            SendBufferRef sendBuffer = ClientPacketHandler::Make_C_MOVE(Position.x, Position.y, Position.z);
            ServiceManager::GetInstace().GetService()->Broadcast(sendBuffer);
        }

    }

   // _float4 Position;

   // XMStoreFloat4(&Position, m_TransformCom->Get_State(CTransform::STATE_POSITION));
   // 
   // float TerrainY = ((CVIBuffer_Terrain*)(m_GameInstance->GetPrototype("TerrainCom")))->Get_Terrain_Heights(Position.x,Position.z);

  

   //m_TransformCom->Set_State(CTransform::STATE_POSITION, _vector{ Position.x,TerrainY,Position.z,1.f });
  
#pragma region MakeSendBuffer & Send Data

   //SendBufferRef sendBuffer = ClientPacketHandler::Make_C_MOVE(Position.x, TerrainY, Position.z);
   //ServiceManager::GetInstace().GetService()->Broadcast(sendBuffer);

#pragma endregion TODO : 여기 좌표 수정 필요


   __super::Tick(fTimeDelta);
}

void CBoxObj::LateTick(float fTimeDelta)
{
    __super::LateTick(fTimeDelta);
}

void CBoxObj::Render()
{
    __super::Render();

    m_VIBufferCom->Render();
}

CBoxObj* CBoxObj::Create()
{
    CBoxObj* pInstance = new CBoxObj;
    pInstance->Initialize();
    return pInstance;
}

void CBoxObj::Free()
{
    __super::Free();
}
