#pragma once
#include "Component.h"

BEGIN(Engine)

class CRenderObject;

class ENGINE_DLL CRenderer : public CComponent_DC
{
public:
	enum RENDERGROUP { RG_PRIORITY, RG_NONLIGHT, RG_NONBLEND, RG_BLEND, RG_UI, RG_END };

public:
	CRenderer(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CRenderer() = default;

public:
	void AddtoRenderObjects(RENDERGROUP RG, CRenderObject* pRenderObject);
	void ResetRenderObjects();

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;
	void Render();

private:
	vector<CRenderObject*> m_vRenderObjects[RG_END];

public:
	void Render_Priority();
	void Render_NonLight();
	void Render_NonBlend();
	void Render_Blend();
	void Render_UI();

private:
	//Todo : ���߿� PSO, RootSignature, FrameResource ��� �������� �ʿ��� �͵��� ��� ��Ƴ��� Render()�Լ� �ȿ��� ó���ϵ��� �����.

public:
	static CRenderer* Create(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	CComponent* Clone(void* pArg) override;
	void Free() override;
};

END