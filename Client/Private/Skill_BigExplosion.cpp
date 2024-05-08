#include "stdafx.h"
#include "Skill_BigExplosion.h"
#include "Camera.h"
#include "FX_ShockWave.h"
#include "Eff_Impact_Glow.h"

#include "GameInstance.h"

CSKill_BigExplosion::CSKill_BigExplosion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkillObj(pDevice, pContext)
{
}

CSKill_BigExplosion::CSKill_BigExplosion(const CSKill_BigExplosion& rhs)
	: CSkillObj(rhs)
{
}

HRESULT CSKill_BigExplosion::Add_Component(void* pArg)
{
	/* For.Com_Renderer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Sphere"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSKill_BigExplosion::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &m_fTimeAcc, sizeof(_float))))
		return E_FAIL;

	_float fCamFar = static_cast<CCamera*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"))->Get_CamFar();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &fCamFar, sizeof(_float))))
		return E_FAIL;

	_float4 vCamPos = m_pGameInstance->Get_CamPosition();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &vCamPos, sizeof(_float4))))
		return E_FAIL;
	
	_float fFloorHeight = 0.1f;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFloorHeight", &fFloorHeight, sizeof(_float))))
		return E_FAIL;
	return S_OK;
}

HRESULT CSKill_BigExplosion::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSKill_BigExplosion::Initialize_Clone(void* pArg)
{
	if (FAILED(CSKill_BigExplosion::Add_Component(pArg)))
		return E_FAIL;

	if (pArg == nullptr)
		return E_FAIL;

	SKILLOBJ_DESC* pDesc = (SKILLOBJ_DESC*)pArg;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vSpawnPos);

	m_pTransformCom->Set_Scale({ 0.0015f, 0.0015f, 0.0015f, 0.f });
	m_fLifeTime = 0.2f;


	CImpact_Glow::IMPACT_DESC descc = {};
	descc.eEffectID = EFFECT_IMPACT_RADIAL;
	_vector vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	descc.eELEMENT = ELEMENT_FIRE;
	descc.pParent = (CGameObject*)this;
	vSelfPos.m128_f32[1] += 0.5f;
	descc.vSpawnPos = vSelfPos;
	descc.vScale = { 6.f, 9.f, 1.f, 1.f };

	if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Impact_Glow", L"Layer_Effect", &descc)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Impact_Glow", L"Layer_Effect", &descc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSKill_BigExplosion::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CSKill_BigExplosion::Tick(_float fTimeDelta)
{
	m_fLifeTime -= fTimeDelta;
	if (m_fLifeTime <= 0.f)
	{
		if (m_iExplosion == 0)
		{
			m_fLifeTime = 0.001f;
			m_iExplosion = 1;


			CFX_ShockWave::SW_DESC tSWDesc = { };
			tSWDesc.fScale = 4.5f;
			tSWDesc.ePosType = CAssetFX::ASFX_DESC::POS_FIX;
			tSWDesc.vInitPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			tSWDesc.fDuration = 0.5f;

			m_pGameInstance->Play_AssetFX(L"ShockWave", Get_ParticleTag(L"ShockWave"), &tSWDesc);
		}
		else if (m_iExplosion == 1)
		{
			m_fLifeTime = 0.001f;
			m_iExplosion = 1;

			/*m_pGameInstance->Play_Particle(TEXT("Dust0"), Get_ParticleTag(TEXT("Dust0")), PARTICLE_DUST, TEX_PARTICLE_DUST, nullptr, m_pTransformCom, 1.f);
			m_pGameInstance->Play_Particle(TEXT("Dust1"), Get_ParticleTag(TEXT("Dust1")), PARTICLE_DUST, TEX_PARTICLE_DUST, nullptr, m_pTransformCom, 1.f);
			m_pGameInstance->Play_Particle(TEXT("RockShard3"), Get_ParticleTag(TEXT("RockShard3")), PARTICLE_SOLID_DEBRIS, TEX_PARTICLE_DEBRIS_AT, nullptr, m_pTransformCom, 1.f);
			m_pGameInstance->Play_Particle(TEXT("RockShard4"), Get_ParticleTag(TEXT("RockShard4")), PARTICLE_SOLID_DEBRIS, TEX_PARTICLE_DEBRIS_AT, nullptr, m_pTransformCom, 1.f);
			m_pGameInstance->Play_Particle(TEXT("RockShard5"), Get_ParticleTag(TEXT("RockShard5")), PARTICLE_SOLID_DEBRIS, TEX_PARTICLE_DEBRIS_AT, nullptr, m_pTransformCom, 1.f);*/


			CFX_ShockWave::SW_DESC tSWDesc = { };
			tSWDesc.fScale = 4.5f;
			tSWDesc.ePosType = CAssetFX::ASFX_DESC::POS_FIX;
			tSWDesc.vInitPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			tSWDesc.fDuration = 0.5f;

			m_pGameInstance->Play_AssetFX(L"ShockWave", Get_ParticleTag(L"ShockWave"), &tSWDesc);

			m_fLifeTime = 0.5f;
			m_iExplosion = 2;
			m_bShockWave = TRUE;
		}
		else
			Set_Destroy(TRUE);
	}

	if (m_bShockWave == TRUE)
	{
		m_fTimeAcc += fTimeDelta * 0.6f;
		m_pTransformCom->Set_Scale({ 0.0015f + m_fTimeAcc, 0.0015f + m_fTimeAcc, 0.0015f + m_fTimeAcc, 0.f });
	}

	return S_OK;
}

HRESULT CSKill_BigExplosion::LateTick(_float fTimeDelta)
{
	if (m_bShockWave == FALSE)
		S_OK;

	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);

	return S_OK;
}

HRESULT CSKill_BigExplosion::Render()
{
	if (FAILED(CSKill_BigExplosion::Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pShaderCom->Begin(VTXMESH_BIG_EXPLOSION)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}


CSKill_BigExplosion* CSKill_BigExplosion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSKill_BigExplosion* pInstance = new CSKill_BigExplosion(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CSKill_BigExplosion"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSKill_BigExplosion::Clone(void* pArg)
{
	CSKill_BigExplosion* pInstance = new CSKill_BigExplosion(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CSKill_BigExplosion"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSKill_BigExplosion::Free()
{
	__super::Free();

	if (m_bCloned)
	{
		Safe_Release(m_pTransformCom);
		Safe_Release(m_pModelCom);
		Safe_Release(m_pShaderCom);
		Safe_Release(m_pRendererCom);
	}
}