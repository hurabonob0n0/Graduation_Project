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
	/* 이 모델을 구성하는 메시들이 혹여 지정된 상태대로 그려져야한다면.(메시의 이름과 같은 이름을 가진 뼈가 존재한다면) */
	/* 그 지정된 상태행렬응ㄹ 이 메시의 정점에 미리 곱해서 로드한다. */
	/* aiProcess_PreTransformVertices옵션을 넣어서 로드하게되면 이미 정점에 뼈의 상태가 적용되어있는것이다. */
	/* 추후에 뼈를 움직이게되면 정점에 들어가있는 뼈 상태를 빼서 넣는다라는 복잡한과정. */
	/* 그래서 해당 옵션을 주고 로드하게되면 어심프자체에서 애니메이션에 관련된 정보를 삭제해버린다. */
	/* 이 옵션을 통해 로드하는 경우는 반드시 애니메이션이 없는 경우에만 처리를 해야되겠다. */
	_uint		iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	if (TYPE_NONANIM == eModelType)
		iFlag |= aiProcess_PreTransformVertices;

	/* m_pAIScene안에 .fbx파일에 담겨있던 정보들이 담긴다.  */
	m_pAIScene = m_Importer.ReadFile(strModelFilePath, iFlag);
	if (nullptr == m_pAIScene)
		return E_FAIL;

	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	/* m_pAIScene안에 들어가있는 정보들을 우리가 사용하기 좋은 형태로 생성, 저장해주는 작업을 수행하면 되겠다. */
	/* mRootNode : 모든 뼈의 가장 시작이되는 뼈다. */
	/* 내 모델의 모든 뼈를 구분없이 다 로드한다. */
	/* WHY? 뼈들의 상태정보를 가진 뼈를 만들어내는 과정. */
	/* 뼈의 상태정보 = 내뼈의 상태 * 부모뼈의 상태 */
	/* 내뼈의 정보만 필요한게아니라 이 뼈의 부모가 대체 누구였는가?! */
	/* 부모자식간의 뼈의 관계 + 뼈의 상태를 로드하는 작업이 Ready_Bones함수의 역활이다. */
	/* aiNode */
	if (FAILED(Ready_Bones(m_pAIScene->mRootNode, -1)))
		return E_FAIL;

	/* 모델 = 메시 + 메시 + ...*/
	/* 각각의 메시 = VB, IB */
	/* 결국, 메시의 정점과 인덱스들을 로드하는 작업. VB, IB를 생성한다. */
	if (FAILED(Ready_Meshes()))
		return E_FAIL;

	/* 재질을 로드한다. 재질 = 픽셀별로 구분되어질 수 있다. = 텍스쳘르 로드한다.  */
	/* aiTextureType = aiTexture_Diffuse aiTexture_Ambient, aiTexture_Emissive */
	if (FAILED(Ready_Materials(strModelFilePath)))
		return E_FAIL;

	/* 애니메이션의 정보를 로드한다.  */
	/* 애니메이션하나는 하나의 동작 전체를 의미한다.(Idle, Attack, Run ...) */
	/* 애니메이션(aiAnimation) = 이 애니메이션의 시간(Duration), 속도(TickPerSecond), 이 애니메이션이 컨트롤해야하는 뼈(Channel)의 정보 */
	/* 애니메이션이 가지고 잇는 채널(뼈)의 갯수만큼 채널 객체를 만든다. */

	/* 채널객체 = 키프레임 + 키프레임 + 키프레임 + 키프레임 => 특ㅈ넝 애니메이션을 표현하기위해 뼈가 어떤 시간에 어떤 상태를취해야하는지에 대한 정보가 여러개 필요할 수 있다. */
	/* 키프레임(KEYFRAME) = 특정 시간(fTime)대에 이 뼈가 어떤 상태(vScale, vRotation, vTranslation)를 취해야하는지에 대한 정보*/
	/* 단순 저장만 해놓는다. 추후 애님에시녀을 구동하기위해서는 뼈들의 시간에따른 상태를 매번 적용하여야한다. */
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
//	/* 현재 애니메이션이 갱신해야하는 뼈들의 TransformationMatrix를 현재 애니메이션의 상태에 맞게 갱신해놓는다. */
//
//	/* m_iCurrentAnimIndex번째 애니메이션이 이용하고 잇는 */
//	/* 모든 뼈들의 상태(TransformationMatrix)를 fTimeDelta을 누적한 시간대에 맞도록 셋팅한다. */
//	m_Animations[m_iCurrentAnimIndex]->Invalidate_TransformationMatrix(isLoop, m_Bones, fTimeDelta);
//
//	// 갱신하기 전에 내가 바꿔줄 뼈가 있다면 설정해준다.
//	if (BoneIndex != -1)
//		Set_Matrix_to_Bone(BoneIndex, mat);
//
//	/* 모든 뼈들의 CombinedTransformationMatrix를 갱신한다. */
//	/* CombinedTransformationMatrix = 내뼈`s TransformationMatrix * 부모`s CombinedTransformationMatrix*/
//	for (auto& pBone : m_Bones)
//	{
//		pBone->Invalidate_CombinedTransformationMatrix(m_Bones);
//	}
//
//}

/* 사전에 뼈의 상태들을 셰이더로 던진다. */
/* Bind_BoneMatrices()함수의 호출이 Render() 함수보다 먼저 호출되어있어야한다. */
/* 지금 그리는 정점들이 뼈의 상탤르 따라가야하기땜누에 .*/

/* 정점들을 그린다. */
HRESULT CModel::Render(_uint iMeshIndex)
{
	/* iMeshIndex에 해당하는 메시에 영향을 주는 뼈들을 모아서 셰이더로 전달한다. */
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
	/* iMeshIndex번째 메시에 접근한다. */
	/* 우리는 메시마다 VB, iB를 만든다. */
	/* 메시마다 그려낸다. */
	/* 한번 그린다. == 특정 메시의 정점들만 셰이더를 통과한다. */
	/* 이 때에 전달되는 뼈들은 특정 메시에 영향을 주는 뼈들만 전달되어야할 필요가 있다. */
	/* + 정점들이 가지고 있는 BlendIndices는 전체뼈의 인덱스가 아니라. 이 메시에 영향을 주는 뼈들읜 인덱스를 의미하는거. */
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
	/* 현재 모델을 구성하는 메시의 갯수. */
	m_iNumMeshes = m_pAIScene->mNumMeshes;

	m_Meshes.reserve(m_iNumMeshes);

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		/* VB, IB를 만든다. */
		CMesh* pMesh = CMesh::Create(m_Device, m_CommandList, m_eModelType, m_pAIScene->mMeshes[i], m_Bones, XMLoadFloat4x4(&m_PivotMatrix));
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(const string& strModelFilePath)
{
	///* aiTextureType : 재질 세트(디퓨즈, 스펙큘러, 앰피언트, 노멀, 기타 빛연산에 필요한 여러가지 데이터) 의 갯수. */	
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
	//		/* j재질을 표현하는 텍스쳐의 정보(텍스쳐 경로)를 얻어와주겠다 */
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
