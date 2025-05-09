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
    // 루트 시그니처 등록 (빌더를 받아서 Build해서 저장
    void Register(const std::string& name, ID3D12Device* device, CRootSignatureBuilder& builder);

    // 루트 시그니처 가져오기
    ID3D12RootSignature* Get(const std::string& name) const;

    // 루트 시그니처 제거
    void Remove(const std::string& name);

    // 모든 루트 시그니처 해제
public:
    void Free() override;

private:
    std::unordered_map<std::string, ID3D12RootSignature*> mRootSignatures;
};

END