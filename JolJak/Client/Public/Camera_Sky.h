#pragma once
#include "Camera.h"

BEGIN(Engine)
class VIBuffer_Geos;
END

class CCamera_Sky : public CCamera
{
private:
	CCamera_Sky();
	virtual ~CCamera_Sky() = default;

public:
	virtual HRESULT Initialize();
	void Tick(float fTimeDelta) override;
	void LateTick(float fTimeDelta) override;
	void Render();

private:
	VIBuffer_Geos* m_VIBufferCom;

public:
	static CCamera_Sky* Create();
	void Free() override;
};

