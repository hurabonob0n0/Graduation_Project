#pragma once
#include "Base.h"

BEGIN(Engine)

class CMaterialMgr : public CBase
{
	DECLARE_SINGLETON(CMaterialMgr)
private:
	CMaterialMgr() = default;
	virtual~CMaterialMgr() = default;

public:
	int Add_Material(string matName, MaterialData matInstance);

public:
	_uint Get_Mat_Index(string matName);

private:
	unordered_map<string, MaterialData> m_MatMap;

public:
	void Free();
};

END