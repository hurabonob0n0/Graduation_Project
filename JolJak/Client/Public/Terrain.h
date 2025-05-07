#pragma once
#include "RenderObject.h"

BEGIN(Engine)
class CVIBuffer_Terrain;
END

BEGIN(Client)
class CTerrain : public CRenderObject
{
private:
	CTerrain();
	virtual~CTerrain() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(float fTimeDelta);
	virtual void LateTick(float fTimeDelta);
	virtual void Render();

private:
	CVIBuffer_Terrain* m_VIBufferCom;

public:
	static CTerrain* Create();
	virtual void Free() override;
};

END