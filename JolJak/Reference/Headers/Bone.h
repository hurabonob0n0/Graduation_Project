#pragma once

#include "Base.h"

/* Assimp : aiBone, aiNode, aiNodeAnim*/

/* aiBone : �� ���� � �����鿡�� �� �ۼ�Ʈ(0 ~ 1)�� ������ �ִ°�? */
/* aiNode : �� ���� ��Ӱ��踦 ǥ���ϰ� �� ���� ���������� ǥ���Ѵ�. */

BEGIN(Engine)

class CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;

public:
	const _char* Get_BoneName() const {
		return m_szName;
	}

	const _float4x4* Get_CombinedMatrix() const {
		return &m_CombindTransformationMatrix;
	}

public:
	void Set_TransformationMatrix(_fmatrix TransformationMatrix) {
		XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix);	
	}
	void Mul_TransformationMatrix(_fmatrix Transformmatrix)
	{
		XMStoreFloat4x4(&m_TransformationMatrix, (XMLoadFloat4x4(&m_TransformationMatrix) * Transformmatrix));
	}

public:
	HRESULT Initialize(const aiNode* pAINode, _int iParentBoneIndex);
	void Invalidate_CombinedTransformationMatrix(const vector<CBone*>& Bones);

private:
	_char				m_szName[MAX_PATH] = "";
	_float4x4			m_TransformationMatrix; /* ���� ������ ���� */
	_float4x4			m_CombindTransformationMatrix; /* ���� ������ ���� * �θ���� ����(m_CombindTransformationMatrix) */
	
	/* �θ���� ���� ���¸� �����س��´�. */		
	_int				m_iParentBoneIndex = { 0 };

public:
	static CBone* Create(const aiNode* pAINode, _int iParentBoneIndex);
	CBone* Clone();
	virtual void Free() override;
};

END