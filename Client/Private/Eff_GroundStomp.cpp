#include "stdafx.h"
#include "Eff_GroundStomp.h"

#include "GameInstance.h"

CEff_GroundStomp::CEff_GroundStomp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect(pDevice, pContext)
{
}

CEff_GroundStomp::CEff_GroundStomp(const CEff_GroundStomp& rhs)
	: CEffect(rhs)
{
}

HRESULT CEff_GroundStomp::Add_Component(void* pArg)
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
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Eff_Ground_Stomp"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CEff_GroundStomp::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEff_GroundStomp::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEff_GroundStomp::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		EFFECT_DESC* pDesc = static_cast<EFFECT_DESC*>(pArg);

		m_EffectDesc.eEffectID = pDesc->eEffectID;
		m_EffectDesc.fLifeTime = pDesc->fLifeTime;
		m_EffectDesc.vSpawnPos = pDesc->vSpawnPos;
		m_EffectDesc.vScale	= pDesc->vScale;
		m_vScale = pDesc->vScale;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectDesc.vSpawnPos);
		m_pTransformCom->Set_Scale(m_EffectDesc.vScale);
	}
	else
		return E_FAIL;

	m_EffectDesc.fLifeTime = 1.f;

	m_pTransformCom->Rotation({ 1.f,0.f,0.f,0.f }, XMConvertToRadians(90.f));

	m_pGameInstance->Play_Particle(TEXT("Dust0"), Get_ParticleTag(TEXT("Dust0")), PARTICLE_DUST, TEX_PARTICLE_DUST, nullptr, m_pTransformCom, 1.f);
	m_pGameInstance->Play_Particle(TEXT("Dust1"), Get_ParticleTag(TEXT("Dust1")), PARTICLE_DUST, TEX_PARTICLE_DUST, nullptr, m_pTransformCom, 1.f);


	m_pGameInstance->Play_Particle(TEXT("RockShard3"), Get_ParticleTag(TEXT("RockShard3")), PARTICLE_SOLID_DEBRIS, TEX_PARTICLE_DEBRIS_AT, nullptr, m_pTransformCom, 1.f);
	m_pGameInstance->Play_Particle(TEXT("RockShard4"), Get_ParticleTag(TEXT("RockShard4")), PARTICLE_SOLID_DEBRIS, TEX_PARTICLE_DEBRIS_AT, nullptr, m_pTransformCom, 1.f);
	m_pGameInstance->Play_Particle(TEXT("RockShard5"), Get_ParticleTag(TEXT("RockShard5")), PARTICLE_SOLID_DEBRIS, TEX_PARTICLE_DEBRIS_AT, nullptr, m_pTransformCom, 1.f);


	return S_OK;
}

HRESULT CEff_GroundStomp::PreviousTick(_float fTimeDelta)
{

	return S_OK;
}

HRESULT CEff_GroundStomp::Tick(_float fTimeDelta)
{
	m_EffectDesc.fLifeTime -= fTimeDelta;
	if (m_EffectDesc.fLifeTime <= 0.f)
	{
		Set_Destroy(TRUE);
	}

	m_fTimeAcc += fTimeDelta*3.f;

	//m_fColorRate0 += log(m_fTimeAcc / 10 + 1.f) * 4.f;

	//XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

	_vector vScale = m_pTransformCom->Get_Scale();
	if (m_fTimeAcc<0.3f)
	{
	vScale.m128_f32[0] = m_vScale.m128_f32[0]*( 1 + m_fTimeAcc*2.f);
	vScale.m128_f32[1] = m_vScale.m128_f32[1]*( 1 + m_fTimeAcc*2.f);
	m_pTransformCom->Set_Scale(vScale);
	}

	return S_OK;
}

HRESULT CEff_GroundStomp::LateTick(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix());

	m_pRendererCom->Add_RenderGroup(CRenderer::RG_POSTEFFECT, this);

	return S_OK;
}

HRESULT CEff_GroundStomp::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	//_vector vSelfScale = m_pTransformCom->Get_Scale();
	//vSelfScale.m128_f32[0] += 0.01f;
	//vSelfScale.m128_f32[1] += 0.01f;
	//m_pTransformCom->Set_Scale(vSelfScale);


	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &m_fTimeAcc, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(VTXTEX_EFF_GROUND_STOMP)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

CEff_GroundStomp* CEff_GroundStomp::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEff_GroundStomp* pInstance = new CEff_GroundStomp(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CEff_GroundStomp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEff_GroundStomp::Clone(void* pArg)
{
	CEff_GroundStomp* pInstance = new CEff_GroundStomp(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CEff_GroundStomp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEff_GroundStomp::Free()
{
	__super::Free();

	if (m_bCloned == true)
	{

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);

	}
}