#pragma once
#include "Base.h"
#include "Texture.h"

BEGIN(Engine)

class ENGINE_DLL CTextureMgr : public CBase
{
	DECLARE_SINGLETON(CTextureMgr)
private:
	CTextureMgr() = default;
	virtual~CTextureMgr() = default;

public:
	void Add_Texture(string texname, CTexture* texInstance);
	ID3D12Resource* Get_Texture(string texname) { return m_TexMap[texname]->Get_Texture(); }

public:
	_uint Get_Tex_Index(string texname);

private:
	unordered_map<string, CTexture*> m_TexMap;

public:
	void Free();
};

END