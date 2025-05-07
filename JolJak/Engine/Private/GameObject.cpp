#include "GameInstance.h"
#include "GameObject.h"
#include "Transform.h"

CGameObject::CGameObject()
{
}

HRESULT CGameObject::Initialize()
{
	m_GameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(m_GameInstance);
	m_TransformCom = (CTransform*)m_GameInstance->GetComponent("TransformCom", nullptr);
	return S_OK;
}

void CGameObject::Free()
{
	Safe_Release(m_GameInstance);
	Safe_Release(m_TransformCom);
}
