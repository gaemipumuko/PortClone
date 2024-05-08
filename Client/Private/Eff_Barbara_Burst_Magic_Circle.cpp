#include "stdafx.h"
#include "Eff_Barbara_Burst_Magic_Circle.h"

#include "GameInstance.h"
#include "Player.h"

CBarbara_Burst_Circle::CBarbara_Burst_Circle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect(pDevice, pContext)
{
}

CBarbara_Burst_Circle::CBarbara_Burst_Circle(const CBarbara_Burst_Circle& rhs)
	: CEffect(rhs)
{
}

HRESULT CBarbara_Burst_Circle::Add_Component(void* pArg)
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
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Burst_Circle_Diffuse"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Water_Turbulence"),
		TEXT("Com_Texture_Ramp"), (CComponent**)&m_pNoiseTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBarbara_Burst_Circle::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBarbara_Burst_Circle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBarbara_Burst_Circle::Initialize_Clone(void* pArg)
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

		m_fLifeTime = m_EffectDesc.fLifeTime;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectDesc.vSpawnPos);
		m_pTransformCom->Set_Scale(m_EffectDesc.vScale);

	}
	else
		return E_FAIL;

	m_pGameInstance->Play_Particle(TEXT("Cross"), Get_ParticleTag(TEXT("Cross")), PARTICLE_BLINK_SHRINK, TEX_PARTICLE_CROSS2, nullptr, m_pTransformCom, 2.f);
	m_pGameInstance->Play_Particle(TEXT("Cross1"), Get_ParticleTag(TEXT("Cross1")), PARTICLE_BLINK_SHRINK, TEX_PARTICLE_CROSS2, nullptr, m_pTransformCom, 2.f);
	m_pGameInstance->Play_Particle(TEXT("Cross2"), Get_ParticleTag(TEXT("Cross2")), PARTICLE_BLINK_SHRINK, TEX_PARTICLE_CROSS2, nullptr, m_pTransformCom, 2.f);
	m_pGameInstance->Play_Particle(TEXT("Cross3"), Get_ParticleTag(TEXT("Cross3")), PARTICLE_BLINK_SHRINK, TEX_PARTICLE_CROSS2, nullptr, m_pTransformCom, 2.f);
	m_pGameInstance->Play_Particle(TEXT("Cross4"), Get_ParticleTag(TEXT("Cross4")), PARTICLE_BLINK_SHRINK, TEX_PARTICLE_CROSS2, nullptr, m_pTransformCom, 2.f);
	m_pGameInstance->Play_Particle(TEXT("Circle_Star"), Get_ParticleTag(TEXT("Circle_Star")), PARTICLE_BLINK_SHRINK, TEX_PARTICLE_STAR, nullptr, m_pTransformCom, 1.f);
	m_pGameInstance->Play_Particle(TEXT("Circle_Star"), Get_ParticleTag(TEXT("Circle_Star")), PARTICLE_STAR_COLOR, TEX_PARTICLE_STAR, nullptr, m_pTransformCom, 1.f);
	m_pGameInstance->Play_Particle(TEXT("Circle_Star1"), Get_ParticleTag(TEXT("Circle_Star1")), PARTICLE_STAR_COLOR, TEX_PARTICLE_STAR, nullptr, m_pTransformCom, 4.f);
	m_pGameInstance->Play_Particle(TEXT("Circle_Star1"), Get_ParticleTag(TEXT("Circle_Star1")), PARTICLE_DEFAULT, TEX_PARTICLE_BUBBLE, nullptr, m_pTransformCom, 4.5f);
	m_pGameInstance->Play_Particle(TEXT("Water_Fog"), Get_ParticleTag(TEXT("Water_Fog")), PARTICLE_WATER_FOG, TEX_PARTICLE_RAIN_DROP, nullptr, m_pTransformCom, 3.5f);

	m_pTransformCom->Rotation({ 1.f,0.f,0.f,0.f }, XMConvertToRadians(90.f));
	m_pTransformCom->Set_Scale(XMVectorSet(10.f,10.f,1.f,0.f));

	m_fLifeTime = 3.6f;

	return S_OK;
}

HRESULT CBarbara_Burst_Circle::PreviousTick(_float fTimeDelta)
{

	return S_OK;
}

HRESULT CBarbara_Burst_Circle::Tick(_float fTimeDelta)
{	
	m_fLifeTime -= fTimeDelta;
	if (m_fLifeTime <= 0.f)
	{
		Set_Destroy(TRUE);
		Set_Dead(TRUE);
	}

	

	m_fTimeAcc += fTimeDelta;
	m_fColorRate0 += log(m_fTimeAcc / 10 + 1.f) * 4.f;

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CBarbara_Burst_Circle::LateTick(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix());

	//m_pTransformCom->Look_At_Position(XMLoadFloat4(&m_pGameInstance->Get_CamPosition()));

	m_pRendererCom->Add_RenderGroup(CRenderer::RG_BLEND, this);

	return S_OK;
}

HRESULT CBarbara_Burst_Circle::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
			return E_FAIL;

		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
			return E_FAIL;

		if (FAILED(m_pNoiseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 2)))
			return E_FAIL;

		if (FAILED(m_pNoiseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture2", 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_ColorRate", &m_fTimeAcc, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(VTXTEX_EFF_BARBARA_BURST_CIRCLE)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;

	return S_OK;
}

CBarbara_Burst_Circle* CBarbara_Burst_Circle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBarbara_Burst_Circle* pInstance = new CBarbara_Burst_Circle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CBarbara_Burst_Circle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBarbara_Burst_Circle::Clone(void* pArg)
{
	CBarbara_Burst_Circle* pInstance = new CBarbara_Burst_Circle(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CBarbara_Burst_Circle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBarbara_Burst_Circle::Free()
{
	__super::Free();

	if (m_bCloned == true)
	{
		Safe_Release(m_pShaderCom);
		Safe_Release(m_pTextureCom);
		Safe_Release(m_pVIBufferCom);
		Safe_Release(m_pNoiseTextureCom);
	}
}