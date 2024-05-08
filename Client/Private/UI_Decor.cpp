#include "stdafx.h"
#include "UI_Decor.h"

#include "GameInstance.h"

CDecor::CDecor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: COrtho(pDevice, pContext)
{
}

CDecor::CDecor(const CDecor& rhs)
	: COrtho(rhs)
{
}

HRESULT CDecor::Add_Component(void* pArg)
{
	if (FAILED(__super::Add_Component(pArg)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(CGameObject::Add_Component(m_tOrthoDesc.eTextureLevel, m_tOrthoDesc.strTextureTag,
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CDecor::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDecor::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	COrtho::Initialize_Matrix();

	return S_OK;
}

HRESULT CDecor::PreviousTick(_float fTimeDelta)
{
	if (m_bWindowTrue == true)
	{
		if (FAILED(__super::PreviousTick(fTimeDelta)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CDecor::Tick(_float fTimeDelta)
{
	if (m_bWindowTrue == true)
	{
		if (FAILED(__super::Tick(fTimeDelta)))
			return E_FAIL;

	}
	return S_OK;
}

HRESULT CDecor::LateTick(_float fTimeDelta)
{
	if (m_bWindowTrue == true)
	{
		if (FAILED(__super::LateTick(fTimeDelta)))
			return E_FAIL;
		m_pRendererCom->Add_RenderGroup(CRenderer::RG_LATEUI, this);
	}

	return S_OK;
}

HRESULT CDecor::Render()
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

CDecor* CDecor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDecor* pInstance = new CDecor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CDecor"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDecor::Clone(void* pArg)
{
	CDecor* pInstance = new CDecor(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CDecor"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDecor::Free()
{
	__super::Free();
}
