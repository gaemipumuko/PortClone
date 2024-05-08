#include "Event_Manager.h"
#include "EventCaller.h"

#include "GameInstance.h"

CEvent_Manager::CEvent_Manager()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

vector<string> CEvent_Manager::Get_EventCallerTags()
{
	vector<string> EventDataTags;
	
	for (auto& iter : m_EventData)
		EventDataTags.push_back(ConvertWCToC(iter.first));
	
	return EventDataTags;
}

vector<string> CEvent_Manager::Get_EventSounds(const wstring& strModelTag, const string& strAnimationTag)
{
	auto& Events = m_EventData.find(strModelTag);
	if (Events == m_EventData.end())
		return vector<string> { };

	auto& Event = Events->second.find(strAnimationTag);
	if (Event == Events->second.end())
		return vector<string> { };

	return Event->second->Get_SoundEvents();
}

vector<string> CEvent_Manager::Get_EventParticles(const wstring& strModelTag, const string& strAnimationTag)
{
	auto& Events = m_EventData.find(strModelTag);
	if (Events == m_EventData.end())
		return vector<string> { };

	auto& Event = Events->second.find(strAnimationTag);
	if (Event == Events->second.end())
		return vector<string> { };

	return Event->second->Get_ParticleEvents();
}

vector<string> CEvent_Manager::Get_EventFunctions(const wstring& strModelTag, const string& strAnimationTag)
{
	auto& Events = m_EventData.find(strModelTag);
	if (Events == m_EventData.end())
		return vector<string> { };

	auto& Event = Events->second.find(strAnimationTag);
	if (Event == Events->second.end())
		return vector<string> { };

	return Event->second->Get_FunctionEvents();
}

HRESULT CEvent_Manager::Create_EventCaller(const wstring& strModelTag, CModel* pModelCom)
{
	auto& iter = m_EventData.find(strModelTag);
	if (iter != m_EventData.end())
		return E_FAIL;

	EVENT_ANIMATION Events = { };

	vector<string> strAnimationTags = pModelCom->Get_AnimationTags();
	for (auto& iter : strAnimationTags)
	{
		CEventCaller* pEventCaller = CEventCaller::Create();
		if (pEventCaller == nullptr)
			return E_FAIL;
	
		Events.emplace(iter, pEventCaller);
	}
	
	m_EventData.emplace(strModelTag, Events);

	return S_OK;
}

HRESULT CEvent_Manager::Delete_EventCaller(const wstring& strModelTag)
{
	auto& Events = m_EventData.find(strModelTag);
	if (Events == m_EventData.end())
		return E_FAIL;

	for (auto& Event : Events->second)
		Safe_Release(Event.second);

	m_EventData.erase(Events);

	return S_OK;
}

CEventCaller* CEvent_Manager::Clone_EventCaller(const wstring& strModelTag, const string& strAnimationTag)
{
	auto& Events = m_EventData.find(strModelTag);
	if (Events == m_EventData.end())
		return nullptr;

	auto& Event = Events->second.find(strAnimationTag);
	if (Event == Events->second.end())
		return nullptr;

	return Event->second->Clone();
}

HRESULT CEvent_Manager::Save_EventDataFile(const string& strEventDataFilePath)
{
	ofstream outFile(strEventDataFilePath.c_str(), ios::binary | ios::out);
	if (outFile.is_open() == FALSE) {
		return E_FAIL;
	}
	
	_int iStore = static_cast<_int>(m_EventData.size());
	outFile.write(reinterpret_cast<char*>(&iStore), sizeof _int);
	for (auto& Events : m_EventData)
	{
		char	szName[MAX_PATH];	strcpy_s(szName, ConvertWCToC(Events.first).c_str());
		outFile.write(reinterpret_cast<char*>(&szName), sizeof(char[MAX_PATH]));

		iStore = static_cast<_int>(Events.second.size());
		outFile.write(reinterpret_cast<char*>(&iStore), sizeof _int);
		for (auto& Event : Events.second)
		{
			char	szAnimation[MAX_PATH];	strcpy_s(szAnimation, Event.first.c_str());
			outFile.write(reinterpret_cast<char*>(&szAnimation), sizeof(char[MAX_PATH]));

			/* Sound Event */
			iStore = static_cast<_int>(Event.second->Get_SoundEvents().size());
			outFile.write(reinterpret_cast<char*>(&iStore), sizeof _int);

			for (auto& SoundEvent : Event.second->Get_SoundEvents())
			{
				char	szSoundEvent[MAX_PATH];	strcpy_s(szSoundEvent, SoundEvent.c_str());
				outFile.write(reinterpret_cast<char*>(&szSoundEvent), sizeof(char[MAX_PATH]));
			}

			/* Particle Event */
			iStore = static_cast<_int>(Event.second->Get_ParticleEvents().size());
			outFile.write(reinterpret_cast<char*>(&iStore), sizeof _int);

			for (auto& ParticleEvent : Event.second->Get_ParticleEvents())
			{
				char	szParticleEvent[MAX_PATH];	strcpy_s(szParticleEvent, ParticleEvent.c_str());
				outFile.write(reinterpret_cast<char*>(&szParticleEvent), sizeof(char[MAX_PATH]));
			}

			/* Function Event */
			iStore = static_cast<_int>(Event.second->Get_FunctionEvents().size());
			outFile.write(reinterpret_cast<char*>(&iStore), sizeof _int);

			for (auto& FunctionEvent : Event.second->Get_FunctionEvents())
			{
				char	szFunctionEvent[MAX_PATH];	strcpy_s(szFunctionEvent, FunctionEvent.c_str());
				outFile.write(reinterpret_cast<char*>(&szFunctionEvent), sizeof(char[MAX_PATH]));
			}
		}
	}
	
	outFile.close();

	return S_OK;
}

HRESULT CEvent_Manager::Load_EventDataFile(const string& strEventDataFilePath)
{
	for (auto& Events : m_EventData)
	{
		for (auto& Event : Events.second)
		{
			Safe_Release(Event.second);
		}
		Events.second.clear();
	}
	m_EventData.clear();

	ifstream inFile(strEventDataFilePath.c_str(), ios::binary | ios::in);
	if (inFile.is_open() == FALSE) {
		return E_FAIL;
	}
	
	_int iNumEventCaller = { };
	inFile.read(reinterpret_cast<char*>(&iNumEventCaller), sizeof _int);
	
	for (_int iEventCallerIdx = 0; iEventCallerIdx < iNumEventCaller; ++iEventCallerIdx)
	{
		char	szName[MAX_PATH];
		inFile.read(reinterpret_cast<char*>(&szName), sizeof(char[MAX_PATH]));
	
		_int iNumAnimation = { };
		inFile.read(reinterpret_cast<char*>(&iNumAnimation), sizeof _int);

		EVENT_ANIMATION Events = { };
		for (_int iAnim = 0; iAnim < iNumAnimation; ++iAnim)
		{
			char	szAnimation[MAX_PATH];
			inFile.read(reinterpret_cast<char*>(&szAnimation), sizeof(char[MAX_PATH]));

			CEventCaller* pEventCaller = CEventCaller::Create();
			if (pEventCaller == nullptr)
				return E_FAIL;

			/* Sound Event */
			_int iNumSoundEvent = { };
			inFile.read(reinterpret_cast<char*>(&iNumSoundEvent), sizeof _int);

			for (_int iSoundEvent = 0; iSoundEvent < iNumSoundEvent; ++iSoundEvent)
			{
				char	szSoundEvent[MAX_PATH];
				inFile.read(reinterpret_cast<char*>(&szSoundEvent), sizeof(char[MAX_PATH]));

				vector<string> SoundEventDesc = SplitBySeparator(szSoundEvent, ',');
				pEventCaller->Append_SoundEvent(ConvertCToWC(SoundEventDesc[1]), stof(SoundEventDesc[0]));
			}

			/* Particle Event */
			_int iNumParticleEvent = { };
			inFile.read(reinterpret_cast<char*>(&iNumParticleEvent), sizeof _int);

			for (_int iParticleEvent = 0; iParticleEvent < iNumParticleEvent; ++iParticleEvent)
			{
				char	szParticleEvent[MAX_PATH];
				inFile.read(reinterpret_cast<char*>(&szParticleEvent), sizeof(char[MAX_PATH]));

				vector<string> ParticleEventDesc = SplitBySeparator(szParticleEvent, ',');
				pEventCaller->Append_ParticleEvent(stoi(ParticleEventDesc[1]), stoi(ParticleEventDesc[2]), 
					ConvertCToWC(ParticleEventDesc[3]), ParticleEventDesc[4], stof(ParticleEventDesc[5]), stof(ParticleEventDesc[0]));
			}

			/* Function Event */
			_int iNumFunctionEvent = { };
			inFile.read(reinterpret_cast<char*>(&iNumFunctionEvent), sizeof _int);

			for (_int iFunctionEvent = 0; iFunctionEvent < iNumFunctionEvent; ++iFunctionEvent)
			{
				char	szFunctionEvent[MAX_PATH];
				inFile.read(reinterpret_cast<char*>(&szFunctionEvent), sizeof(char[MAX_PATH]));

				vector<string> FunctionEventDesc = SplitBySeparator(szFunctionEvent, ',');
				pEventCaller->Append_FunctionEvent(ConvertCToWC(FunctionEventDesc[1]), stof(FunctionEventDesc[0]));
			}

			Events.emplace(szAnimation, pEventCaller);
		}

		m_EventData.emplace(ConvertCToWC(szName), Events);
	}
	
	inFile.close();

	return S_OK;
}

HRESULT CEvent_Manager::Append_SoundEvent(const wstring& strModelTag, const string& strAnimationTag, const wstring& strSoundTag, _float fEventFrame)
{
	auto& Events = m_EventData.find(strModelTag);
	if (Events == m_EventData.end())
		return E_FAIL;
	
	auto& Event = Events->second.find(strAnimationTag);
	if (Event == Events->second.end())
		return E_FAIL;
	
	if (FAILED(Event->second->Append_SoundEvent(strSoundTag, fEventFrame)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEvent_Manager::Append_EffectEvent(const wstring& strModelTag, const string& strAnimationTag, _float fEventFrame)
{
	return S_OK;
}

HRESULT CEvent_Manager::Append_ParticleEvent(const wstring& strModelTag, const string& strAnimationTag, _int iShaderPass, _int iParticleIndex, const wstring& strParticleTag, const string& strBoneTag, _float fDuration, _float fEventFrame)
{
	auto& Events = m_EventData.find(strModelTag);
	if (Events == m_EventData.end())
		return E_FAIL;

	auto& Event = Events->second.find(strAnimationTag);
	if (Event == Events->second.end())
		return E_FAIL;

	if (FAILED(Event->second->Append_ParticleEvent(iShaderPass, iParticleIndex, strParticleTag, strBoneTag, fDuration, fEventFrame)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEvent_Manager::Append_FunctionEvent(const wstring& strModelTag, const string& strAnimationTag, const wstring& strFunctionTag, _float fEventFrame)
{
	auto& Events = m_EventData.find(strModelTag);
	if (Events == m_EventData.end())
		return E_FAIL;

	auto& Event = Events->second.find(strAnimationTag);
	if (Event == Events->second.end())
		return E_FAIL;

	if (FAILED(Event->second->Append_FunctionEvent(strFunctionTag, fEventFrame)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEvent_Manager::Append_TranslationEvent(const wstring& strModelTag, const string& strAnimationTag, _float fEventFrame)
{
	return S_OK;
}

HRESULT CEvent_Manager::Delete_SoundEvent(const wstring& strModelTag, const string& strAnimationTag, _uint iIndex)
{
	auto& Events = m_EventData.find(strModelTag);
	if (Events == m_EventData.end())
		return E_FAIL;

	auto& Event = Events->second.find(strAnimationTag);
	if (Event == Events->second.end())
		return E_FAIL;

	if (FAILED(Event->second->Delete_SoundEvent(iIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEvent_Manager::Delete_FunctionEvent(const wstring& strModelTag, const string& strAnimationTag, _uint iIndex)
{
	auto& Events = m_EventData.find(strModelTag);
	if (Events == m_EventData.end())
		return E_FAIL;

	auto& Event = Events->second.find(strAnimationTag);
	if (Event == Events->second.end())
		return E_FAIL;

	if (FAILED(Event->second->Delete_FunctionEvent(iIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEvent_Manager::Initialize()
{
	Load_EventDataFile("../../Reference/Resources/Data/Event/Event.dat");

	return S_OK;
}

CEvent_Manager* CEvent_Manager::Create()
{
	CEvent_Manager* pInstance = new CEvent_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CEvent_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Manager::Free()
{
	__super::Free();

	for (auto& Events : m_EventData)
	{
		for (auto& Event : Events.second)
		{
			Safe_Release(Event.second);
		}
		Events.second.clear();
	}
	m_EventData.clear();

	Safe_Release(m_pGameInstance);
}
