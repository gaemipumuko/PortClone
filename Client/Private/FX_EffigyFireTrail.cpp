#include "stdafx.h"
#include "FX_EffigyFireTrail.h"

#include "DeciderObject.h"
#include "GameInstance.h"

CFX_EffigyFireTrail::CFX_EffigyFireTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAssetFX(pDevice, pContext)
{
}

CFX_EffigyFireTrail::CFX_EffigyFireTrail(const CFX_EffigyFireTrail& rhs)
	: CAssetFX(rhs)
{
}

HRESULT CFX_EffigyFireTrail::Add_Component(void* pArg)
{
	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Trail"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EffigyFire_Trail"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_VIBuffer_Fire */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Trail_Fire"),
		TEXT("Com_VIBuffer_Fire"), (CComponent**)&m_pVIBufferTrailCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFX_EffigyFireTrail::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture2", 1)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFrameRate", &m_fFrame, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CFX_EffigyFireTrail::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFX_EffigyFireTrail::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	ASFX_DESC* pAsfxDesc = static_cast<ASFX_DESC*>(pArg);
	if (pAsfxDesc->ePosType == ASFX_DESC::POS_FIX)
		return E_FAIL;

	m_pTransformCom = pAsfxDesc->pTransform;
	Safe_AddRef(m_pTransformCom);

	return S_OK;
}

HRESULT CFX_EffigyFireTrail::PreviousTick(_float fTimeDelta)
{
	if (CDeciderObject::Get_RenderMode() != CDeciderObject::RMODE_UI)
	{
		if (FAILED(__super::PreviousTick(fTimeDelta)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CFX_EffigyFireTrail::Tick(_float fTimeDelta)
{
	if (CDeciderObject::Get_RenderMode() != CDeciderObject::RMODE_UI)
	{
		/* Frame */
		m_fFrame += fTimeDelta;
		m_fFrame = fmod(m_fFrame, 1.f);
	}

	return S_OK;
}

HRESULT CFX_EffigyFireTrail::LateTick(_float fTimeDelta)
{
	if (CDeciderObject::Get_RenderMode() != CDeciderObject::RMODE_UI)
	{
		/* Invalidate Trail */
		_matrix TrailMatrix = { };
		_vector	vLow = { }, vHigh = { };

		TrailMatrix =
			XMMatrixTranslation(-1.75f, 0.2f, 0.f) *
			XMMatrixRotationAxis(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(180.f)) *
			XMMatrixTranslation(0.f, 0.f, 0.f) *
			m_pTransformCom->Get_WorldMatrix();
		vLow = TrailMatrix.r[3];

		TrailMatrix =
			XMMatrixTranslation(1.75f, 0.2f, 0.f) *
			XMMatrixRotationAxis(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(180.f)) *
			XMMatrixTranslation(0.f, 0.f, 0.f) *
			m_pTransformCom->Get_WorldMatrix();
		vHigh = TrailMatrix.r[3];

		m_pVIBufferTrailCom->Set_PlayTrail(TRUE);
		m_pVIBufferTrailCom->Append_TrailVertex(fTimeDelta, vLow, vHigh);
		m_pVIBufferTrailCom->Invalidate_VertexBuff(fTimeDelta);
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RG_BLEND, this);

	return S_OK;
}

HRESULT CFX_EffigyFireTrail::Render()
{
	if (FAILED(CFX_EffigyFireTrail::Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(TRAIL_EFFIGYFIRE)))
		return E_FAIL;
	
	if (FAILED(m_pVIBufferTrailCom->Render()))
		return E_FAIL;

	return S_OK;
}

CFX_EffigyFireTrail* CFX_EffigyFireTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFX_EffigyFireTrail* pInstance = new CFX_EffigyFireTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CFX_EffigyFireTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFX_EffigyFireTrail::Clone(void* pArg)
{
	CFX_EffigyFireTrail* pInstance = new CFX_EffigyFireTrail(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CFX_EffigyFireTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFX_EffigyFireTrail::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferTrailCom);
}
