#pragma once
#include "Base.h"
#include "FrameResource.h"

BEGIN(Engine)

class CFrameResourceMgr : public CBase
{
    DECLARE_SINGLETON(CFrameResourceMgr)

private:
    CFrameResourceMgr() = default;
    virtual ~CFrameResourceMgr() = default;

public:
    HRESULT Initialize(ID3D12Device* device, UINT numFrameResources)
    {
        if (m_FrameResources.size() > 0)
            return E_FAIL; // 이미 초기화됨

        m_FrameResources.resize(numFrameResources);
        for (UINT i = 0; i < numFrameResources; ++i)
        {
            m_FrameResources[i] = CFrameResource::Create(device);
            if (m_FrameResources[i] == nullptr)
                return E_FAIL;
        }

        m_CurrentFrameIndex = 0;
        m_NumFrames = numFrameResources;
        return S_OK;
    }

    // 현재 FrameResource 반환
    CFrameResource* GetCurrentFrameResource() const
    {
        if (m_FrameResources.empty())
            return nullptr;
        return m_FrameResources[m_CurrentFrameIndex];
    }

    UINT            GetCurrentFrameIndex() const { return m_CurrentFrameIndex; }

    // 다음 프레임으로 넘어가기
    void AdvanceFrame()
    {
        m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % m_NumFrames;
    }   

    UINT64 Get_Final_Fence() {
        auto iter = *max_element(
            m_FrameResources.begin(),
            m_FrameResources.end(),
            [](CFrameResource* a, CFrameResource* b) {
                return a->m_Fence < b->m_Fence; // 주의: 내림차순이면 큰 값을 얻으려면 '<'
            });
        return iter->m_Fence;
    }

private:
    std::vector<CFrameResource*> m_FrameResources;
    UINT m_CurrentFrameIndex = 0;
    UINT m_NumFrames = 0;

public:
    void Free() override
    {
        for (auto& frameResource : m_FrameResources)
        {
            if (frameResource)
            {
                frameResource->Free();
                Safe_Release(frameResource); // or delete frameResource;
            }
        }
        m_FrameResources.clear();
    }
};

END
