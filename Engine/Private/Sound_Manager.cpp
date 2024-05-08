#include "Sound_Manager.h"

CSound_Manager::CSound_Manager()
	: m_pChannelArr{ nullptr }
{
}

vector<string> CSound_Manager::Get_SoundTags()
{
	vector<string> SoundTags;

	for (auto& iter : m_mapSound)
		SoundTags.push_back(ConvertWCToC(iter.first));

	return SoundTags;
}

void CSound_Manager::LoadSoundFile(const wstring& strSoundFilePath)
{
	HANDLE hFind;
	WIN32_FIND_DATA tagFindData;

	_bool bResult = { TRUE };

	_tchar	strDir[MAX_PATH];
	wstring strNewSoundFilePath;
	wstring strFullPath;

	hFind = FindFirstFile(strSoundFilePath.c_str(), &tagFindData);

	if (hFind == INVALID_HANDLE_VALUE)
		return;

	_wsplitpath_s(strSoundFilePath.c_str(), NULL, NULL, strDir, MAX_PATH, NULL, NULL, NULL, NULL);

	while (bResult)
	{
		if (tagFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (lstrcmp(tagFindData.cFileName, L".") && lstrcmp(tagFindData.cFileName, L".."))
			{
				strNewSoundFilePath = L"";
				strNewSoundFilePath += strDir;
				strNewSoundFilePath += tagFindData.cFileName;
				strNewSoundFilePath += L"/*.*";

				LoadSoundFile(strNewSoundFilePath);
			}
		}
		else
		{
			strFullPath = L"";
			strFullPath += strDir;
			strFullPath += tagFindData.cFileName;

			FMOD_SOUND* pSound = nullptr;
			_char		szSoundKey[MAX_PATH];

			wcstombs_s(nullptr, szSoundKey, MAX_PATH, strFullPath.c_str(), _TRUNCATE);
			FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, szSoundKey, FMOD_DEFAULT, 0, &pSound);

			if (eRes == FMOD_OK)
			{
				wstring strSoundKey = tagFindData.cFileName;

				m_mapSound.emplace(strSoundKey, pSound);
			}
		}

		bResult = FindNextFile(hFind, &tagFindData);
	}

	FindClose(hFind);

	FMOD_System_Update(m_pSystem);
}

void CSound_Manager::PlayBGM(wstring strSoundKey, float fVolume)
{
	FMOD_Channel_Stop(m_pChannelArr[SOUND_BGM]);

	auto& iter = m_mapSound.find(strSoundKey);
	if (iter == m_mapSound.end())
		return;

	FMOD_System_PlaySound(m_pSystem, iter->second, 0, FALSE, &m_pChannelArr[SOUND_BGM]);

	FMOD_Channel_SetMode(m_pChannelArr[SOUND_BGM], FMOD_LOOP_NORMAL);

	FMOD_Channel_SetVolume(m_pChannelArr[SOUND_BGM], fVolume);
	FMOD_System_Update(m_pSystem);
}

void CSound_Manager::PlayBGM(wstring strSoundKey, float fVolume, CHANNELID eChannel)
{
	FMOD_Channel_Stop(m_pChannelArr[eChannel]);

	auto& iter = m_mapSound.find(strSoundKey);
	if (iter == m_mapSound.end())
		return;

	FMOD_System_PlaySound(m_pSystem, iter->second, 0, FALSE, &m_pChannelArr[eChannel]);

	FMOD_Channel_SetMode(m_pChannelArr[eChannel], FMOD_LOOP_NORMAL);

	FMOD_Channel_SetVolume(m_pChannelArr[eChannel], fVolume);
	FMOD_System_Update(m_pSystem);
}

CHANNELID CSound_Manager::PlaySoundW(wstring strSoundKey, float fVolume)
{
	auto& iter = m_mapSound.find(strSoundKey);
	if (iter == m_mapSound.end())
		return CHANNELID::MAXCHANNEL;

	FMOD_BOOL bPlay = FALSE;
	CHANNELID eNextID = CHANNELID::MAXCHANNEL;

	_int iMaxSearchCnt = static_cast<_int>(CHANNELID::MAXCHANNEL);
	_int iCurrSearchCnt = 0;
	do
	{
		++iCurrSearchCnt;
		if (iCurrSearchCnt > iMaxSearchCnt)
			break;

		eNextID = Get_NextChannel();
		FMOD_Channel_IsPlaying(m_pChannelArr[eNextID], &bPlay);
	} while (bPlay == FMOD_ERR_BADCOMMAND);

	if (iMaxSearchCnt > iCurrSearchCnt && eNextID != CHANNELID::MAXCHANNEL)
	{
		FMOD_System_PlaySound(m_pSystem, iter->second, 0, FALSE, &m_pChannelArr[eNextID]);
		FMOD_Channel_SetVolume(m_pChannelArr[eNextID], fVolume);
	}

	FMOD_System_Update(m_pSystem);

	return eNextID;
}

void CSound_Manager::PlaySoundW(wstring strSoundKey, float fVolume, CHANNELID eChannel)
{
	auto& iter = m_mapSound.find(strSoundKey);
	if (iter == m_mapSound.end())
		return;

	FMOD_BOOL bPlay = FALSE;
	CHANNELID eNextID = eChannel;

	_int iMaxSearchCnt = static_cast<_int>(CHANNELID::MAXCHANNEL);
	_int iCurrSearchCnt = 0;
	if (eNextID == CHANNELID::MAXCHANNEL)
	{
		do
		{
			++iCurrSearchCnt;
			if (iCurrSearchCnt > iMaxSearchCnt)
				break;

			eNextID = Get_NextChannel();
			FMOD_Channel_IsPlaying(m_pChannelArr[eNextID], &bPlay);
		} while (bPlay == FMOD_ERR_BADCOMMAND);
	}

	if (iMaxSearchCnt > iCurrSearchCnt && eNextID != CHANNELID::MAXCHANNEL)
	{
		FMOD_System_PlaySound(m_pSystem, iter->second, 0, FALSE, &m_pChannelArr[eNextID]);
		FMOD_Channel_SetVolume(m_pChannelArr[eNextID], fVolume);
	}

	FMOD_System_Update(m_pSystem);
}

void CSound_Manager::StopAll()
{
	for (_uint idx = 0; idx < MAXCHANNEL; ++idx)
		FMOD_Channel_Stop(m_pChannelArr[idx]);
}

void CSound_Manager::StopSound(CHANNELID eID)
{
	FMOD_Channel_Stop(m_pChannelArr[eID]);
}

void CSound_Manager::SetChannelVolume(CHANNELID eID, float fVolume)
{
	FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);

	FMOD_System_Update(m_pSystem);
}

HRESULT CSound_Manager::Initialize()
{
	FMOD_System_Create(&m_pSystem, 0x00020217);
	FMOD_System_Init(m_pSystem, 32, FMOD_INIT_NORMAL, NULL);

	LoadSoundFile(L"../../Reference/Resources/Sound/*.*");

	return S_OK;
}

CSound_Manager* CSound_Manager::Create()
{
	CSound_Manager* pInstance = new CSound_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CSound_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSound_Manager::Free()
{
	__super::Free();

	for (auto& Mypair : m_mapSound)
	{
		FMOD_Sound_Release(Mypair.second);
	}
	m_mapSound.clear();

	FMOD_System_Release(m_pSystem);
	FMOD_System_Close(m_pSystem);
}
