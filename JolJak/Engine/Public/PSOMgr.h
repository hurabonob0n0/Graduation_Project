#pragma once
#include "Base.h"
#include "PSO.h"

BEGIN(Engine)

class CPSOMgr : public CBase
{
	DECLARE_SINGLETON(CPSOMgr)

public:
	CPSOMgr() = default;
	virtual ~CPSOMgr() = default;

public:
	// PSO �߰� (InputLayout Ÿ�� �߰�!)
	HRESULT AddPSO(const string& PSOName, ID3D12RootSignature* pRootSignature, CShader* VS, CShader* PS, CGraphic_Device* pGD, ID3D12Device* pDevice, CPSO::INPUTLAYOUT_TYPE eLayout);

	// PSO ���� ���
	CPSO* GetPSOObj(const string& PSOName) const;

	// PSO ���� ����
	ID3D12PipelineState* Get(const string& PSOName) const;

private:
	unordered_map<string, CPSO*> m_PSOs;

public:
	virtual void Free() override;
};

END
