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
    // 1. 새로운 Range 리스트를 하나 만든다.
    mTableRanges.emplace_back(); // 새 vector 하나 추가
    auto& rangeVec = mTableRanges.back();
    rangeVec.resize(1);          // range 1개만 사용

    // 2. range 채우기
    rangeVec[0].RangeType = rangeType;
    rangeVec[0].NumDescriptors = numDescriptors;
    rangeVec[0].BaseShaderRegister = shaderRegister;
    rangeVec[0].RegisterSpace = registerSpace;
    rangeVec[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    // 3. root parameter 설정
    D3D12_ROOT_PARAMETER rp{};
    rp.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rp.ShaderVisibility = visibility;
    rp.DescriptorTable.NumDescriptorRanges = 1;
    rp.DescriptorTable.pDescriptorRanges = rangeVec.data(); // 🔥 포인터가 항상 유효

    mParams.push_back(rp);
    mParamMap[param] = static_cast<UINT>(mParams.size() - 1);
    return *this;
}

ID3D12RootSignature* CRootSignatureBuilder::Build(ID3D12Device* device,
    D3D12_ROOT_SIGNATURE_FLAGS flags) {

    for (int i = 0; i < mParams.size(); ++i) {
        if (mParams[i].ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE) {
            assert(mParams[i].DescriptorTable.NumDescriptorRanges > 0);
            assert(mParams[i].DescriptorTable.pDescriptorRanges != nullptr);
        }
    }

    auto staticSamplers = GetStaticSamplers();
    D3D12_ROOT_SIGNATURE_DESC desc{};
    desc.NumParameters = static_cast<UINT>(mParams.size());
    desc.pParameters = mParams.data();
    desc.NumStaticSamplers = staticSamplers.size();
    desc.pStaticSamplers = staticSamplers.data();
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
