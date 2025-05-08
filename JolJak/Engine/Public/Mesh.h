#pragma once

/* ���� �����ϴ� �ϳ��� ����. */
/* ������ �ε����� �����ϱ����� �����ε� Ŭ������. */
#include "VIBuffer.h"
#include "Model.h"


BEGIN(Engine)

class CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	CMesh(CMesh& rhs);
	virtual ~CMesh() = default;

public:
	virtual HRESULT Initialize_Prototype(CModel::TYPE eModelType, const aiMesh* pAIMesh, const vector<class CBone*>& Bones, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg);

public:
	HRESULT Bind_BoneMatrices(CShader * pShader, const vector<class CBone*>& Bones, const _char * pConstantName, _fmatrix PivotMatrix);

private:
	char				m_szName[MAX_PATH] = "";
	_uint				m_iMaterialIndex = { 0 };
	_uint				m_iNumBones = { 0 };
	vector<_uint>		m_BoneIndices; /* �� �޽ÿ� ������ �ִ� ������ �ε���(��ü������)���� ��Ƶд�. */
	vector<_float4x4>	m_OffsetMatices;

private:
	HRESULT Ready_NonAnim_Mesh(const aiMesh* pAIMesh, _fmatrix PivotMatrix);
	HRESULT Ready_Anim_Mesh(const aiMesh* pAIMesh, const vector<class CBone*>& Bones);

public:
	static CMesh* Create(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, CModel::TYPE eModelType, const aiMesh* pAIMesh, const vector<class CBone*>& Bones, _fmatrix PivotMatrix);
	virtual CComponent_DC* Clone(void* pArg);
	virtual void Free() override;
};

END