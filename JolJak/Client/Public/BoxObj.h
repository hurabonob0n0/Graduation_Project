#pragma once
#include "Client_Defines.h"
#include "RenderObject.h"

BEGIN(Engine)
class CVIBuffer_Geos;
END

BEGIN(Client)

class CBoxObj : public CRenderObject
{
private:
	CBoxObj();
	virtual ~CBoxObj() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(float fTimeDelta);
	virtual void LateTick(float fTimeDelta);
	virtual void Render();

private:
	CVIBuffer_Geos* m_VIBufferCom;

public:
	static CBoxObj* Create();
	virtual void Free() override;
};

END