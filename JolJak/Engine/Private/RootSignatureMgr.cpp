#include "RootSignatureMgr.h"

IMPLEMENT_SINGLETON(CRootSignatureMgr)

void CRootSignatureMgr::Register(const std::string& name, ID3D12Device* device, CRootSignatureBuilder& builder)
{
    if (mRootSignatures.find(name) != mRootSignatures.end()) {
        throw std::runtime_error("RootSignature '" + name + "' already exists.");
    }

    auto rootSig = builder.Build(device);
    mRootSignatures[name] = rootSig;
}

ID3D12RootSignature* CRootSignatureMgr::Get(const std::string& name) const
{
    auto it = mRootSignatures.find(name);
    if (it == mRootSignatures.end()) {
        throw std::runtime_error("RootSignature '" + name + "' not found.");
    }
    return it->second;
}

void CRootSignatureMgr::Remove(const std::string& name)
{
    mRootSignatures.erase(name);
}

void CRootSignatureMgr::Free()
{
    for (auto& RootSig : mRootSignatures)
        Safe_Release(RootSig.second);

    mRootSignatures.clear();
}
