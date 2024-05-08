#include "stdafx.h"
#include "AlphaRect.h"

#include "GameInstance.h"

CAlphaRect::CAlphaRect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: COrtho(pDevice, pContext)
{
}

CAlphaRect::CAlphaRect(const CAlphaRect& rhs)
	: COrtho(rhs)
{
}

HRESULT CAlphaRect::Add_Component(void* pArg)
{
	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(CGameObject::Add_Component(m_tOrthoDesc.eTextureLevel, m_tOrthoDesc.strTextureTag,
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CAlphaRect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAlphaRect::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	COrtho::Initialize_Matrix();

	m_eRenderType = RTYPE_GAMEPLAY;

	return S_OK;
}

HRESULT CAlphaRect::PreviousTick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{

	if (FAILED(__super::PreviousTick(fTimeDelta)))
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CAlphaRect::Tick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{
		if (FAILED(__super::Tick(fTimeDelta)))
			return E_FAIL;


	}
	return S_OK;
}

HRESULT CAlphaRect::LateTick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{
		if (FAILED(__super::LateTick(fTimeDelta)))
			return E_FAIL;
	}
	    //m_pRendererCom->Add_RenderGroup(CRenderer::RG_LATEUI, this);
    

	return S_OK;
}

HRESULT CAlphaRect::Render()
{

		if (FAILED(Bind_ShaderResources()))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &m_vUVScale, sizeof(_float2))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &m_vUVPosition, sizeof(_float2))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;


		//Render Font

		if (m_eTextPosition != TEXT_END)
		{
			_uint iLength = 0;

			if (m_eTextPosition == TEXT_DOWN)
			{
				iLength = static_cast<_int>(m_strText.size());
				m_pGameInstance->Render_Font(L"Font_Default", m_strText, _float2(static_cast<_float>(m_tOrthoDesc.fPosition.x - iLength / 2 * 38 * 0.50-5 + g_iWinSizeX / 2),
					static_cast<_float>(-(m_tOrthoDesc.fPosition.y - 40.f - g_iWinSizeY / 2))), 0.4f, Colors::White, 0.f, { 0.f,0.f }, false);
			}
			if (m_eTextPosition == TEXT_ON)
			{
				iLength = static_cast<_int>(m_strText.size());
				m_pGameInstance->Render_Font(L"Font_Default", m_strText, _float2(static_cast<_float>(m_tOrthoDesc.fPosition.x - iLength / 2 * 37 * 0.45 - 20 + g_iWinSizeX / 2),
					static_cast<_float>(-(m_tOrthoDesc.fPosition.y + 7.f - g_iWinSizeY / 2))), 0.4f, Colors::White, 0.f, { 0.f,0.f }, false);
			}

			iLength = static_cast<_int>(m_strDetailBelow.size());
			m_pGameInstance->Render_Font(L"Font_Default", m_strDetailBelow, _float2(static_cast<_float>(m_tOrthoDesc.fPosition.x - iLength / 2 * 20 * 0.5 + g_iWinSizeX / 2),
				static_cast<_float>(-(m_tOrthoDesc.fPosition.y - 60.f - g_iWinSizeY / 2))), 0.4f, Colors::Aquamarine, 0.f, { 0.f,0.f }, false);

		}
	

	return S_OK;
}

CAlphaRect* CAlphaRect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAlphaRect* pInstance = new CAlphaRect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CAlphaRect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAlphaRect::Clone(void* pArg)
{
	CAlphaRect* pInstance = new CAlphaRect(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CAlphaRect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAlphaRect::Free()
{
	__super::Free();
}
