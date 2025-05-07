#pragma once

#include "Base.h"

/* Ư�� �ִϸ��̼ǿ��� �����ϴ� ���� ����. */
/* �ִϸ��̼��� ��ü ����Ǵ� ���ȿ� ������ �ð��뿡 ���� �� ���� ���µ鸦 �����Ѵ�. */

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones);
	void Invalidate_TransformationMatrix(const vector<class CBone*>& Bones, _uint* pCurrentKeyFrameIndex, _float fCurrentAnimPosition);

private:
	_char					m_szName[MAX_PATH] = "";

	/* ���� �� ���� �� �ִϸ��̼��� ����Ҷ� � Ű�����ӱ��� �����Ͽ� ���¸� ǥ���ϰ� �־��°�?! */
	// _uint					m_iCurrentKeyFrame = { 0 };
	_uint					m_iNumKeyFrames = { 0 };
	vector<KEYFRAME>		m_KeyFrames;
	_uint					m_iBoneIndex = { 0 };

public:
	static CChannel* Create(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones);
	virtual void Free() override;
};

END