#include "stdafx.h"
#include "ItemGlow.h"

#include "GameInstance.h"
#include "Eff_Effigy_Recharge_Trail.h"

CEff_Effigy_Recharge_Trail::CEff_Effigy_Recharge_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect(pDevice, pContext)
{
}

CEff_Effigy_Recharge_Trail::CEff_Effigy_Recharge_Trail(const CEff_Effigy_Recharge_Trail& rhs)
	: CEffect(rhs)
{
}

HRESULT CEff_Effigy_Recharge_Trail::Add_Component(void* pArg)
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
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Recharge_Trail"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CEff_Effigy_Recharge_Trail::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEff_Effigy_Recharge_Trail::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEff_Effigy_Recharge_Trail::Initialize_Clone(void* pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	if (pArg == nullptr)
		return E_FAIL;

	EFFECT_DESC* pDesc = static_cast<EFFECT_DESC*>(pArg);

	m_EffectDesc.eEffectID = pDesc->eEffectID;
	m_EffectDesc.fLifeTime = pDesc->fLifeTime;
	m_EffectDesc.vSpawnPos = pDesc->vSpawnPos;
	m_EffectDesc.vScale = pDesc->vScale;
	m_EffectDesc.vBaseColor = pDesc->vBaseColor;
	m_EffectDesc.pParent = pDesc->pParent;

	m_fLifeTime = m_EffectDesc.fLifeTime;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectDesc.vSpawnPos + XMVectorSet(0.f, 1.f, 0.f, 0.f));
	m_pTransformCom->Set_Scale(m_EffectDesc.vScale);

		

	m_pTransformCom->Look_At_Direction(pDesc->vDir);
	_vector vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	m_pTransformCom->RotationFromCurrentState(vUp, XMConvertToRadians(90.f));
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	m_pTransformCom->RotationFromCurrentState(vLook, XMConvertToRadians(-4.f));
	// 
	//m_pTransformCom->Rotation({ 0.f,0.f,1.f,0.f }, XMConvertToRadians(90.f));

	return S_OK;
}

HRESULT CEff_Effigy_Recharge_Trail::PreviousTick(_float fTimeDelta)
{

	return S_OK;
}

HRESULT CEff_Effigy_Recharge_Trail::Tick(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	m_fEmittionTime += fTimeDelta;
	if (m_fEmittionTime > .1f)
	{
		m_fEmittionTime = 0.f;
	}

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CEff_Effigy_Recharge_Trail::LateTick(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix());


	m_pRendererCom->Add_RenderGroup(CRenderer::RG_BLEND, this);

	return S_OK;
}

HRESULT CEff_Effigy_Recharge_Trail::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;



	//m_pTransformCom->Rotation({ 0.f,1.f,0.f,0.f }, XMConvertToRadians(0.f));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture",0)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture",1)))
		return E_FAIL;
	_float2 fUVPos = {0.f,0.f};
	_float2 fUVScale = {1.f,3.f};

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &m_fTimeAcc, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(VTXTEX_EFF_EFFIGY_RECHARGE_TRAIL)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	//m_pTransformCom->Rotation({ 1.f,0.f,0.f,0.f }, XMConvertToRadians(90.f));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(VTXTEX_EFF_EFFIGY_RECHARGE_TRAIL)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	return S_OK;
}

void CEff_Effigy_Recharge_Trail::Set_Matrix(_matrix vWorldMat)
{
	m_pTransformCom->Set_WorldMatrix(vWorldMat);
}

CEff_Effigy_Recharge_Trail* CEff_Effigy_Recharge_Trail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEff_Effigy_Recharge_Trail* pInstance = new CEff_Effigy_Recharge_Trail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CEff_Effigy_Recharge_Trail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEff_Effigy_Recharge_Trail::Clone(void* pArg)
{
	CEff_Effigy_Recharge_Trail* pInstance = new CEff_Effigy_Recharge_Trail(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CEff_Effigy_Recharge_Trail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEff_Effigy_Recharge_Trail::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}