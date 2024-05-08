#include "stdafx.h"
#include "ButtonAlpha.h"
// #include "CUI_Manager.h"

#include "GameInstance.h"

CButtonAlpha::CButtonAlpha(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: COrtho(pDevice, pContext)
{
}

CButtonAlpha::CButtonAlpha(const CButtonAlpha& rhs)
	: COrtho(rhs)
{
}

HRESULT CButtonAlpha::Add_Component(void* pArg)
{
	if (FAILED(__super::Add_Component(pArg)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(CGameObject::Add_Component(m_tOrthoDesc.eTextureLevel, m_tOrthoDesc.strTextureTag,
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CButtonAlpha::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CButtonAlpha::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	COrtho::Initialize_Matrix();

	m_eRenderType = RENDER_TYPE::RTYPE_GAMEPLAY;

	return S_OK;
}

HRESULT CButtonAlpha::PreviousTick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{
		if (m_bWindowTrue == true)
		{
			if (FAILED(__super::PreviousTick(fTimeDelta)))
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CButtonAlpha::Tick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{
		if (m_bWindowTrue == true)
		{
			if (FAILED(__super::Tick(fTimeDelta)))
				return E_FAIL;

			POINT       ptMouse;
			RECT        rcDimentions;

			GetCursorPos(&ptMouse);

			ScreenToClient(g_hWnd, &ptMouse);

			ptMouse.x = ptMouse.x - g_iWinSizeX / 2;
			ptMouse.y = (-ptMouse.y + g_iWinSizeY / 2);

			rcDimentions.left = static_cast<LONG>(m_tOrthoDesc.fPosition.x - m_tOrthoDesc.fSize.x / 2.f);
			rcDimentions.top = static_cast<LONG>(m_tOrthoDesc.fPosition.y - m_tOrthoDesc.fSize.y / 2.f);
			rcDimentions.right = static_cast<LONG>(m_tOrthoDesc.fPosition.x + m_tOrthoDesc.fSize.x / 2.f);
			rcDimentions.bottom = static_cast<LONG>(m_tOrthoDesc.fPosition.y + m_tOrthoDesc.fSize.y / 2.f);

			if (PtInRect(&rcDimentions, ptMouse) && m_pGameInstance->Key_Down(VK_LBUTTON))
			{

				if (m_tOrthoDesc.strTextureTag == L"Prototype_Component_Texture_UI_Icon_Paimon_Menu")
				{

					// CUI_Manager::Get_Instance()->Close_Page(PAGE_MAIN);
					// CUI_Manager::Get_Instance()->Show_Page(PAGE_PAIMON);

				}


				if (m_tOrthoDesc.strTextureTag == L"Prototype_Component_Texture_UI_Icon_Quit_Game")
				{
					// CUI_Manager::Get_Instance()->Close_Page(PAGE_PAIMON);
					// CUI_Manager::Get_Instance()->Show_Page(PAGE_MAIN);
				}

			}
		}
	}

	return S_OK;
}

HRESULT CButtonAlpha::LateTick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{
		if (m_bWindowTrue == true)
		{
			if (FAILED(__super::LateTick(fTimeDelta)))
				return E_FAIL;

			//m_pRendererCom->Add_RenderGroup(CRenderer::RG_LATEUI, this);
		}
	}

	return S_OK;
}

HRESULT CButtonAlpha::Render()
{
	if (m_bWindowTrue == true)
	{
		if (FAILED(Bind_ShaderResources()))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}
	return S_OK;
}

CButtonAlpha* CButtonAlpha::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CButtonAlpha* pInstance = new CButtonAlpha(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CButtonAlpha"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CButtonAlpha::Clone(void* pArg)
{
	CButtonAlpha* pInstance = new CButtonAlpha(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CButtonAlpha"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CButtonAlpha::Free()
{
	__super::Free();
}
