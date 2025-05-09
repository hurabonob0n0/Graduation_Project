#include "MaterialMgr.h"

IMPLEMENT_SINGLETON(CMaterialMgr)

void CMaterialMgr::Add_Material(string matName, MaterialConstants* matInstance)
{
    if (m_MatMap.find(matName) != m_MatMap.end())
        return;

    m_MatMap[matName] = matInstance;
}

_uint CMaterialMgr::Get_Mat_Index(string matName)
{
    _uint index = 0;
    for (auto& iter = m_MatMap.begin(); iter != m_MatMap.end(); ++iter) {
        if (iter->first == matName)
            return index;
        ++index;
    }
    return index;
}

void CMaterialMgr::Free()
{      
}

