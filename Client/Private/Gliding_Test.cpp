#include "stdafx.h"
#include "Gliding_Test.h"

#include "GameInstance.h"
#include "Player.h"
#include "UI_Manager.h"
#include "Page_Main.h"

CGliding_Test::CGliding_Test()
{
}

CGliding_Test::CGliding_Test(const CGliding_Test& rhs)
{
}

HRESULT CGliding_Test::Create_Updraft()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	_int iIndex = 0;
	CUpdraft::UPDRAFT_DESC tUpdraftDesc = {};
	tUpdraftDesc.vPosition = _float4(195.f, 50.f, 256.f, 1.f);
	tUpdraftDesc.vColliderSize = _float3(12.f, 12.f, 4.f);
	if (FAILED(pGameInstance->Add_Object_Layer(LEVEL_WORLD, TEXT("Prototype_GameObject_Updraft"), L"Layer_Updraft", &tUpdraftDesc)))
		return E_FAIL;
	iIndex++;
	tUpdraftDesc.vPosition = _float4(210.f, 50.f, 264.f, 1.f);
	tUpdraftDesc.vColliderSize = _float3(6.f, 12.f, 4.f);
	if (FAILED(pGameInstance->Add_Object_Layer(LEVEL_WORLD, TEXT("Prototype_GameObject_Updraft"), L"Layer_Updraft", &tUpdraftDesc)))
		return E_FAIL;
	iIndex++;
	tUpdraftDesc.vPosition = _float4(241.5f, 50.f, 264.f, 1.f);
	tUpdraftDesc.vColliderSize = _float3(4.f, 10.f, 3.f);
	if (FAILED(pGameInstance->Add_Object_Layer(LEVEL_WORLD, TEXT("Prototype_GameObject_Updraft"), L"Layer_Updraft", &tUpdraftDesc)))
		return E_FAIL;
	iIndex++;
	tUpdraftDesc.vPosition = _float4(246.f, 50.f, 236.f, 1.f);
	tUpdraftDesc.vColliderSize = _float3(4.f, 15.f, 6.f);
	if (FAILED(pGameInstance->Add_Object_Layer(LEVEL_WORLD, TEXT("Prototype_GameObject_Updraft"), L"Layer_Updraft", &tUpdraftDesc)))
		return E_FAIL;
	iIndex++;

	for (size_t i = 0; i < iIndex; i++)
	{
		UpDraft.push_back(static_cast<CUpdraft*>(pGameInstance->Find_Object(pGameInstance->Get_CurrentLevel(), L"Layer_Updraft", static_cast<_uint>(i))));
	}

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CGliding_Test::Create_Ring()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	
	_int iIndex = 0;
	CGliding_Ring::GLIDINGRING_DESC GlidingDesc;
	
	/* 벤티석상 머리 뒤 3개 */
	GlidingDesc.vPos = { 214.f, 71.f, 256.5f, 1.f };
	GlidingDesc.fRadian = 100.f;
	if (FAILED(pGameInstance->Add_Object_Layer(pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Gliding_Ring"), L"Layer_GlidingRing", &GlidingDesc)))
		return E_FAIL;
	iIndex++;

	GlidingDesc.vPos = { 216.f, 71.5f, 256.7f, 1.f };
	GlidingDesc.fRadian = 110.f;
	if (FAILED(pGameInstance->Add_Object_Layer(pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Gliding_Ring"), L"Layer_GlidingRing", &GlidingDesc)))
		return E_FAIL;
	iIndex++;

	GlidingDesc.vPos = { 217.5f, 71.3f, 256.f, 1.f };
	GlidingDesc.fRadian = 120.f;
	if (FAILED(pGameInstance->Add_Object_Layer(pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Gliding_Ring"), L"Layer_GlidingRing", &GlidingDesc)))
		return E_FAIL;
	iIndex++;

	m_iCreateNum++;

	for (size_t i = 0; i < iIndex; i++)
	{
		Gliding_Ring.push_back(static_cast<CGliding_Ring*>(pGameInstance->Find_Object(pGameInstance->Get_CurrentLevel(), L"Layer_GlidingRing", static_cast<_uint>(i))));
	}
	
	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CGliding_Test::Create_AddRing()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (m_iCreateNum == 1 && Gliding_Ring.size() == 0)
	{
		_int iIndex = 0;
		CGliding_Ring::GLIDINGRING_DESC GlidingDesc;
		/* 벤티석상 머리 왼쪽 옆 3개 */
		GlidingDesc.vPos = { 221.5f, 72.5f, 258.f, 1.f };
		GlidingDesc.fRadian = 90.f;
		if (FAILED(pGameInstance->Add_Object_Layer(pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Gliding_Ring"), L"Layer_GlidingRing", &GlidingDesc)))
			return E_FAIL;
		iIndex++;

		GlidingDesc.vPos = { 225.f, 71.5f, 259.f, 1.f };
		GlidingDesc.fRadian = 60.f;
		if (FAILED(pGameInstance->Add_Object_Layer(pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Gliding_Ring"), L"Layer_GlidingRing", &GlidingDesc)))
			return E_FAIL;
		iIndex++;

		GlidingDesc.vPos = { 229.f, 68.3f, 262.f, 1.f };
		GlidingDesc.fRadian = 70.f;
		if (FAILED(pGameInstance->Add_Object_Layer(pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Gliding_Ring"), L"Layer_GlidingRing", &GlidingDesc)))
			return E_FAIL;
		iIndex++;

		for (size_t i = 0; i < iIndex; i++)
		{
			Gliding_Ring.push_back(static_cast<CGliding_Ring*>(pGameInstance->Find_Object(pGameInstance->Get_CurrentLevel(), L"Layer_GlidingRing", static_cast<_uint>(i))));
		}

		m_iCreateNum++;
	}
	else if (m_iCreateNum == 2 && Gliding_Ring.size() == 0)
	{
		_int iIndex = 0;
		CGliding_Ring::GLIDINGRING_DESC GlidingDesc;
		/* 벤티석상 머리 왼쪽 옆 3개 */
		GlidingDesc.vPos = { 233.f, 66.f, 265.f, 1.f };
		GlidingDesc.fRadian = 50.f;
		if (FAILED(pGameInstance->Add_Object_Layer(pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Gliding_Ring"), L"Layer_GlidingRing", &GlidingDesc)))
			return E_FAIL;
		iIndex++;

		GlidingDesc.vPos = { 235.f, 65.5f, 263.5f, 1.f };
		GlidingDesc.fRadian = 80.f;
		if (FAILED(pGameInstance->Add_Object_Layer(pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Gliding_Ring"), L"Layer_GlidingRing", &GlidingDesc)))
			return E_FAIL;
		iIndex++;

		GlidingDesc.vPos = { 237.f, 65.f, 263.3f, 1.f };
		GlidingDesc.fRadian = 100.f;
		if (FAILED(pGameInstance->Add_Object_Layer(pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Gliding_Ring"), L"Layer_GlidingRing", &GlidingDesc)))
			return E_FAIL;
		iIndex++;

		for (size_t i = 0; i < iIndex; i++)
		{
			Gliding_Ring.push_back(static_cast<CGliding_Ring*>(pGameInstance->Find_Object(pGameInstance->Get_CurrentLevel(), L"Layer_GlidingRing", static_cast<_uint>(i))));
		}

		m_iCreateNum++;
	}
	else if (m_iCreateNum == 3 && Gliding_Ring.size() == 0)
	{
		_int iIndex = 0;
		CGliding_Ring::GLIDINGRING_DESC GlidingDesc;
		/* 벤티석상 손 왼쪽 옆 3개 */
		GlidingDesc.vPos = { 239.9f, 68.f, 258.5f, 1.f };
		GlidingDesc.fRadian = 40.f;
		if (FAILED(pGameInstance->Add_Object_Layer(pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Gliding_Ring"), L"Layer_GlidingRing", &GlidingDesc)))
			return E_FAIL;
		iIndex++;

		GlidingDesc.vPos = { 241.f, 67.f, 256.f, 1.f };
		GlidingDesc.fRadian = 50.f;
		if (FAILED(pGameInstance->Add_Object_Layer(pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Gliding_Ring"), L"Layer_GlidingRing", &GlidingDesc)))
			return E_FAIL;
		iIndex++;

		GlidingDesc.vPos = { 240.5f, 66.5f, 254.3f, 1.f };
		GlidingDesc.fRadian = 50.f;
		if (FAILED(pGameInstance->Add_Object_Layer(pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Gliding_Ring"), L"Layer_GlidingRing", &GlidingDesc)))
			return E_FAIL;
		iIndex++;

		for (size_t i = 0; i < iIndex; i++)
		{
			Gliding_Ring.push_back(static_cast<CGliding_Ring*>(pGameInstance->Find_Object(pGameInstance->Get_CurrentLevel(), L"Layer_GlidingRing", static_cast<_uint>(i))));
		}

		m_iCreateNum++;
	}

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CGliding_Test::Create_AcceleratingRing()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	
	_int iIndex = 0;
	CAccelerating_Ring::ACCELERATING_DESC AcceleratingDesc;

	/* 처음 3개 */
	AcceleratingDesc.vPos = { 180.f, 61.8f, 256.f, 1.f };
	AcceleratingDesc.fRadian = 90.f;
	if (FAILED(pGameInstance->Add_Object_Layer(pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Accelerating_Ring"), L"Layer_AcceleratingRing", &AcceleratingDesc)))
		return E_FAIL;
	iIndex++;

	AcceleratingDesc.vPos = { 183.f, 61.6f, 256.f, 1.f };
	AcceleratingDesc.fRadian = 90.f;
	if (FAILED(pGameInstance->Add_Object_Layer(pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Accelerating_Ring"), L"Layer_AcceleratingRing", &AcceleratingDesc)))
		return E_FAIL;
	iIndex++;

	AcceleratingDesc.vPos = { 186.f, 61.4f, 256.f, 1.f };
	AcceleratingDesc.fRadian = 90.f;
	if (FAILED(pGameInstance->Add_Object_Layer(pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Accelerating_Ring"), L"Layer_AcceleratingRing", &AcceleratingDesc)))
		return E_FAIL;
	iIndex++;

	/* 성당 가는 길 4개 */
	AcceleratingDesc.vPos = { 248.f, 68.f, 230.f, 1.f };
	AcceleratingDesc.fRadian = -20.f;
	if (FAILED(pGameInstance->Add_Object_Layer(pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Accelerating_Ring"), L"Layer_AcceleratingRing", &AcceleratingDesc)))
		return E_FAIL;
	iIndex++;

	AcceleratingDesc.vPos = { 251.f, 67.8f, 227.f, 1.f };
	AcceleratingDesc.fRadian = -20.f;
	if (FAILED(pGameInstance->Add_Object_Layer(pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Accelerating_Ring"), L"Layer_AcceleratingRing", &AcceleratingDesc)))
		return E_FAIL;
	iIndex++;

	AcceleratingDesc.vPos = { 254.f, 67.6f, 224.f, 1.f };
	AcceleratingDesc.fRadian = -20.f;
	if (FAILED(pGameInstance->Add_Object_Layer(pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Accelerating_Ring"), L"Layer_AcceleratingRing", &AcceleratingDesc)))
		return E_FAIL;
	iIndex++;

	AcceleratingDesc.vPos = { 257.f, 67.4f, 221.f, 1.f };
	AcceleratingDesc.fRadian = -20.f;
	if (FAILED(pGameInstance->Add_Object_Layer(pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Accelerating_Ring"), L"Layer_AcceleratingRing", &AcceleratingDesc)))
		return E_FAIL;
	iIndex++;

	AcceleratingDesc.vPos = { 260.f, 67.4f, 218.f, 1.f };
	AcceleratingDesc.fRadian = -20.f;
	if (FAILED(pGameInstance->Add_Object_Layer(pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Accelerating_Ring"), L"Layer_AcceleratingRing", &AcceleratingDesc)))
		return E_FAIL;
	iIndex++;

	for (size_t i = 0; i < iIndex; i++)
	{
		Accelerating_Ring.push_back(static_cast<CAccelerating_Ring*>(pGameInstance->Find_Object(pGameInstance->Get_CurrentLevel(), L"Layer_AcceleratingRing", static_cast<_uint>(i))));
	}

	Safe_Release(pGameInstance);
	return S_OK;
}

void CGliding_Test::Release_Object()
{
	for (auto& pRing : Gliding_Ring)
		pRing->Set_Destroy(TRUE);

	for (auto& pDraft : UpDraft)
	{
		pDraft->Set_Destroy(TRUE);
		//pDraft->Release_Collider();
	}

	for (auto& pAccelerating : Accelerating_Ring)
		pAccelerating->Set_Destroy(TRUE);

	Gliding_Ring.clear();
	Accelerating_Ring.clear();
	UpDraft.clear();
}

HRESULT CGliding_Test::Initialize(void* pArg)
{
	if (pArg != nullptr)
	{
		if (FAILED(__super::Initialize(pArg)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CGliding_Test::Update(_float fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if(m_fStartTime < 3.f && m_bTryAgain == FALSE)
		m_fStartTime += fTimeDelta;

	if (m_fStartTime >= 3.f) /* 게임 시작 */
	{
		if (m_bStartGame == FALSE)
		{
			m_bStartGame = TRUE;

			if (FAILED(CGliding_Test::Create_Updraft()))
				return E_FAIL;

			if(FAILED(CGliding_Test::Create_Ring()))
				return E_FAIL;

			if (FAILED(CGliding_Test::Create_AcceleratingRing()))
				return E_FAIL;
		}
	}

	if (m_bStartGame == TRUE)
	{
		m_fTotalTime -= fTimeDelta;

		if (FAILED(CGliding_Test::Create_AddRing()))
			return E_FAIL;

		CUI_Manager* pUIManager = (CUI_Manager*)pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_UIManager"), 0);
		CPage_Main* pPage_Main = (CPage_Main*)pUIManager->Get_UI_Page(PAGE_MAIN);

		for (auto iter = Gliding_Ring.begin(); iter != Gliding_Ring.end();)
		{
			if ((*iter)->Get_Destroy() == TRUE)
			{
				iter = Gliding_Ring.erase(iter);
				pGameInstance->Add_InteractionCount(m_strMiniGameName);
			}
			else
				iter++;
		}

		if (m_fTotalTime >= 0.f && pGameInstance->Get_InteractionCount(m_strMiniGameName) == 12)
		{
			m_bClearGame = TRUE;

			CPlayer* pPlayer = static_cast<CPlayer*>(pGameInstance->Find_Object(LEVEL_WORLD, TEXT("Layer_Player")));
			pPlayer->Set_Gravity();
		}
		else if(m_fTotalTime < 0.f && m_bClearGame != TRUE)
		{
			pGameInstance->Set_InteractionCount(m_strMiniGameName);

			m_fTotalTime = 60.f;
			m_fStartTime = 0.f;
			m_bStartGame = FALSE;
			m_bTryAgain = TRUE;
			
			for (auto& pRing : Gliding_Ring)
				pRing->Set_Destroy(TRUE);

			for (auto& pDraft : UpDraft)
			{
				pDraft->Set_Destroy(TRUE);
				//pDraft->Release_Collider();
			}

			for (auto& pAccelerating : Accelerating_Ring)
				pAccelerating->Set_Destroy(TRUE);

			Gliding_Ring.clear();
			UpDraft.clear();
			Accelerating_Ring.clear();

			CPlayer* pPlayer = static_cast<CPlayer*>(pGameInstance->Find_Object(LEVEL_WORLD, TEXT("Layer_Player")));
			pPlayer->Set_Gravity();
		}

		if(m_bClearGame == TRUE || m_bTryAgain == TRUE)
			pPage_Main->Display_Flight_Test_UI(false);
		else
			pPage_Main->Display_Flight_Test_UI(true);

		pPage_Main->Update_Timer(m_fTotalTime);
		pPage_Main->Update_Acquired_Rings(pGameInstance->Get_InteractionCount(m_strMiniGameName));
	}

	Safe_Release(pGameInstance);
	return S_OK;
}

CGliding_Test* CGliding_Test::Create(void* pArg)
{
	CGliding_Test* pInstance = new CGliding_Test();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Initialized : CGliding_Test"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGliding_Test::Free()
{
	__super::Free();

	for (auto& pRing : Gliding_Ring)
	{
		Safe_Release(pRing);
	}
	Gliding_Ring.clear();

	for (auto& pRing : Accelerating_Ring)
	{
		Safe_Release(pRing);
	}
	Accelerating_Ring.clear();

	for (auto& pDraft : UpDraft)
	{
		//pDraft->Release_Collider();
		Safe_Release(pDraft);
	}
	UpDraft.clear();
}
