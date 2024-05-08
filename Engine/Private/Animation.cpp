#include "Animation.h"

#include "Channel.h"
#include "EventCaller.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& rhs)
	: m_Duration(rhs.m_Duration)
	, m_fTickPerSecond(rhs.m_fTickPerSecond)
	, m_CurrentAnimPosition(rhs.m_CurrentAnimPosition)
	, m_iNumChannels(rhs.m_iNumChannels)
	, m_Channels(rhs.m_Channels)
	, m_bFinished(rhs.m_bFinished)
	, m_CurrentKeyFrameIndices(rhs.m_CurrentKeyFrameIndices)
{
	strcpy_s(m_szName, rhs.m_szName);

	for (auto& pChannel : m_Channels)
	{
		Safe_AddRef(pChannel);
	}
}

HRESULT CAnimation::Reset_EventCaller()
{
	if (m_pEventCaller == nullptr)
		return S_OK;

	return m_pEventCaller->Reset_Event();
}

HRESULT CAnimation::Bind_EventCaller(CEventCaller* pEventCaller)
{
	m_pEventCaller = pEventCaller;

	return S_OK;
}

HRESULT CAnimation::Invalidate_EventCaller(CEventCaller* pEventCaller)
{
	Safe_Release(m_pEventCaller);
	m_pEventCaller = pEventCaller;

	return S_OK;
}

void CAnimation::Reset_Anim()
{
	m_bFinished = FALSE;
	m_CurrentAnimPosition = 0.f;
	fill(m_CurrentKeyFrameIndices.begin(), m_CurrentKeyFrameIndices.end(), 0);
}

_bool CAnimation::isFunctionTrue()
{
	return m_pEventCaller->isFunctionTrue();
}

CChannel* CAnimation::Find_Channel(_uint iBoneIdx)
{
	for (auto& Channel : m_Channels)
	{
		if (Channel->Get_BoneIndex() == iBoneIdx)
			return Channel;
	}

	return nullptr;
}

CChannel* CAnimation::Find_Channel(string strBoneTag)
{
	for (auto& Channel : m_Channels)
	{
		if (Channel->Get_BoneTag() == strBoneTag)
			return Channel;
	}

	return nullptr;
}

_float CAnimation::Invalidate_TransformationMatrix(_bool isLoop, class CTransform* pTransform, const vector<class CBone*>& Bones, _float fTimeDelta)
{
	m_CurrentAnimPosition += m_fTickPerSecond * fTimeDelta;

	if (m_CurrentAnimPosition >= m_Duration)
	{
		m_bFinished = TRUE;

		if (m_pEventCaller != nullptr)
			m_pEventCaller->Reset_Event();

		if (true == isLoop)
		{
			m_bFinished = FALSE;
			m_CurrentAnimPosition = 0.f;
		}
	}

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		m_Channels[i]->Invalidate_TransformationMatrix(Bones, &m_CurrentKeyFrameIndices[i], m_CurrentAnimPosition);
	}

	if (m_pEventCaller != nullptr)
		m_pEventCaller->Invalidate_Event(pTransform, Bones, m_CurrentAnimPosition);

	return m_CurrentAnimPosition;
}

_float CAnimation::Invalidate_TransformationMatrix(_bool isLoop, CTransform* pTransform, const vector<class CBone*>& Bones, _float fDuration, _float fTimeDelta)
{
	m_CurrentAnimPosition += m_fTickPerSecond * fTimeDelta;

	if (m_CurrentAnimPosition > (m_Duration / fDuration))
	{
		m_bFinished = TRUE;

		if (m_pEventCaller != nullptr)
			m_pEventCaller->Reset_Event();

		if (true == isLoop)
		{
			m_bFinished = FALSE;
			m_CurrentAnimPosition = 0.f;
		}
	}

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		m_Channels[i]->Invalidate_TransformationMatrix(Bones, &m_CurrentKeyFrameIndices[i], m_CurrentAnimPosition);
	}

	if (m_pEventCaller != nullptr)
		m_pEventCaller->Invalidate_Event(pTransform, Bones, m_CurrentAnimPosition);

	return m_CurrentAnimPosition;
}

_float CAnimation::Invalidate_TransformationMatrix_Half(_bool isLoop, class CTransform* pTransform, const vector<class CBone*>& Bones, _float fTimeDelta)
{
	m_CurrentAnimPosition += m_fTickPerSecond * fTimeDelta;

	if (m_CurrentAnimPosition > (m_Duration / 2.f))
	{
		m_bFinished = TRUE;

		if (m_pEventCaller != nullptr)
			m_pEventCaller->Reset_Event();

		if (true == isLoop)
		{
			m_bFinished = FALSE;
			m_CurrentAnimPosition = 0.f;
		}
	}

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		m_Channels[i]->Invalidate_TransformationMatrix(Bones, &m_CurrentKeyFrameIndices[i], m_CurrentAnimPosition);
	}

	if (m_pEventCaller != nullptr)
		m_pEventCaller->Invalidate_Event(pTransform, Bones, m_CurrentAnimPosition);

	return m_CurrentAnimPosition;
}

HRESULT CAnimation::Initialize(const ANIM_DESC* pAnimDesc, const vector<class CBone*>& Bones)
{
	/* Animation Name */
	strcpy_s(m_szName, pAnimDesc->szName);

	/* Duration, TPS */
	m_Duration			= static_cast<_float>(pAnimDesc->dDuration);
	m_fTickPerSecond	= static_cast<_float>(pAnimDesc->dTicksPerSecond);

	/* Channel */
	m_iNumChannels		= pAnimDesc->iNumChannels;
	m_CurrentKeyFrameIndices.resize(m_iNumChannels);

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel* pChannel = CChannel::Create(&(pAnimDesc->pChannelDesc[i]), Bones);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

CAnimation* CAnimation::Create(const ANIM_DESC* pAnimDesc, const vector<class CBone*>& Bones)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pAnimDesc, Bones)))
	{
		MSG_BOX(TEXT("Failed to Create : CAnimation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation* CAnimation::Clone()
{
	return new CAnimation(*this);
}

void CAnimation::Free()
{
	__super::Free();

	m_CurrentKeyFrameIndices.clear();

	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);
	m_Channels.clear();

	Safe_Release(m_pEventCaller);
}
