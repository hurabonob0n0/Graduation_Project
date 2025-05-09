#include "RootSignatures.h"

CRootSignatureBuilder& CRootSignatureBuilder::Push(RootParam param, D3D12_ROOT_PARAMETER_TYPE paramType,
    UINT shaderRegister, UINT registerSpace,
    D3D12_SHADER_VISIBILITY visibility,
    UINT num32BitValues) {
    D3D12_ROOT_PARAMETER rp{};
    rp.ParameterType = paramType;
    rp.ShaderVisibility = visibility;

    if (paramType == D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS) {
        rp.Constants.Num32BitValues = num32BitValues;
        rp.Constants.ShaderRegister = shaderRegister;
        rp.Constants.RegisterSpace = registerSpace;
    }
    else {
        rp.Descriptor.ShaderRegister = shaderRegister;
        rp.Descriptor.RegisterSpace = registerSpace;
    }

    mParams.push_back(rp);
    mParamMap[param] = static_cast<UINT>(mParams.size() - 1);
    return *this;
}

CRootSignatureBuilder& CRootSignatureBuilder::PushTable(RootParam param, D3D12_DESCRIPTOR_RANGE_TYPE rangeType,
    UINT shaderRegister, UINT registerSpace, UINT numDescriptors,
    D3D12_SHADER_VISIBILITY visibility) {
    D3D12_DESCRIPTOR_RANGE range{};
    range.RangeType = rangeType;
    range.NumDescriptors = numDescriptors;
    range.BaseShaderRegister = shaderRegister;
    range.RegisterSpace = registerSpace;
    range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    mRanges.push_back(range);

    D3D12_ROOT_PARAMETER rp{};
    rp.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rp.ShaderVisibility = visibility;
    rp.DescriptorTable.NumDescriptorRanges = 1;
    rp.DescriptorTable.pDescriptorRanges = &mRanges.back();

    mParams.push_back(rp);
    mParamMap[param] = static_cast<UINT>(mParams.size() - 1);
    return *this;
}

ID3D12RootSignature* CRootSignatureBuilder::Build(ID3D12Device* device,
    D3D12_ROOT_SIGNATURE_FLAGS flags) {
    D3D12_ROOT_SIGNATURE_DESC desc{};
    desc.NumParameters = static_cast<UINT>(mParams.size());
    desc.pParameters = mParams.data();
    desc.NumStaticSamplers = 0;
    desc.pStaticSamplers = nullptr;
    desc.Flags = flags;

    ID3DBlob* sigBlob;
    ID3DBlob* errorBlob;
    HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &sigBlob, &errorBlob);
    if (FAILED(hr)) {
        if (errorBlob) {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
        }
        throw std::runtime_error("Failed to serialize root signature.");
    }

    ID3D12RootSignature* rootSig;
    hr = device->CreateRootSignature(0, sigBlob->GetBufferPointer(), sigBlob->GetBufferSize(), IID_PPV_ARGS(&rootSig));
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create root signature.");
    }
    return rootSig;
}

UINT CRootSignatureBuilder::GetRootParamIndex(RootParam param) const {
    return mParamMap.at(param);
}

void CRootSignatureBuilder::Free()
{
}
