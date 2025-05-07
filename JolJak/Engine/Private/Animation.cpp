#include "..\Public\Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation & rhs)
	: m_Duration(rhs.m_Duration)
	, m_fTickPerSecond(rhs.m_fTickPerSecond)
	, m_CurrentAnimPosition(rhs.m_CurrentAnimPosition)
	, m_iNumChannels(rhs.m_iNumChannels)
	, m_Channels(rhs.m_Channels)
	, m_isFinished(rhs.m_isFinished)
	, m_CurrentKeyFrameIndices(rhs.m_CurrentKeyFrameIndices)
{
	strcpy_s(m_szName, rhs.m_szName);

	for (auto& pChannel : m_Channels)
	{
		Safe_AddRef(pChannel);
	}
	
}

HRESULT CAnimation::Initialize(const aiAnimation * pAIAnimation, const vector<class CBone*>& Bones)
{
	strcpy_s(m_szName, pAIAnimation->mName.data);
	m_Duration = pAIAnimation->mDuration;
	m_fTickPerSecond = pAIAnimation->mTicksPerSecond;

	m_iNumChannels = pAIAnimation->mNumChannels;

	m_CurrentKeyFrameIndices.resize(m_iNumChannels);

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel*		pChannel = CChannel::Create(pAIAnimation->mChannels[i], Bones);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

void CAnimation::Invalidate_TransformationMatrix(_bool isLoop, const vector<class CBone*>& Bones, _float fTimeDelta)
{
	/* ���� �� �ִϸ��̼��� ��� ��ġ�� ���Ѵ�. */
	m_CurrentAnimPosition += m_fTickPerSecond * fTimeDelta;

	if (m_CurrentAnimPosition >= m_Duration)
	{
		m_isFinished = true;

		if (true == isLoop)
		{
			m_isFinished = false;
			m_CurrentAnimPosition = 0.f;
		}
	}

	/* �� �ִϸ��̼ǿ��� ����ϴ� ��� ���� ���¸� �����Ѵ�. */
	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		/* �� ���� ���¸� �������ֱ����� �����س��� �ð�(m_CurrentAnimPosition)�� �� ä�ο��� �����ش�. */
		/* �� ä���� �޾ƿ� �ð��� ���� ��������� ������. */
		m_Channels[i]->Invalidate_TransformationMatrix(Bones, &m_CurrentKeyFrameIndices[i], m_CurrentAnimPosition);
	}
}

CAnimation * CAnimation::Create(const aiAnimation * pAIAnimation, const vector<class CBone*>& Bones)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pAIAnimation, Bones)))
	{
		MSG_BOX("Failed to Created : CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation * CAnimation::Clone()
{
	return new CAnimation(*this);
}

void CAnimation::Free()
{
	m_CurrentKeyFrameIndices.clear();

	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}
