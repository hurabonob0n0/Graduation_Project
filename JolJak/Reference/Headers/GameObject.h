#pragma once
#include "Base.h"
#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject : public CBase
{
public:
	CGameObject();
	virtual ~CGameObject() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(float fTimeDelta) = 0;
	virtual void LateTick(float fTimeDelta) = 0;

	void Set_Position(float* position) {
		_vector vec = XMVectorSet(position[0], position[1], position[2], 1.f);
		m_TransformCom->Set_State(CTransform::STATE_POSITION, vec);
	}

protected:
	CTransform* m_TransformCom = {};
	class CGameInstance* m_GameInstance = {};

protected:
	unordered_map<wstring, class CComponent*> m_Components;
	//CComponent* Get_Object_Component(wstring ComName) { return m_Components; };

public:
	void Free() override;
};

END