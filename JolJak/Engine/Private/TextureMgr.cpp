#include "TextureMgr.h"

IMPLEMENT_SINGLETON(CTextureMgr);

void CTextureMgr::Add_Texture(string texname, CTexture* texInstance)
{
    if (m_TexMap.find(texname) != m_TexMap.end())
        return;

    Safe_AddRef(texInstance);

    m_TexMap[texname] = texInstance;
}

_uint CTextureMgr::Get_Tex_Index(string texname)
{
    _uint index = 0;
    for (auto& iter = m_TexMap.begin();iter != m_TexMap.end(); ++iter) {
        if (iter->first == texname)
            return index;
        ++index;
    }
    return index;
}

void CTextureMgr::Free()
{
    for (auto& tex : m_TexMap)
        Safe_Release(tex.second);
}
