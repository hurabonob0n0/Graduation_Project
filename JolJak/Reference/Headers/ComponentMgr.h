#pragma once
#include "Base.h"
#include "Component.h"
#include "VIBuffer_Geos.h"
#include "VIBuffer_Terrain.h"
#include "Transform.h"
#include "Renderer.h"
#include "Model.h"
#include "Mesh.h"
#include "Bone.h"

class CComponentMgr : public CBase
{
    DECLARE_SINGLETON(CComponentMgr)

private:
    CComponentMgr() = default;
    virtual ~CComponentMgr() = default;

public:
    // ���� ������Ʈ ���
    HRESULT AddPrototype(const string& prototypeTag, CComponent* pPrototype)
    {
        if (m_Prototypes.find(prototypeTag) != m_Prototypes.end())
            return E_FAIL; // �̹� ��ϵ�

        m_Prototypes[prototypeTag] = pPrototype;
        return S_OK;
    }

    // ���� ������Ʈ�κ��� Clone ��ȯ
    CComponent* GetComponent(const string& prototypeTag, void* pArg = nullptr) const
    {
        auto iter = m_Prototypes.find(prototypeTag);
        if (iter != m_Prototypes.end())
            return iter->second->Clone(pArg); // ���纻 ��ȯ

        return nullptr;
    }

    CComponent* GetPrototype(const string& prototypeTag) const
    {
        auto iter = m_Prototypes.find(prototypeTag);
        if (iter != m_Prototypes.end())
            return iter->second; // 

        return nullptr;
    }

    // ��ϵ� ��� ���� ����
    void Free() override
    {
        for (auto& pair : m_Prototypes)
        {
            Safe_Release(pair.second); // or delete pair.second;
        }
        m_Prototypes.clear();
    }

private:
    unordered_map<string, CComponent*> m_Prototypes;
};

