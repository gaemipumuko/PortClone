#include "stdafx.h"
#include "DeciderObject.h"

#include "Camera_Main.h"
#include "Camera_Char.h"

#include "GameInstance.h"

CDeciderObject::RENDER_MODE CDeciderObject::m_eRenderMode = CDeciderObject::RENDER_MODE::RMODE_END;
CDeciderObject::RENDER_UI CDeciderObject::m_eRenderUI = CDeciderObject::RENDER_UI::RUI_END;

CDeciderObject::CDeciderObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CDeciderObject::CDeciderObject(const CDeciderObject& rhs)
	: CGameObject(rhs)
{
}

void CDeciderObject::Invalidate_CameraMode(RENDER_MODE eMode, _bool bMove)
{
	if (LEVEL_LOADING == m_pGameInstance->Get_CurrentLevel())
		return;

	if (eMode == RMODE_GAMEPLAY)
	{
		static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera", 0))->Set_Activated(TRUE);
		static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera", 0))->Set_Activated_Move(bMove);
	}
	else
	{
		static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera", 0))->Set_Activated(FALSE);
	}
}

HRESULT CDeciderObject::Verify_TickValidity()
{
	if ((CDeciderObject::m_eRenderMode ^ m_eRenderType) == FALSE)
	{
		if ((CDeciderObject::m_eRenderUI ^ m_eUIState) == FALSE)
			return S_OK;
		else
			return E_FAIL;
	}
	else
		return E_FAIL;
}

HRESULT CDeciderObject::Verify_RenderValidity()
{
	if (CDeciderObject::m_eRenderMode == RMODE_UI && CDeciderObject::m_eRenderUI == RUI_CHAR)
		return E_FAIL;

	return S_OK;
}

HRESULT CDeciderObject::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	return S_OK;
}

void CDeciderObject::Free()
{
	__super::Free();
}
