#pragma once

/* 어떤 모델이든 다수의 메시로 구성되어 있다. */
/* 모델 = 메시 + 메시 + 메시... */
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
	/* .fbx파일로부터 읽어온 정보를 1차적으로 저장한다.*/
	Assimp::Importer			m_Importer;

	// m_pAIScene = m_Importer.ReadFile( 경로, 플래그 );
	const aiScene*				m_pAIScene = { nullptr };

private:
	TYPE						m_eModelType = { TYPE_END };
	_float4x4					m_PivotMatrix;

	/* 모델은 여러개의 메시로 구성되어있다.*/
	/* 모델 == 메시(VB, IB) + 메시(VB, IB) + 메시(VB, IB)*/
	/* 메시객체들를 만든다.(정점과 인덱스버퍼를 생성한다. )*/
	_uint						m_iNumMeshes = { 0 };
	vector<class CMesh*>		m_Meshes;

	_uint						m_iNumMaterials = { 0 };
	vector<MODEL_MATERIAL>		m_Materials;

	vector<class CBone*>			m_Bones;

	/*_uint						m_iNumAnimations = { 0 };
	_uint						m_iCurrentAnimIndex = { 0 };
	vector<class CAnimation*>	m_Animations;*/

	/* 1차적으로 저장된 데이터로부터 내가 사용할 정보를 꺼내와서 추후 사용하기 편하도록 내 스타일대로 정리한다. */
private:	
	HRESULT Ready_Meshes();
	/* 재질을 롤드한다. == 픽셀마다 재질이 부여되어있어야한다. == 텍스쳐. */
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

