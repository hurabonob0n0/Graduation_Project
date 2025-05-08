#pragma once
#include "Base.h"
#include "RootSignature.h"

BEGIN(Engine)



class ENGINE_DLL CRootSignatureMgr : public CBase
{
    DECLARE_SINGLETON(CRootSignatureMgr)
private:
    CRootSignatureMgr() = default;
    virtual ~CRootSignatureMgr() = default;

public:
    void Register(const std::string& name,CRootSignature* RS);

    // ��Ʈ �ñ״�ó ��������
    ID3D12RootSignature* Get(const std::string& name) const;

    // ��Ʈ �ñ״�ó ����
    void Remove(const std::string& name);

public:
    void Free() override;

private:
    std::unordered_map<std::string, CRootSignature*> mRootSignatures;
};

END