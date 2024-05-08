#include "stdafx.h"
#include "Page.h"

#include "GameInstance.h"

CPage::CPage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CPage::CPage(const CPage& rhs)
	: CGameObject(rhs)
{
}

HRESULT CPage::Add_Component(void* pArg)
{
	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
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

	return S_OK;
}

HRESULT CPage::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_ViewMatrix)))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderCurrentTexture(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	return S_OK;
}

HRESULT CPage::Initialize_Matrix()
{
	_vector vScale = XMLoadFloat2(&m_tOrthoDesc.fSize);
	m_pTransformCom->Set_Scale(vScale);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(m_tOrthoDesc.fPosition.x, m_tOrthoDesc.fPosition.y, 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(static_cast<_float>(g_iWinSizeX), static_cast<_float>(g_iWinSizeY), 0.f, 1.f));

	return S_OK;
}

HRESULT CPage::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPage::Initialize_Clone(void* pArg)
{
	if (pArg != nullptr)
	{
		ORTHO_DESC* pDesc = static_cast<ORTHO_DESC*>(pArg);

		m_tOrthoDesc.eTextureLevel = pDesc->eTextureLevel;
		m_tOrthoDesc.strTextureTag = pDesc->strTextureTag;
		m_tOrthoDesc.fPosition = pDesc->fPosition;
		m_tOrthoDesc.fSize = pDesc->fSize;
	}

	return S_OK;
}

HRESULT CPage::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CPage::Tick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CPage::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_PREVUI, this);

	

	return S_OK;
}

HRESULT CPage::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(VTXTEX_DEFAULT)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CPage::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
}
