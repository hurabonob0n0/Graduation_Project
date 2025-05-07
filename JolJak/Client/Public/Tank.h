#pragma once
#include "RenderObject.h"

BEGIN(Engine)
class CModel;
END


BEGIN(Client)

class CTank : public CRenderObject
{
private:
	CTank();
	virtual~CTank() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(float fTimeDelta);
	virtual void LateTick(float fTimeDelta);
	virtual void Render();
	//dslfkjdslkfjsdl

private:
	CModel* m_VIBufferCom;

public:
	static CTank* Create();
	virtual void Free() override;
};

END