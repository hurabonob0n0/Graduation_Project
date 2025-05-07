#include "ObjectMgr.h"

IMPLEMENT_SINGLETON(CObjectMgr)

CObjectMgr::CObjectMgr()
{
}

HRESULT CObjectMgr::AddObject(const string& layerTag, CGameObject* pGameObject)
{
    if (pGameObject == nullptr)
        return E_FAIL;

    m_Layers[layerTag].push_back(pGameObject);
    return S_OK;
}

HRESULT CObjectMgr::RemoveObject(const string& layerTag, CGameObject* pGameObject)
{
    auto iter = m_Layers.find(layerTag);
    if (iter == m_Layers.end())
        return E_FAIL;

    Layer& layer = iter->second;
    auto objIter = std::find(layer.begin(), layer.end(), pGameObject);

    if (objIter != layer.end())
    {
        Safe_Release(*objIter);  // 참조 카운트 감소 (혹은 delete)
        layer.erase(objIter);
        return S_OK;
    }

    return E_FAIL;
}

void CObjectMgr::Update(const float& fTimeDelta)
{
    for (auto& pair : m_Layers)
    {
        for (auto& obj : pair.second)
        {
            if (obj)
                obj->Tick(fTimeDelta);
        }
    }
}

void CObjectMgr::LateUpdate(const float& fTimeDelta)
{
    for (auto& pair : m_Layers)
    {
        for (auto& obj : pair.second)
        {
            if (obj)
                obj->LateTick(fTimeDelta);
        }
    }
}

void CObjectMgr::Free()
{
    for (auto& pair : m_Layers)
    {
        for (auto& obj : pair.second)
        {
            Safe_Release(obj);  // or delete obj;
        }
        pair.second.clear();
    }
    m_Layers.clear();
}