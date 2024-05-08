#include "stdafx.h"
#include "UI_Manager.h"

#include "Ortho.h"
#include "GameInstance.h"
#include "Damage_Font.h"
#include "Inventory.h"

CUI_Manager::CUI_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CDeciderObject(pDevice, pContext)
{
}

CUI_Manager::CUI_Manager(const CUI_Manager& rhs)
	: CDeciderObject(rhs)
{
}

HRESULT CUI_Manager::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Manager::Initialize_Clone(void* pArg)
{
	/* For.Com_Renderer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;


	m_eCurrentPage = PAGE_MAIN;

	//	enum UI_PAGE	{ PAGE_MAIN,PAGE_PAIMON, PAGE_GACHA, PAGE_GACHA_RESULT, PAGE_INVEN, PAGE_DIALOGUE, PAGE_SHOP,PAGE_QUEST,PAGE_ALCHEMY,PAGE_COOK,PAGE_CHAR,PAGE_MAP,PAGE_TEAM,PAGE_END};


	COrtho* pPage = { };

	/* Clone Main Page */
	pPage = static_cast<COrtho*>(m_pGameInstance->Find_Object_Prototype(L"Prototype_GameObject_Page_Main")->Clone(nullptr));
	m_vec_UI_Pages.push_back(pPage);

	/* Clone Paimon Page */
	pPage = static_cast<COrtho*>(m_pGameInstance->Find_Object_Prototype(L"Prototype_GameObject_Paimon_Page")->Clone(nullptr));
	m_vec_UI_Pages.push_back(pPage);

	/* Clone Gacha Page */
	pPage = static_cast<COrtho*>(m_pGameInstance->Find_Object_Prototype(L"Prototype_GameObject_Gacha_Page")->Clone(nullptr));
	m_vec_UI_Pages.push_back(pPage);

	///* Clone Gacha Result Page */
	COrtho::ORTHO_DESC tOrthoDesc = {};
	tOrthoDesc.eTextureLevel = LEVEL_STATIC;
	tOrthoDesc.fPosition = { 0.f,0.f };
	tOrthoDesc.fSize = { g_iWinSizeX, g_iWinSizeY };

	pPage = static_cast<COrtho*>(m_pGameInstance->Find_Object_Prototype(L"Prototype_GameObject_Gacha_Result_Page")->Clone(&tOrthoDesc));
	m_vec_UI_Pages.push_back(pPage);

	///* Clone Inventory Page */
	pPage = static_cast<COrtho*>(m_pGameInstance->Find_Object_Prototype(L"Prototype_GameObject_Inventory")->Clone(nullptr));
	m_vec_UI_Pages.push_back(pPage);

	/* Clone Dialogue Page */
	pPage = static_cast<COrtho*>(m_pGameInstance->Find_Object_Prototype(L"Prototype_GameObject_Dialogue_Page")->Clone(nullptr));
	m_vec_UI_Pages.push_back(pPage);

	/* Clone Shop Page */
	pPage = static_cast<COrtho*>(m_pGameInstance->Find_Object_Prototype(L"Prototype_GameObject_Shop_Page")->Clone(nullptr));
	m_vec_UI_Pages.push_back(pPage);

	/* Clone Quest Page */
	pPage = static_cast<COrtho*>(m_pGameInstance->Find_Object_Prototype(L"Prototype_GameObject_Quest_Page")->Clone(nullptr));
	m_vec_UI_Pages.push_back(pPage);

	/* Clone Alchemy Page */
	pPage = static_cast<COrtho*>(m_pGameInstance->Find_Object_Prototype(L"Prototype_GameObject_Alchemy_Page")->Clone(nullptr));
	m_vec_UI_Pages.push_back(pPage);

	/* Clone Cooking Page */
	pPage = static_cast<COrtho*>(m_pGameInstance->Find_Object_Prototype(L"Prototype_GameObject_Cooking_Page")->Clone(nullptr));
	m_vec_UI_Pages.push_back(pPage);

	return S_OK;

}

HRESULT CUI_Manager::PreviousTick(_float fTimeDelta)
{
	//for (auto iter : m_UI_Pages)
	//{
	//	iter->PreviousTick(fTimeDelta);
	//}
	if (m_eRenderMode == RMODE_UI)
	{
		if (m_eRenderUI == RUI_CHAR)
			Invalidate_CameraMode(RMODE_UI);
		else
			Invalidate_CameraMode(RMODE_GAMEPLAY, FALSE);
	}
	else
		Invalidate_CameraMode(RMODE_GAMEPLAY, TRUE);


	//for (auto iter : m_UI_Objects)


	m_vec_UI_Pages[m_eCurrentPage]->PreviousTick(fTimeDelta);

	if (m_eCurrentPage == PAGE_QUEST ||
		m_eCurrentPage == PAGE_INVEN ||
		m_eCurrentPage == PAGE_ALCHEMY ||
		m_eCurrentPage == PAGE_COOK ||
		m_eCurrentPage == PAGE_SHOP)
		m_pGameInstance->Set_InvenBlur(TRUE);
	else
		m_pGameInstance->Set_InvenBlur(FALSE);

	return S_OK;
}

HRESULT CUI_Manager::Tick(_float fTimeDelta)
{

	m_vec_UI_Pages[m_eCurrentPage]->Tick(fTimeDelta);



	//for (auto iter : m_vec_UI_Pages)
	//{
	//	iter->Tick(fTimeDelta);
	//}

	for (auto iter : m_DamageFontList)
	{
		iter->Tick(fTimeDelta);
	}

	return S_OK;
}

HRESULT CUI_Manager::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_PREVUI, this);

	m_vec_UI_Pages[m_eCurrentPage]->LateTick(fTimeDelta);


	//for (auto iter : m_vec_UI_Pages)
	//{
	//	iter->LateTick(fTimeDelta);
	//}



	if (m_iNumInteraction == 0)
	{
		m_iSelectedBoxIdx = 0;
	}

	if (m_iNumInteraction > 0)
	{
		if (m_iSelectedBoxIdx > m_iNumInteraction-1)
		{
			m_iSelectedBoxIdx = m_iNumInteraction-1;
		}


		if (m_pGameInstance->Key_Down(VK_UP))
		{
			if (m_iSelectedBoxIdx > 0)
			{
				m_iSelectedBoxIdx--;
			}
		}
		if (m_pGameInstance->Key_Down(VK_DOWN))
		{
			if (m_iSelectedBoxIdx < m_iNumInteraction - 1)
			{
				m_iSelectedBoxIdx++;
			}
		}

	}

	for (auto& iter = m_DamageFontList.begin(); iter != m_DamageFontList.end();)
	{
		if ((*iter)->Get_Dead() == TRUE)
		{
			Safe_Release(*iter);
			iter = m_DamageFontList.erase(iter);
		}
		else
		{
			(*iter)->LateTick(fTimeDelta);
			++iter;
		}
	}


	return S_OK;
}

HRESULT CUI_Manager::Render()
{
	for (auto iter : m_DamageFontList)
	{
		iter->Render();
	}

	m_iNumInteraction = 0;

	return S_OK;
}

HRESULT CUI_Manager::Add_UI_Object(UI_PAGE iPageIndex, COrtho* pGameObject)
{
	if (pGameObject == nullptr)
		return E_FAIL;

	if (iPageIndex >= PAGE_END)
		return E_FAIL;

	// m_UI_Objects[iPageIndex].push_back(pGameObject);

	return S_OK;
}

HRESULT CUI_Manager::Close_Page(UI_PAGE iPageIndex)
{
	return S_OK;
}

HRESULT CUI_Manager::Show_Page(UI_PAGE iPageIndex)
{
	if (iPageIndex == PAGE_MAIN || iPageIndex == PAGE_DIALOGUE)
	{
		m_eRenderMode = CDeciderObject::RMODE_GAMEPLAY;
		m_eRenderUI = CDeciderObject::RUI_END;
	}
	else if (iPageIndex == PAGE_INVEN)
	{
		m_eRenderMode = CDeciderObject::RMODE_UI;
		m_eRenderUI = CDeciderObject::RUI_INVEN;

	}
	else
	{
		m_eRenderMode = CDeciderObject::RMODE_UI;
		m_eRenderUI = CDeciderObject::RUI_END;
	}


	m_eCurrentPage = iPageIndex;

	return S_OK;
}


_int CUI_Manager::Register_InteractionBox(_int iNumbox)
{
	if (m_iNumInteraction + iNumbox < 6)
	{
		m_iNumInteraction = m_iNumInteraction + iNumbox;

		return m_iNumInteraction;

	}
	else
	{
		return -1;
	}
}

void CUI_Manager::Add_DamageFonts(CDamage_Font* DamageFont)
{
	m_DamageFontList.push_back(DamageFont);
}

CUI_Manager* CUI_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Manager* pInstance = new CUI_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CUI_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Manager::Clone(void* pArg)
{
	CUI_Manager* pInstance = new CUI_Manager(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CUI_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);


	for (auto& iter : m_vec_UI_Pages)
		Safe_Release(iter);
	m_vec_UI_Pages.clear();

	for (auto& iter : m_DamageFontList)
		Safe_Release(iter);
	m_DamageFontList.clear();

	// Safe_Delete_Array(m_UI_Objects);

}
