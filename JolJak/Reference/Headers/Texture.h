#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTexture : public CComponent_DC
{
public:
	CTexture(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	CTexture(CTexture& rhs);
	virtual~CTexture() = default;

public:
	void Initialize_Prototype(const wchar_t* Filename);
	void Initialize();

public:
	ID3D12Resource* Get_Texture() { return m_Resource.Get(); }

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> m_Resource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_UploadHeap = nullptr;

public:
	CComponent* Clone(void* pArg) override;
	static CTexture* Create(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, const wchar_t* Filename);
};

END