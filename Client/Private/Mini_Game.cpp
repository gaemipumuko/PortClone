#include "stdafx.h"
#include "Mini_Game.h"

#include "GameInstance.h"

CMini_Game::CMini_Game()
{
}

CMini_Game::CMini_Game(const CMini_Game& rhs)
{
}

HRESULT CMini_Game::Initialize(void* pArg)
{
	if (pArg != nullptr)
	{
		MINIGAME_DESC* tMiniGame = static_cast<MINIGAME_DESC*>(pArg);

		m_strMiniGameName = tMiniGame->strMiniGameName;
		m_fTotalTime = tMiniGame->fTotalTime;
	}

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	pGameInstance->Register_Interaction(m_strMiniGameName);

	Safe_Release(pGameInstance);

	return S_OK;
}

void CMini_Game::Free()
{
}
