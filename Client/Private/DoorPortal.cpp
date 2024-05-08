#include "stdafx.h"
#include "DoorPortal.h"

#include "GameInstance.h"


CDoor_Portal::CDoor_Portal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect(pDevice, pContext)
{
}

CDoor_Portal::CDoor_Portal(const CDoor_Portal& rhs)
	: CEffect(rhs)
{
}

HRESULT CDoor_Portal::Add_Component(void* pArg)
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
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Eff_Universe"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CDoor_Portal::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CDoor_Portal::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDoor_Portal::Initialize_Clone(void* pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		EFFECT_DESC* pDesc = static_cast<EFFECT_DESC*>(pArg);

		m_EffectDesc.eEffectID = pDesc->eEffectID;
		m_EffectDesc.vSpawnPos = pDesc->vSpawnPos;
		m_EffectDesc.vScale	= pDesc->vScale;
		m_EffectDesc.vBaseColor = pDesc->vBaseColor;
		m_EffectDesc.pParent = pDesc->pParent;

		m_fLifeTime = m_EffectDesc.fLifeTime;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectDesc.vSpawnPos);
		m_pTransformCom->Set_Scale(m_EffectDesc.vScale);

	}
	else
		return E_FAIL;

	m_pTransformCom->Rotation({ 0.f,1.f,0.f,0.f }, XMConvertToRadians(90.f));

	return S_OK;
}

HRESULT CDoor_Portal::PreviousTick(_float fTimeDelta)
{

	return S_OK;
}

HRESULT CDoor_Portal::Tick(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	m_pTransformCom->Rotation({ 0.f,1.f,0.f,0.f }, XMConvertToRadians(90.f));

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CDoor_Portal::LateTick(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix());


	m_pRendererCom->Add_RenderGroup(CRenderer::RG_BLEND, this);

	return S_OK;
}

HRESULT CDoor_Portal::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture",0)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture2", 1)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &m_fTimeAcc, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(VTXTEX_DOOR_PORTAL)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	return S_OK;
}

void CDoor_Portal::Set_Matrix(_matrix vWorldMat)
{
	m_pTransformCom->Set_WorldMatrix(vWorldMat);
}

CDoor_Portal* CDoor_Portal::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDoor_Portal* pInstance = new CDoor_Portal(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CDoor_Portal"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDoor_Portal::Clone(void* pArg)
{
	CDoor_Portal* pInstance = new CDoor_Portal(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CDoor_Portal"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDoor_Portal::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}