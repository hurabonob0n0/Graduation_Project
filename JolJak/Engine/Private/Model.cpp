#include "..\Public\Model.h"
#include "Bone.h"
#include "Mesh.h"
#include "Texture.h"
#include "Animation.h"

CModel::CModel(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList)
	: CComponent_DC(pDevice, pCommandList)
{
}

CModel::CModel(const CModel& rhs)
	: CComponent_DC(rhs)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	//, m_Meshes(rhs.m_Meshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Materials(rhs.m_Materials)
	, m_eModelType(rhs.m_eModelType)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	//, m_iCurrentAnimIndex(rhs.m_iCurrentAnimIndex)
	//, m_iNumAnimations(rhs.m_iNumAnimations)
{
	//for (auto& pPrototypeAnimation : rhs.m_Animations)
	//{
	//	m_Animations.push_back(pPrototypeAnimation->Clone());
	//}

	for (auto& mesh : rhs.m_Meshes)
		m_Meshes.push_back(mesh);

	int a = 0;

	for (auto& pPrototypeBone : rhs.m_Bones)
	{
		m_Bones.push_back(pPrototypeBone->Clone());
	}

	for (auto& Material : m_Materials)
	{
		for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
		{
			Safe_AddRef(Material.pMaterials[i]);
		}
	}

	for (auto& pMesh : m_Meshes)
		Safe_AddRef(pMesh);
}

HRESULT CModel::Initialize_Prototype(TYPE eModelType, const string& strModelFilePath, _fmatrix PivotMatrix)
{
	m_eModelType = eModelType;

	/* aiProcess_PreTransformVertices : */
	/* �� ���� �����ϴ� �޽õ��� Ȥ�� ������ ���´�� �׷������Ѵٸ�.(�޽��� �̸��� ���� �̸��� ���� ���� �����Ѵٸ�) */
	/* �� ������ ����������� �� �޽��� ������ �̸� ���ؼ� �ε��Ѵ�. */
	/* aiProcess_PreTransformVertices�ɼ��� �־ �ε��ϰԵǸ� �̹� ������ ���� ���°� ����Ǿ��ִ°��̴�. */
	/* ���Ŀ� ���� �����̰ԵǸ� ������ ���ִ� �� ���¸� ���� �ִ´ٶ�� �����Ѱ���. */
	/* �׷��� �ش� �ɼ��� �ְ� �ε��ϰԵǸ� �������ü���� �ִϸ��̼ǿ� ���õ� ������ �����ع�����. */
	/* �� �ɼ��� ���� �ε��ϴ� ���� �ݵ�� �ִϸ��̼��� ���� ��쿡�� ó���� �ؾߵǰڴ�. */
	_uint		iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	if (TYPE_NONANIM == eModelType)
		iFlag |= aiProcess_PreTransformVertices;

	/* m_pAIScene�ȿ� .fbx���Ͽ� ����ִ� �������� ����.  */
	m_pAIScene = m_Importer.ReadFile(strModelFilePath, iFlag);
	if (nullptr == m_pAIScene)
		return E_FAIL;

	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	/* m_pAIScene�ȿ� ���ִ� �������� �츮�� ����ϱ� ���� ���·� ����, �������ִ� �۾��� �����ϸ� �ǰڴ�. */
	/* mRootNode : ��� ���� ���� �����̵Ǵ� ����. */
	/* �� ���� ��� ���� ���о��� �� �ε��Ѵ�. */
	/* WHY? ������ ���������� ���� ���� ������ ����. */
	/* ���� �������� = ������ ���� * �θ���� ���� */
	/* ������ ������ �ʿ��ѰԾƴ϶� �� ���� �θ� ��ü �������°�?! */
	/* �θ��ڽİ��� ���� ���� + ���� ���¸� �ε��ϴ� �۾��� Ready_Bones�Լ��� ��Ȱ�̴�. */
	/* aiNode */
	if (FAILED(Ready_Bones(m_pAIScene->mRootNode, -1)))
		return E_FAIL;

	/* �� = �޽� + �޽� + ...*/
	/* ������ �޽� = VB, IB */
	/* �ᱹ, �޽��� ������ �ε������� �ε��ϴ� �۾�. VB, IB�� �����Ѵ�. */
	if (FAILED(Ready_Meshes()))
		return E_FAIL;

	/* ������ �ε��Ѵ�. ���� = �ȼ����� ���еǾ��� �� �ִ�. = �ؽ��x�� �ε��Ѵ�.  */
	/* aiTextureType = aiTexture_Diffuse aiTexture_Ambient, aiTexture_Emissive */
	if (FAILED(Ready_Materials(strModelFilePath)))
		return E_FAIL;

	/* �ִϸ��̼��� ������ �ε��Ѵ�.  */
	/* �ִϸ��̼��ϳ��� �ϳ��� ���� ��ü�� �ǹ��Ѵ�.(Idle, Attack, Run ...) */
	/* �ִϸ��̼�(aiAnimation) = �� �ִϸ��̼��� �ð�(Duration), �ӵ�(TickPerSecond), �� �ִϸ��̼��� ��Ʈ���ؾ��ϴ� ��(Channel)�� ���� */
	/* �ִϸ��̼��� ������ �մ� ä��(��)�� ������ŭ ä�� ��ü�� �����. */

	/* ä�ΰ�ü = Ű������ + Ű������ + Ű������ + Ű������ => Ư���� �ִϸ��̼��� ǥ���ϱ����� ���� � �ð��� � ���¸����ؾ��ϴ����� ���� ������ ������ �ʿ��� �� �ִ�. */
	/* Ű������(KEYFRAME) = Ư�� �ð�(fTime)�뿡 �� ���� � ����(vScale, vRotation, vTranslation)�� ���ؾ��ϴ����� ���� ����*/
	/* �ܼ� ���常 �س��´�. ���� �ִԿ��ó��� �����ϱ����ؼ��� ������ �ð������� ���¸� �Ź� �����Ͽ����Ѵ�. */
	//if (FAILED(Ready_Animations()))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{

	return S_OK;
}

//void CModel::Play_Animation(_bool isLoop, _float fTimeDelta,_uint BoneIndex, _fmatrix mat)
//{
//	/* ���� �ִϸ��̼��� �����ؾ��ϴ� ������ TransformationMatrix�� ���� �ִϸ��̼��� ���¿� �°� �����س��´�. */
//
//	/* m_iCurrentAnimIndex��° �ִϸ��̼��� �̿��ϰ� �մ� */
//	/* ��� ������ ����(TransformationMatrix)�� fTimeDelta�� ������ �ð��뿡 �µ��� �����Ѵ�. */
//	m_Animations[m_iCurrentAnimIndex]->Invalidate_TransformationMatrix(isLoop, m_Bones, fTimeDelta);
//
//	// �����ϱ� ���� ���� �ٲ��� ���� �ִٸ� �������ش�.
//	if (BoneIndex != -1)
//		Set_Matrix_to_Bone(BoneIndex, mat);
//
//	/* ��� ������ CombinedTransformationMatrix�� �����Ѵ�. */
//	/* CombinedTransformationMatrix = ����`s TransformationMatrix * �θ�`s CombinedTransformationMatrix*/
//	for (auto& pBone : m_Bones)
//	{
//		pBone->Invalidate_CombinedTransformationMatrix(m_Bones);
//	}
//
//}

/* ������ ���� ���µ��� ���̴��� ������. */
/* Bind_BoneMatrices()�Լ��� ȣ���� Render() �Լ����� ���� ȣ��Ǿ��־���Ѵ�. */
/* ���� �׸��� �������� ���� ���Ÿ� ���󰡾��ϱⶫ���� .*/

/* �������� �׸���. */
HRESULT CModel::Render(_uint iMeshIndex)
{
	/* iMeshIndex�� �ش��ϴ� �޽ÿ� ������ �ִ� ������ ��Ƽ� ���̴��� �����Ѵ�. */
	m_Meshes[iMeshIndex]->Render();
	return S_OK;
}

HRESULT CModel::Render()
{

	for (auto& Meshes : m_Meshes) {
		CMesh* pMesh = Meshes;
		pMesh->Render();
	}

	return S_OK;
}

HRESULT CModel::Bind_BoneMatrices(_uint iMeshIndex, CShader* pShader, const _char* pConstantName)
{
	/* iMeshIndex��° �޽ÿ� �����Ѵ�. */
	/* �츮�� �޽ø��� VB, iB�� �����. */
	/* �޽ø��� �׷�����. */
	/* �ѹ� �׸���. == Ư�� �޽��� �����鸸 ���̴��� ����Ѵ�. */
	/* �� ���� ���޵Ǵ� ������ Ư�� �޽ÿ� ������ �ִ� ���鸸 ���޵Ǿ���� �ʿ䰡 �ִ�. */
	/* + �������� ������ �ִ� BlendIndices�� ��ü���� �ε����� �ƴ϶�. �� �޽ÿ� ������ �ִ� ������ �ε����� �ǹ��ϴ°�. */
	return m_Meshes[iMeshIndex]->Bind_BoneMatrices(pShader, m_Bones, pConstantName, XMLoadFloat4x4(&m_PivotMatrix));
}

HRESULT CModel::Bind_Material(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eMaterialType)
{
	/*_uint		iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	return m_Materials[iMaterialIndex].pMaterials[eMaterialType]->Bind_ShaderResource(pShader, pConstantName, 0);*/
	return S_OK;
}

HRESULT CModel::Ready_Meshes()
{
	/* ���� ���� �����ϴ� �޽��� ����. */
	m_iNumMeshes = m_pAIScene->mNumMeshes;

	m_Meshes.reserve(m_iNumMeshes);

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		/* VB, IB�� �����. */
		CMesh* pMesh = CMesh::Create(m_Device, m_CommandList, m_eModelType, m_pAIScene->mMeshes[i], m_Bones, XMLoadFloat4x4(&m_PivotMatrix));
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(const string& strModelFilePath)
{
	///* aiTextureType : ���� ��Ʈ(��ǻ��, ����ŧ��, ���Ǿ�Ʈ, ���, ��Ÿ �����꿡 �ʿ��� �������� ������) �� ����. */	
	//m_iNumMaterials = m_pAIScene->mNumMaterials;

	//_char		szDrive[MAX_PATH] = "";
	//_char		szDirectory[MAX_PATH] = "";
	//_char		szFileName[MAX_PATH] = "";
	//_char		szExt[MAX_PATH] = "";

	//_char		szTextureFilePath[MAX_PATH] = "";

	//_splitpath_s(strModelFilePath.c_str(), szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);

	//for (size_t i = 0; i < m_iNumMaterials; i++)
	//{
	//	MODEL_MATERIAL			ModelMaterials = {};

	//	aiMaterial*		pAIMaterial = m_pAIScene->mMaterials[i];

	//	for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
	//	{			
	//		aiString		strTexturePath;
	//		/* j������ ǥ���ϴ� �ؽ����� ����(�ؽ��� ���)�� �����ְڴ� */
	//		if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strTexturePath)))
	//			continue;			
	//		
	//		_splitpath_s(strTexturePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

	//		strcpy_s(szTextureFilePath, szDrive);
	//		strcat_s(szTextureFilePath, szDirectory);
	//		strcat_s(szTextureFilePath, szFileName);
	//		strcat_s(szTextureFilePath, szExt);

	//		_tchar			szFullPath[MAX_PATH] = L"";
	//		MultiByteToWideChar(CP_ACP, 0, szTextureFilePath, strlen(szTextureFilePath), szFullPath, MAX_PATH);

	//		ModelMaterials.pMaterials[j] = CTexture::Create(m_pDevice, m_pContext, szFullPath);

	//		if (nullptr == ModelMaterials.pMaterials[j])
	//			return E_FAIL;
	//	}	

	//	m_Materials.push_back(ModelMaterials);
	//}

	return S_OK;
}

HRESULT CModel::Ready_Bones(aiNode* pAINode, _int iParentBoneIndex)
{
	CBone* pBone = CBone::Create(pAINode, iParentBoneIndex);
	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.push_back(pBone);

	_int iParentIndex = m_Bones.size() - 1;

	for (size_t i = 0; i < pAINode->mNumChildren; i++)
	{
		Ready_Bones(pAINode->mChildren[i], iParentIndex);
	}
	return S_OK;
}

//HRESULT CModel::Ready_Animations()
//{
//	m_iNumAnimations = m_pAIScene->mNumAnimations;
//
//	for (size_t i = 0; i < m_iNumAnimations; i++)
//	{
//		CAnimation*		pAnimation = CAnimation::Create(m_pAIScene->mAnimations[i], m_Bones);
//		if (nullptr == pAnimation)
//			return E_FAIL;
//
//		m_Animations.push_back(pAnimation);
//	}
//
//	return S_OK;
//}

void CModel::Set_Matrix_to_Bone(_uint iBoneIndex, _matrix Mat)
{
	m_Bones[iBoneIndex]->Mul_TransformationMatrix(Mat);
}

CModel* CModel::Create(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, TYPE eModelType, const string& strModelFilePath, _fmatrix PivotMatrix)
{
	CModel* pInstance = new CModel(pDevice, pCommandList);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, strModelFilePath, PivotMatrix)))
	{
		MSG_BOX("Failed to Created : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	/*for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);

	m_Animations.clear();*/

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);

	m_Bones.clear();

	for (auto& Material : m_Materials)
	{
		for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
		{
			Safe_Release(Material.pMaterials[i]);
		}
	}

	m_Materials.clear();

	for (auto& pMesh : m_Meshes)
		Safe_Release(pMesh);

	m_Importer.FreeScene();

}
