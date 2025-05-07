#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCamera : public CGameObject
{
public:
	CCamera();
	virtual ~CCamera() = default;

public:
	virtual HRESULT Initialize();
	void Tick(float fTimeDelta) override;
	void LateTick(float fTimeDelta) override;

public:
	static CCamera* Create();
	void Free() override;
};

END