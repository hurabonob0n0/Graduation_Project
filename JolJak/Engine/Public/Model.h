#pragma once

/* � ���̵� �ټ��� �޽÷� �����Ǿ� �ִ�. */
/* �� = �޽� + �޽� + �޽�... */
#include "Component.h"


BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent_DC
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END};
private:
	CModel(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

//public:
//	void Set_Animation(_uint iAnimIndex) {
//		m_iCurrentAnimIndex = iAnimIndex;
//	}

public:
	virtual HRESULT Initialize_Prototype(TYPE eModelType, const string& strModelFilePath, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg);

public:
	//void Play_Animation(_bool isLoop, _float fTimeDelta, _uint BoneIndex = -1, _fmatrix mat = XMMatrixIdentity());
	HRESULT Render(_uint iMeshIndex);
	HRESULT Render();

public:
	HRESULT Bind_BoneMatrices(_uint iMeshIndex, class CShader* pShader, const _char* pConstantName);
	HRESULT Bind_Material(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eMaterialType);


private:
	/* .fbx���Ϸκ��� �о�� ������ 1�������� �����Ѵ�.*/
	Assimp::Importer			m_Importer;

	// m_pAIScene = m_Importer.ReadFile( ���, �÷��� );
	const aiScene*				m_pAIScene = { nullptr };

private:
	TYPE						m_eModelType = { TYPE_END };
	_float4x4					m_PivotMatrix;

	/* ���� �������� �޽÷� �����Ǿ��ִ�.*/
	/* �� == �޽�(VB, IB) + �޽�(VB, IB) + �޽�(VB, IB)*/
	/* �޽ð�ü�鸦 �����.(������ �ε������۸� �����Ѵ�. )*/
	_uint						m_iNumMeshes = { 0 };
	vector<class CMesh*>		m_Meshes;

	_uint						m_iNumMaterials = { 0 };
	vector<MODEL_MATERIAL>		m_Materials;

	vector<class CBone*>			m_Bones;

	/*_uint						m_iNumAnimations = { 0 };
	_uint						m_iCurrentAnimIndex = { 0 };
	vector<class CAnimation*>	m_Animations;*/

	/* 1�������� ����� �����ͷκ��� ���� ����� ������ �����ͼ� ���� ����ϱ� ���ϵ��� �� ��Ÿ�ϴ�� �����Ѵ�. */
private:	
	HRESULT Ready_Meshes();
	/* ������ �ѵ��Ѵ�. == �ȼ����� ������ �ο��Ǿ��־���Ѵ�. == �ؽ���. */
	HRESULT Ready_Materials(const string& strModelFilePath);
	HRESULT Ready_Bones(aiNode* pNode, _int iParentBoneIndex);
	//HRESULT Ready_Animations();

private:
	void Set_Matrix_to_Bone(_uint iBoneIndex,_matrix mat);

public:
	static CModel* Create(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, TYPE eModelType, const string& strModelFilePath, _fmatrix PivotMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

	
};

END

