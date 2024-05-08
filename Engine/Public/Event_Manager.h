#pragma once

#include "Base.h"

BEGIN(Engine)
class CModel;
class CEventCaller;
class CGameInstance;
END

BEGIN(Engine)

class CEvent_Manager final : public CBase
{
public:
	typedef map<string, CEventCaller*>	EVENT_ANIMATION;

private:
	CEvent_Manager();
	virtual ~CEvent_Manager() = default;

private:
	CGameInstance*	m_pGameInstance = { };

private:
	map<wstring, EVENT_ANIMATION>	m_EventData;

public:
	vector<string>	Get_EventCallerTags();
	vector<string>	Get_EventSounds(const wstring& strModelTag, const string& strAnimationTag);
	vector<string>	Get_EventParticles(const wstring& strModelTag, const string& strAnimationTag);
	vector<string>	Get_EventFunctions(const wstring& strModelTag, const string& strAnimationTag);

public:
	HRESULT	Create_EventCaller(const wstring& strModelTag, CModel* pModelCom);
	HRESULT	Delete_EventCaller(const wstring& strModelTag);

	CEventCaller*	Clone_EventCaller(const wstring& strModelTag, const string& strAnimationTag);

public:
	HRESULT	Save_EventDataFile(const string& strEventDataFilePath);
	HRESULT	Load_EventDataFile(const string& strEventDataFilePath);

public:
	HRESULT	Append_SoundEvent(const wstring& strModelTag, const string& strAnimationTag, 
		const wstring& strSoundTag, _float fEventFrame = 0.f);
	HRESULT	Append_EffectEvent(const wstring& strModelTag, const string& strAnimationTag, _float fEventFrame = 0.f);
	HRESULT	Append_ParticleEvent(const wstring& strModelTag, const string& strAnimationTag, 
		_int iShaderPass, _int iParticleIndex, const wstring& strParticleTag, const string& strBoneTag, _float fDuration, _float fEventFrame = 0.f);
	HRESULT	Append_FunctionEvent(const wstring& strModelTag, const string& strAnimationTag,
		const wstring& strFunctionTag, _float fEventFrame = 0.f);
	HRESULT	Append_TranslationEvent(const wstring& strModelTag, const string& strAnimationTag, _float fEventFrame = 0.f);

	HRESULT Delete_SoundEvent(const wstring& strModelTag, const string& strAnimationTag, _uint iIndex);
	HRESULT Delete_FunctionEvent(const wstring& strModelTag, const string& strAnimationTag, _uint iIndex);

public:
	HRESULT Initialize();

public:
	static CEvent_Manager*	Create();
	virtual void			Free();
};

END