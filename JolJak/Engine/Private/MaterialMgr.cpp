#include "MaterialMgr.h"

IMPLEMENT_SINGLETON(CMaterialMgr)

int CMaterialMgr::Add_Material(string matName, MaterialData matInstance)
{
    int index = 0;
    for (auto pair : m_MatMap)
    {
        if (pair.first == matName)
            return -index;
        ++index;
    }
    m_MatMap[matName] = matInstance;
    return index;
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

