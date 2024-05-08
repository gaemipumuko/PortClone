#include "Level_Loading.h"
#include "Loader.h"
#include "Level_Edit.h"
#include "GameInstance.h"

#define CURRENT_LEVEL LEVEL_LOADING

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
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

	return S_OK;
}

HRESULT CLevel_Loading::LateTick(_float fTimeDelta)
{
	if (FAILED(__super::LateTick(fTimeDelta)))
		return E_FAIL;

	if (m_pLoader->Get_Finished())
	{
		CLevel* pLevel = { nullptr };

		switch (m_eNextLevelID)
		{
		case LEVEL_EDIT:
			pLevel = CLevel_Edit::Create(m_pDevice, m_pContext);
			break;
		default:
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

HRESULT CLevel_Loading::Ready_Level()
{
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
