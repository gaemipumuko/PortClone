#include "stdafx.h"
#include "Loader.h"
#include "Level_World.h"
#include "Level_Temple_Lion.h"
#include "Level_StrongHold_AbyssMage.h"
#include "Level_Quest.h"
#include "Level_StormTerror.h"
#include "Level_Loading.h"

#include "LoadingRect.h"
#include "GameInstance.h"

#define CURRENT_LEVEL LEVEL_LOADING

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Loading::Ready_Level()
{
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	m_pGameInstance->StopSound(CHANNELID::SOUND_ANNOUNCE);
	m_pGameInstance->PlaySoundW(L"Loading.mp3", 1.f, CHANNELID::SOUND_ANNOUNCE);

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_BackGround(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_LoadingRect"), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Loading::Initialize(LEVELID eLevel)
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Level()))
		return E_FAIL;

	m_eNextLevelID = eLevel;

	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eLevel);
	if (m_pLoader == nullptr)
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Loading::PreviousTick(_float fTimeDelta)
{
	if (FAILED(__super::PreviousTick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Loading::Tick(_float fTimeDelta)
{
	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;

	CLoadingRect* pLoadingRect = static_cast<CLoadingRect*>(m_pGameInstance->Find_Object(CURRENT_LEVEL, L"Layer_BackGround"));
	pLoadingRect->Setup_LoadRatio(m_pLoader->Get_LoadRatio());

	return S_OK;
}

HRESULT CLevel_Loading::LateTick(_float fTimeDelta)
{
	if (FAILED(__super::LateTick(fTimeDelta)))
		return E_FAIL;

	CLoadingRect* pLoadingRect = static_cast<CLoadingRect*>(m_pGameInstance->Find_Object(CURRENT_LEVEL, L"Layer_BackGround"));

	if (pLoadingRect->Is_Finished())
	{
		CLevel* pLevel = { nullptr };

		switch (m_eNextLevelID)
		{
		case LEVEL_WORLD:
			pLevel = CLevel_World::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_TEMPLE_LION:
			pLevel = CLevel_Temple_Lion::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_STRONGHOLD_ABYSSMAGE:
			pLevel = CLevel_StrongHold_AbyssMage::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_QUEST:
			pLevel = CLevel_Quest::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_STORMTERROR:
			pLevel = CLevel_StormTerror::Create(m_pDevice, m_pContext);
			break;
		}

		if (pLevel == nullptr)
		{
			MSG_BOX(TEXT("Failed to Create : NextLevel"));
			return E_FAIL;
		}

		if (FAILED(m_pGameInstance->Open_Level(m_eNextLevelID, pLevel)))
		{
			MSG_BOX(TEXT("Failed to Open Level : CLevel_Loading"));
			return E_FAIL;
		}

		return S_OK;
	}

	return S_OK;
}

HRESULT CLevel_Loading::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CLevel_Loading* CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eLevel)
{
	CLevel_Loading* pLevel = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pLevel->Initialize(eLevel)))
	{
		MSG_BOX(TEXT("Failed to Create : CLevel_Loading"));
		Safe_Release(pLevel);
	}

	return pLevel;
}

void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
}
