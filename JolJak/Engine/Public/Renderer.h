#pragma once
#include "Component.h"

BEGIN(Engine)

class CRenderObject;

class ENGINE_DLL CRenderer : public CComponent_DC
{
public:
	CRenderer(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CRenderer() = default;

public:
	void AddtoRenderObjects(CRenderObject* pRenderObject);
	void ResetRenderObjects();

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;
	void Render();

private:
	vector<CRenderObject*> m_vRenderObjects;

private:
	//Todo : ���߿� PSO, RootSignature, FrameResource ��� �������� �ʿ��� �͵��� ��� ��Ƴ��� Render()�Լ� �ȿ��� ó���ϵ��� �����.

public:
	static CRenderer* Create(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	CComponent* Clone(void* pArg) override;
	void Free() override;
};

END