#pragma once
#include "Base.h"
#include "RootSignatures.h"

BEGIN(Engine)



class ENGINE_DLL CRootSignatureMgr : public CBase
{
    DECLARE_SINGLETON(CRootSignatureMgr)
private:
    CRootSignatureMgr() = default;
    virtual ~CRootSignatureMgr() = default;

public:
    // ��Ʈ �ñ״�ó ��� (������ �޾Ƽ� Build�ؼ� ����
    void Register(const std::string& name, ID3D12Device* device, CRootSignatureBuilder& builder);

    // ��Ʈ �ñ״�ó ��������
    ID3D12RootSignature* Get(const std::string& name) const;

    // ��Ʈ �ñ״�ó ����
    void Remove(const std::string& name);

    // ��� ��Ʈ �ñ״�ó ����
public:
    void Free() override;

private:
    std::unordered_map<std::string, ID3D12RootSignature*> mRootSignatures;
};

END