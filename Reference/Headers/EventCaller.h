#pragma once

#include "Base.h"

BEGIN(Engine)

class CEventCaller final : public CBase
{
public:
	typedef struct tagSoundEvent
	{
		_bool	bPlayEvent;
		_float	fEventFrame;

		wstring strSoundTag;

		bool operator < (const tagSoundEvent& t1)
		{
			return this->fEventFrame < t1.fEventFrame;
		}
	} SOUND_EVENT;

	typedef struct tagEffectEvent
	{
	} EFFECT_EVENT;

	typedef struct tagParticleEvent
	{
		_bool	bPlayEvent;
		_float	fEventFrame;

		_int				iShaderPass;
		_int				iParticleIndex;

		wstring				strParticleTag;
		string				strBoneTag;
		_float				fDuration;

		bool operator < (const tagParticleEvent& t1)
		{
			return this->fEventFrame < t1.fEventFrame;
		}
	} PARTICLE_EVENT;

	typedef struct tagFunctionEvent
	{
		_bool	bPlayEvent;
		_float	fEventFrame;

		wstring				strFunctionTag;

		bool operator < (const tagFunctionEvent& t1)
		{
			return this->fEventFrame < t1.fEventFrame;
		}
	} FUNCTION_EVENT;

private:
	CEventCaller();
	CEventCaller(const CEventCaller& rhs);
	virtual ~CEventCaller() = default;

private:
	class CGameInstance*	m_pGameInstance = { nullptr };

private: /* For.Particle ID */
	static _ulong			m_UniqueCallerID;

	_ulong					m_ulCallerID = { };
	_ulong					m_ulEventID = { };

private: /* For.Function */
	_bool					m_bFunction = { FALSE };

private:
	vector<SOUND_EVENT>		m_SoundEvents;
	vector<EFFECT_EVENT>	m_EffectEvents;
	vector<PARTICLE_EVENT>	m_ParticleEvents;
	vector<FUNCTION_EVENT>	m_FunctionEvents;

public:
	vector<string>			Get_SoundEvents();
	vector<string>			Get_EffectEvents();
	vector<string>			Get_ParticleEvents();
	vector<string>			Get_FunctionEvents();

public:
	HRESULT	Append_SoundEvent(const wstring& strSoundTag, _float fEventFrame);
	HRESULT	Append_EffectEvent(wstring strSoundTag, _float fEventFrame);
	HRESULT	Append_ParticleEvent(_int iShaderPass, _int iParticleIndex, const wstring& strParticleTag, const string& strBoneTag, _float fDuration, _float fEventFrame);
	HRESULT	Append_FunctionEvent(const wstring& strFunctionTag, _float fEventFrame);
	HRESULT	Append_TranslationEvent(wstring strSoundTag, _float fEventFrame);

public:
	HRESULT					Delete_SoundEvent(_uint iIndex);
	HRESULT					Delete_FunctionEvent(_uint iIndex);

public:
	HRESULT	Reset_Event();
	HRESULT	Invalidate_Event(class CTransform* pTransform, const vector<class CBone*>& Bones, _float fCurrDuration);

public:
	_bool isFunctionTrue();

public:
	HRESULT					Initialize();

public:
	static CEventCaller*	Create();
	CEventCaller*			Clone();
	virtual void			Free() override;
};

END