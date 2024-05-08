#pragma once

#include "Base.h"

BEGIN(Engine)

class CSound_Manager final : public CBase
{
public:
	CSound_Manager();
	virtual ~CSound_Manager() = default;

private:
	map<wstring, FMOD_SOUND*> m_mapSound;		// 사운드 리소스 정보를 갖는 객체 

	FMOD_CHANNEL*	m_pChannelArr[MAXCHANNEL];	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD_SYSTEM*	m_pSystem = { nullptr };	// 사운드 ,채널 객체 및 장치를 관리하는 객체 

public:
	vector<string>	Get_SoundTags();

private:
	CHANNELID m_eChannel = { CHANNELID::MAXCHANNEL };

public:
	CHANNELID Get_NextChannel()
	{
		m_eChannel = CHANNELID(m_eChannel + 1);
		if (m_eChannel > CHANNELID::SOUND_CHMAX)
			m_eChannel = CHANNELID::SOUND_CH0;

		return m_eChannel;
	}

private:
	void		LoadSoundFile(const wstring& strSoundFilePath);

public:
	void		PlayBGM(wstring strSoundKey, float fVolume);
	void		PlayBGM(wstring strSoundKey, float fVolume, CHANNELID eChannel);

	CHANNELID	PlaySoundW(wstring strSoundKey, float fVolume);
	void		PlaySoundW(wstring strSoundKey, float fVolume, CHANNELID eChannel);

	void StopAll();
	void StopSound(CHANNELID eID);

	void SetChannelVolume(CHANNELID eID, float fVolume);

public:
	HRESULT Initialize();

public:
	static CSound_Manager*	Create();
	virtual void			Free() override;
};

END