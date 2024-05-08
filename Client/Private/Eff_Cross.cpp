#include "stdafx.h"
#include "Eff_Cross.h"

#include "GameInstance.h"

CEff_Cross::CEff_Cross(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect(pDevice, pContext)
{
}

CEff_Cross::CEff_Cross(const CEff_Cross& rhs)
	: CEffect(rhs)
{
}

HRESULT CEff_Cross::Add_Component(void* pArg)
{
	__super::Add_Component(pArg);

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Eff_Cross"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CEff_Cross::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEff_Cross::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEff_Cross::Initialize_Clone(void* pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		EFFECT_DESC* pDesc = static_cast<EFFECT_DESC*>(pArg);

		m_EffectDesc.eEffectID = pDesc->eEffectID;
		m_EffectDesc.fLifeTime = pDesc->fLifeTime;
		m_EffectDesc.vSpawnPos = pDesc->vSpawnPos;
		m_EffectDesc.vScale	= pDesc->vScale;
		m_EffectDesc.vDir = pDesc->vDir;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectDesc.vSpawnPos);
		m_pTransformCom->Set_Scale(XMVectorSet(1.f, 1.f, 1.f, 1.f));
	}
	else
		return E_FAIL;

	m_EffectDesc.fLifeTime = 2.f;

	// m_pTransformCom->Look_At_Direction(m_EffectDesc.vDir);
	// m_pTransformCom->RotationFromCurrentState(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(90.f));


	return S_OK;
}

HRESULT CEff_Cross::PreviousTick(_float fTimeDelta)
{

	return S_OK;
}

HRESULT CEff_Cross::Tick(_float fTimeDelta)
{
	m_EffectDesc.fLifeTime -= fTimeDelta;
	if (m_EffectDesc.fLifeTime <= 0.f)
	{
		Set_Destroy(TRUE);
	}

	m_fTimeAcc += fTimeDelta;

	//if (m_fTimeAcc <= 1.f)
	//{
	//	_float fSize = sin(XMConvertToRadians(m_fTimeAcc * 90.f)) * m_EffectDesc.vScale.m128_f32[0];
	//	m_pTransformCom->Set_Scale(XMVectorSet(fSize, fSize, fSize, 1.f));
	//}

	return S_OK;
}

HRESULT CEff_Cross::LateTick(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix());

	m_pRendererCom->Add_RenderGroup(CRenderer::RG_POSTEFFECT, this);

	return S_OK;
}

HRESULT CEff_Cross::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &m_fTimeAcc, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(VTXTEX_CROSS)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

CEff_Cross* CEff_Cross::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEff_Cross* pInstance = new CEff_Cross(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CEff_Cross"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEff_Cross::Clone(void* pArg)
{
	CEff_Cross* pInstance = new CEff_Cross(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CEff_Cross"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEff_Cross::Free()
{
	__super::Free();

	if (m_bCloned == true)
	{

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);

	}
}