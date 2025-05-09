#pragma once
#include "Base.h"

BEGIN(Engine)

enum class RootParam {
    ObjectCB,
    PassCB,
    MaterialBuffer,
    SkyCubeMap,
    Textures
};

class CRootSignatureBuilder : public CBase
{
public:
    //enum RootParam{RP_CBV,RP_SRV,RP_SRVTABLE,RP_END};

public:
    CRootSignatureBuilder& Push(RootParam param, D3D12_ROOT_PARAMETER_TYPE paramType,
        UINT shaderRegister, UINT registerSpace,
        D3D12_SHADER_VISIBILITY visibility,
        UINT num32BitValues = 0);

    CRootSignatureBuilder& PushTable(RootParam param, D3D12_DESCRIPTOR_RANGE_TYPE rangeType,
        UINT shaderRegister, UINT registerSpace, UINT numDescriptors,
        D3D12_SHADER_VISIBILITY visibility);

    ID3D12RootSignature* Build(ID3D12Device* device,
        D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    UINT GetRootParamIndex(RootParam param) const;

private:
    std::vector<D3D12_ROOT_PARAMETER> mParams;
    std::vector<D3D12_DESCRIPTOR_RANGE> mRanges;
    std::unordered_map<RootParam, UINT> mParamMap;

public:
    void Free();
};

END