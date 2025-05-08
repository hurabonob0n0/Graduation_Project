#include "TextureMgr.h"

IMPLEMENT_SINGLETON(CTextureMgr);

int CTextureMgr::Add_Texture(string texname, CTexture* texInstance)
{
    int index = 0;
    for (auto pair : m_TexMap)
    {
        if (pair.first == texname)
            return -index;
        ++index;
    }

    Safe_AddRef(texInstance);

    m_TexMap[texname] = texInstance;
    return index;
}

void CTextureMgr::Free()
{
    for (auto& tex : m_TexMap)
        Safe_Release(tex.second);
}
