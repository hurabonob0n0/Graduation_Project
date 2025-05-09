#include "..\Public\Mesh.h"
#include "Bone.h"
#include "Shader.h"

CMesh::CMesh(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList)
	: CVIBuffer(pDevice, pCommandList)
{
}

CMesh::CMesh(CMesh & rhs) : CVIBuffer(rhs)
{
}

HRESULT CMesh::Initialize_Prototype(CModel::TYPE eModelType, const aiMesh * pAIMesh, const vector<class CBone*>& Bones, _fmatrix PivotMatrix)
{
	//Todo: //m_iMaterialIndex = pAIMesh->mMaterialIndex;
	strcpy_s(m_szName, pAIMesh->mName.data);


#ifndef _DEBUG
	D3D12_GPU_VIRTUAL_ADDRESS	m_VertexBufferGPUaddress;
	D3D12_GPU_VIRTUAL_ADDRESS	m_IndexBufferGPUaddress;

#endif // !_DEBUG

	m_VertexNum = pAIMesh->mNumVertices;
	m_IndexNum = pAIMesh->mNumFaces * 3;
	m_IndexFormat = DXGI_FORMAT_R32_UINT;
	m_PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_IndexBufferByteSize = m_IndexNum * sizeof(std::uint32_t);



	//47872 * 3

#pragma region VERTEX_BUFFER

	HRESULT		hr = eModelType == CModel::TYPE_NONANIM ? Ready_NonAnim_Mesh(pAIMesh, PivotMatrix) : Ready_Anim_Mesh(pAIMesh, Bones);
	if (FAILED(hr))
		return E_FAIL;
	
#pragma endregion


#pragma region INDEX_BUFFER
	_ulong*		pIndices = new _ulong[m_IndexNum];
	ZeroMemory(pIndices, m_IndexBufferByteSize);

	_uint		iNumIndices = 0;

	for (size_t i = 0; i < pAIMesh->mNumFaces; ++i)
	{
		aiFace	AIFace = pAIMesh->mFaces[i];	

		pIndices[iNumIndices++] = AIFace.mIndices[0];
		pIndices[iNumIndices++] = AIFace.mIndices[1];
		pIndices[iNumIndices++] = AIFace.mIndices[2];
	}

	__super::Create_Buffer(&m_IndexBufferGPU,&m_IndexBufferUploader, pIndices, m_IndexBufferByteSize);

	int a = 0;

	delete[] pIndices;
	pIndices = nullptr;
#pragma endregion

	return S_OK;
}

HRESULT CMesh::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CMesh::Bind_BoneMatrices(CShader * pShader, const vector<CBone*>& Bones, const _char * pConstantName, _fmatrix PivotMatrix)
{
	/* vtf : Vertex Texture Fetch */
	/* �޽ÿ��� ������ �ִ� ������ ����� ���̴��� �ѹ��� �����ֱ����ؼ� ��Ĺ迭�� �������Ѵ�. */
	_float4x4		BoneMatrices[128] = {};
	ZeroMemory(BoneMatrices, sizeof(_float4x4) * 128);

	_uint			iNumBones = 0;

	for (size_t i = 0; i < m_iNumBones; i++)
	{	
		/* m_OffsetMatices : Ư�� �ִϸ��̼��� ǥ���ϱ����� ���� ���� �ٸ� ���¿� �𵨿��Ե� ����� �� �ִ� ��Ȳ�� ��������� .*/
		/* �� Ư�� �𵨿� �ٱ����ؼ��� �� ���� ������� ������ �ʿ��ߴ�. */
		/* ���� �������� = m_OffsetMatices * CombinedMatrix * PivotMatrix */
		XMStoreFloat4x4(&BoneMatrices[i], XMLoadFloat4x4(&m_OffsetMatices[i]) * 
			XMLoadFloat4x4(Bones[m_BoneIndices[i]]->Get_CombinedMatrix()) * PivotMatrix);
	}

	return S_OK;//Todo:pShader->Bind_Matrices(pConstantName, BoneMatrices, m_iNumBones);
}

HRESULT CMesh::Ready_NonAnim_Mesh(const aiMesh * pAIMesh, _fmatrix PivotMatrix)
{
	m_VertexByteStride = sizeof(VTXPOSNOR);
	m_VertexBufferByteSize = m_VertexByteStride*m_VertexNum;

	VTXPOSNOR* pVertices = new VTXPOSNOR[m_VertexNum];
	ZeroMemory(pVertices, sizeof(VTXPOSNOR) * m_VertexNum);

	for (size_t i = 0; i < m_VertexNum; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PivotMatrix));

		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PivotMatrix));

		/*memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));*/
	}

	__super::Create_Buffer(&m_VertexBufferGPU,&m_VertexBufferUploader, pVertices, m_VertexBufferByteSize);

	delete[] pVertices;
	pVertices = nullptr;

	return S_OK;
}

HRESULT CMesh::Ready_Anim_Mesh(const aiMesh * pAIMesh, const vector<class CBone*>& Bones)
{
	m_VertexByteStride = sizeof(VTXANIMMESH);
	m_VertexBufferByteSize = m_VertexByteStride * m_VertexNum;


	VTXANIMMESH*		pVertices = new VTXANIMMESH[m_VertexNum];
	ZeroMemory(pVertices, m_VertexBufferByteSize);

	for (size_t i = 0; i < m_VertexNum; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}	

	/* �޽��� ���� ����(mNumBones). (��ü���� ����( x), �� �޽ÿ��� ���⤷�� �ִ� ���� ����(o) */
	/* vBlendIndices : �� �޽ÿ��� ���⤷�� �ִ� �������� �ε����� */
	/* vBlendWeights : */
	m_iNumBones = pAIMesh->mNumBones;

	for (size_t i = 0; i < m_iNumBones; i++)
	{
		aiBone*		pAIBone = pAIMesh->mBones[i];

		/* �޽ÿ�  mOffsetMatrix����� ����ִ� ���´�. */
		/* ����� �� ��Ʈ�� ���� �ٸ� �޽ÿ� �������ֱ����� �ʿ��� ���� ���¸� ������ �ִ� ���. */
		_float4x4		OffsetMatrix;

		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));		

		XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

		m_OffsetMatices.push_back(OffsetMatrix);

		_uint		iBoneIndex = 0;

		auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool {					
			if (0 == strcmp(pBone->Get_BoneName(), pAIBone->mName.data))
				return true;
			++iBoneIndex;
			return false;
		});	

		m_BoneIndices.push_back(iBoneIndex);

		for (size_t j = 0; j < pAIBone->mNumWeights; j++)
		{
			if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.x = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x = pAIBone->mWeights[j].mWeight;
			}
			
			else if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.y = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y = pAIBone->mWeights[j].mWeight;
			}
			else if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.z = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z = pAIBone->mWeights[j].mWeight;
			}
			else if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.w)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.w = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.w = pAIBone->mWeights[j].mWeight;
			}			
		}
	}

	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;

		_uint		iBoneIndex = 0;

		auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool {
			if (0 == strcmp(pBone->Get_BoneName(), m_szName))
				return true;
			++iBoneIndex;
			return false;
		});

		_float4x4		OffsetMatrix;

		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());	

		m_OffsetMatices.push_back(OffsetMatrix);

		m_BoneIndices.push_back(iBoneIndex);
	}

	__super::Create_Buffer(&m_VertexBufferGPU, &m_VertexBufferUploader, pVertices, m_VertexBufferByteSize);

	delete[] pVertices;
	pVertices = nullptr;

	return S_OK;
}

CMesh* CMesh::Create(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, CModel::TYPE eModelType, const aiMesh* pAIMesh, const vector<class CBone*>& Bones, _fmatrix PivotMatrix)
{
	CMesh* pInstance = new CMesh(pDevice, pCommandList);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, pAIMesh, Bones, PivotMatrix)))
	{
		MSG_BOX("Failed to Created : CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent_DC* CMesh::Clone(void * pArg)
{
	CMesh* pInstance = new CMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMesh::Free()
{
	__super::Free();

}
