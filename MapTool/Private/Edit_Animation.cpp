#include "Edit_Animation.h"
#include "Edit_Channel.h"

CEdit_Animation::CEdit_Animation()
{
}

CEdit_Animation::CEdit_Animation(const CEdit_Animation& rhs)
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

CEdit_Channel* CEdit_Animation::Find_Channel(_uint iBoneIdx)
{
	for (auto& Channel : m_Channels)
	{
		if (Channel->Get_BoneIndex() == iBoneIdx)
			return Channel;
	}

	return nullptr;
}

_float CEdit_Animation::Invalidate_TransformationMatrix(_bool isLoop, const vector<class CEdit_Bone*>& Bones, _float fTimeDelta)
{
	m_CurrentAnimPosition += m_fTickPerSecond * fTimeDelta;

	if (m_CurrentAnimPosition >= m_Duration)
	{
		m_bFinished = TRUE;

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

	return m_CurrentAnimPosition;
}

_float CEdit_Animation::Invalidate_TransformationMatrix_Half(_bool isLoop, const vector<class CEdit_Bone*>& Bones, _float fTimeDelta)
{
	m_CurrentAnimPosition += m_fTickPerSecond * fTimeDelta;

	if (m_CurrentAnimPosition > (m_Duration / 2.f))
	{
		m_bFinished = TRUE;

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

	return m_CurrentAnimPosition;
}

HRESULT CEdit_Animation::Initialize(const ANIM_DESC* pAnimDesc, const vector<class CEdit_Bone*>& Bones)
{
	/* Animation Name */
	strcpy_s(m_szName, pAnimDesc->szName);

	/* Duration, TPS */
	m_Duration = static_cast<_float>(pAnimDesc->dDuration);
	m_fTickPerSecond = static_cast<_float>(pAnimDesc->dTicksPerSecond);

	/* Channel */
	m_iNumChannels = pAnimDesc->iNumChannels;
	m_CurrentKeyFrameIndices.resize(m_iNumChannels);

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CEdit_Channel* pChannel = CEdit_Channel::Create(&(pAnimDesc->pChannelDesc[i]), Bones);
		
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

CEdit_Animation* CEdit_Animation::Create(const ANIM_DESC* pAnimDesc, const vector<class CEdit_Bone*>& Bones)
{
	CEdit_Animation* pInstance = new CEdit_Animation();

	if (FAILED(pInstance->Initialize(pAnimDesc, Bones)))
	{
		MSG_BOX(TEXT("Failed to Create : CEdit_Animation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CEdit_Animation* CEdit_Animation::Clone()
{
	return new CEdit_Animation(*this);
}

void CEdit_Animation::Free()
{
	__super::Free();

	m_CurrentKeyFrameIndices.clear();

	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}
