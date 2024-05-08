#include "EventCaller.h"

#include "Bone.h"
#include "GameInstance.h"

_ulong CEventCaller::m_UniqueCallerID = 0;

CEventCaller::CEventCaller()
{
}

CEventCaller::CEventCaller(const CEventCaller& rhs)
	: m_pGameInstance(CGameInstance::Get_Instance())
	, m_ulCallerID(m_UniqueCallerID++)
	, m_SoundEvents(rhs.m_SoundEvents)
	, m_EffectEvents(rhs.m_EffectEvents)
	, m_ParticleEvents(rhs.m_ParticleEvents)
	, m_FunctionEvents(rhs.m_FunctionEvents)
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CEventCaller::Append_SoundEvent(const wstring& strSoundTag, _float fEventFrame)
{
	SOUND_EVENT eEvent = { };
	eEvent.fEventFrame = fEventFrame;
	eEvent.strSoundTag = strSoundTag;

	m_SoundEvents.push_back(eEvent);
	sort(m_SoundEvents.begin(), m_SoundEvents.end());

	return S_OK;
}

HRESULT CEventCaller::Append_EffectEvent(wstring strSoundTag, _float fEventFrame)
{
	return S_OK;
}

HRESULT CEventCaller::Append_ParticleEvent(_int iShaderPass, _int iParticleIndex, const wstring& strParticleTag, const string& strBoneTag, _float fDuration, _float fEventFrame)
{
	PARTICLE_EVENT eEvent = { };
	eEvent.fEventFrame = fEventFrame;

	eEvent.iShaderPass = iShaderPass;
	eEvent.iParticleIndex = iParticleIndex;
	eEvent.strParticleTag = strParticleTag;
	eEvent.strBoneTag = strBoneTag;
	eEvent.fDuration = fDuration;

	m_ParticleEvents.push_back(eEvent);
	sort(m_ParticleEvents.begin(), m_ParticleEvents.end());

	return S_OK;
}

HRESULT CEventCaller::Append_FunctionEvent(const wstring& strFunctionTag, _float fEventFrame)
{
	FUNCTION_EVENT eEvent = { };
	eEvent.fEventFrame = fEventFrame;
	eEvent.strFunctionTag = strFunctionTag;

	m_FunctionEvents.push_back(eEvent);
	sort(m_FunctionEvents.begin(), m_FunctionEvents.end());

	return S_OK;
}

HRESULT CEventCaller::Append_TranslationEvent(wstring strSoundTag, _float fEventFrame)
{
	return S_OK;
}

HRESULT CEventCaller::Delete_SoundEvent(_uint iIndex)
{
	if (m_SoundEvents.size() - 1 < iIndex || m_SoundEvents.empty() == TRUE)
		return E_FAIL;

	m_SoundEvents.erase(m_SoundEvents.begin() + iIndex);

	return S_OK;
}

HRESULT CEventCaller::Delete_FunctionEvent(_uint iIndex)
{
	m_FunctionEvents.erase(m_FunctionEvents.begin() + iIndex);

	return S_OK;
}

HRESULT CEventCaller::Reset_Event()
{
	for (auto& iter : m_SoundEvents)
		iter.bPlayEvent = FALSE;

	for (auto& iter : m_ParticleEvents)
		iter.bPlayEvent = FALSE;

	for (auto& iter : m_FunctionEvents)
		iter.bPlayEvent = FALSE;

	m_bFunction = FALSE;

	return S_OK;
}

HRESULT CEventCaller::Invalidate_Event(class CTransform* pTransform, const vector<class CBone*>& Bones, _float fCurrDuration)
{
	for (auto& iter : m_SoundEvents)
	{
		if (iter.fEventFrame > fCurrDuration)
			break;

		if (iter.bPlayEvent == FALSE && iter.fEventFrame < fCurrDuration)
		{
			iter.bPlayEvent = TRUE;
			m_pGameInstance->PlaySoundW(iter.strSoundTag, 1.f);
		}
	}

	for (auto& iter : m_ParticleEvents)
	{
		if (iter.fEventFrame > fCurrDuration)
			break;

		if (iter.bPlayEvent == FALSE && iter.fEventFrame < fCurrDuration)
		{
			iter.bPlayEvent = TRUE;

			for (auto& Bone : Bones)
			{
				if (!strcmp(Bone->Get_BoneName(), iter.strBoneTag.c_str()))
				{
					wstring strParticleTag = iter.strParticleTag + L"_" + to_wstring(m_ulCallerID) + L"_" + to_wstring(m_ulEventID++);
					m_pGameInstance->Play_Particle(iter.strParticleTag, strParticleTag, iter.iShaderPass, iter.iParticleIndex, Bone->Get_CombinedMatrix(), pTransform, iter.fDuration);
					break;
				}
			}
		}
	}

	for (auto& iter : m_FunctionEvents)
	{
		if (iter.fEventFrame > fCurrDuration)
			break;

		if (iter.bPlayEvent == FALSE && iter.fEventFrame < fCurrDuration)
		{
			iter.bPlayEvent = TRUE;
			m_bFunction = TRUE;
			//m_pGameInstance->PlaySoundW(iter.strSoundTag, 1.f);
		}
	}

	return S_OK;
}

_bool CEventCaller::isFunctionTrue()
{
	_bool bFunction = m_bFunction;
	m_bFunction = FALSE;

	return bFunction;
}

vector<string> CEventCaller::Get_SoundEvents()
{
	vector<string> SoundEvents;

	for (auto& iter : m_SoundEvents)
	{
		string strSoundEvent = 
			to_string(iter.fEventFrame).substr(0, to_wstring(iter.fEventFrame).find('.') + 3);
		strSoundEvent += ",";
		strSoundEvent += ConvertWCToC(iter.strSoundTag);

		SoundEvents.push_back(strSoundEvent);
	}

	return SoundEvents;
}

vector<string> CEventCaller::Get_EffectEvents()
{
	return vector<string>();
}

vector<string> CEventCaller::Get_ParticleEvents()
{
	vector<string> ParticleEvents;

	for (auto& iter : m_ParticleEvents)
	{
		string strParticleEvent =
			to_string(iter.fEventFrame).substr(0, to_wstring(iter.fEventFrame).find('.') + 3);
		strParticleEvent += ",";
		strParticleEvent += to_string(iter.iShaderPass);
		strParticleEvent += ",";
		strParticleEvent += to_string(iter.iParticleIndex);
		strParticleEvent += ",";
		strParticleEvent += ConvertWCToC(iter.strParticleTag);
		strParticleEvent += ",";
		strParticleEvent += iter.strBoneTag;
		strParticleEvent += ",";
		strParticleEvent += to_string(iter.fDuration).substr(0, to_wstring(iter.fDuration).find('.') + 3);

		ParticleEvents.push_back(strParticleEvent);
	}

	return ParticleEvents;
}

vector<string> CEventCaller::Get_FunctionEvents()
{
	vector<string> FunctionEvents;

	for (auto& iter : m_FunctionEvents)
	{
		string strSoundEvent =
			to_string(iter.fEventFrame).substr(0, to_wstring(iter.fEventFrame).find('.') + 3);
		strSoundEvent += ",";
		strSoundEvent += ConvertWCToC(iter.strFunctionTag);

		FunctionEvents.push_back(strSoundEvent);
	}

	return FunctionEvents;
}

HRESULT CEventCaller::Initialize()
{
	return S_OK;
}

CEventCaller* CEventCaller::Create()
{
	CEventCaller* pInstance = new CEventCaller();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CEventCaller"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CEventCaller* CEventCaller::Clone()
{
	return new CEventCaller(*this);
}

void CEventCaller::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
