#include "stdafx.h"
#include "BabaraBasicAttack.h"

#include "Player.h"
#include "PartObject.h"

#include "Eff_Impact_Glow.h"
#include "Effect.h"
#include "GameInstance.h"

CBabaraBasicAttack::CBabaraBasicAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkillObj(pDevice, pContext)
{
}

CBabaraBasicAttack::CBabaraBasicAttack(const CBabaraBasicAttack& rhs)
	: CSkillObj(rhs)
{
}

HRESULT CBabaraBasicAttack::Add_Component(void* pArg)
{
	/* For.Com_Renderer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBabaraBasicAttack::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CBabaraBasicAttack::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBabaraBasicAttack::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(CBabaraBasicAttack::Add_Component(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		BARBARA_ATK_DESC* pDesc = (BARBARA_ATK_DESC*)pArg;
		m_fDamage = pDesc->tSkillDesc.fDamage;
		m_eType = pDesc->eBarbaraAtkType;

		/* For.Com_Collider_SPHERE */
		CBounding_SPHERE::BOUNDING_SPHERE_DESC tBoundingDesc;
		tBoundingDesc.fRadius = pDesc->tSkillDesc.vScale.m128_f32[0];
		tBoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);
		if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
			TEXT("Com_Collider_SPHERE"), (CComponent**)&m_pColliderCom, &tBoundingDesc)))
			return E_FAIL;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->tSkillDesc.vSpawnPos);
		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());


		if (pDesc->eBarbaraAtkType == BARBARA_ATK_FORCE)
		{
			m_pGameInstance->PlaySoundW(L"Barbara_ExtraAttack.mp3");

			CEffect::EFFECT_DESC descc = {};
			descc.eEffectID = EFFECT_IMPACT_WATER_SPLASH;
			descc.fLifeTime = 1.f;
			_vector vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			/*vSelfPos.m128_f32[2] += 2.f;
			vSelfPos.m128_f32[1] += 1.5f;*/
			descc.vSpawnPos = vSelfPos;
			descc.vScale = { 2.4f, 2.4f, 1.f, 0.f };

			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Liquid_Splash", L"Layer_Effect", &descc)))
				return E_FAIL;

			m_pGameInstance->Play_Particle(TEXT("Circle_Star1"), Get_ParticleTag(TEXT("Circle_Star1")), PARTICLE_DEFAULT, TEX_PARTICLE_BUBBLE, nullptr, m_pTransformCom, 0.5f);
			m_pGameInstance->Play_Particle(TEXT("Circle_Star1"), Get_ParticleTag(TEXT("Circle_Star1")), PARTICLE_DEFAULT, TEX_PARTICLE_BUBBLE, nullptr, m_pTransformCom, 0.5f);

			CImpact_Glow::IMPACT_DESC desc = {};
			desc.eEffectID = EFFECT_IMPACT_PLANE;
			vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			desc.pParent = (CGameObject*)this;
			desc.vSpawnPos = vSelfPos;
			desc.vScale = { 3.f, 3.f, 1.f, 1.f };
			desc.fRadius = { 1.f };
			desc.iNumSpike = { 8 };
			desc.iFanSize = { 75 };
			desc.eELEMENT = ELEMENT_FROST;

			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Impact_Glow", L"Layer_Effect", &desc)))
				return E_FAIL;


			CEffect::EFFECT_DESC tDesc = {};
			tDesc.eEffectID = EFFECT_ETC;
			tDesc.fLifeTime = 1.f;
			tDesc.vDir = pDesc->tSkillDesc.vDir;
			vSelfPos.m128_f32[1] += 1.f;
			tDesc.vSpawnPos = vSelfPos;
			tDesc.vScale = { 1.f, 1.f, 1.f, 0.f };

			m_fAttackTime = 0.f;
			if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_STATIC, L"Prototype_GameObject_Effect_Barbara_Normal_Atk", L"Layer_Effect", &tDesc)))
				return E_FAIL;
		}
		else
		{
			CEffect::EFFECT_DESC tDesc = {};
			tDesc.eEffectID = EFFECT_ETC;
			tDesc.fLifeTime = 1.f;
			_vector vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			tDesc.vDir = pDesc->tSkillDesc.vDir;
			tDesc.vSpawnPos = vSelfPos;
			tDesc.vScale = { 1.f, 1.f, 1.f, 0.f };

			if (pDesc->eBarbaraAtkType == BARBARA_ATK_BASE)
			{
				m_fAttackTime = 0.f;
				if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_STATIC, L"Prototype_GameObject_Effect_Barbara_Normal_Atk", L"Layer_Effect", &tDesc)))
					return E_FAIL;
				m_pGameInstance->PlaySoundW(L"Barbara_B_Attack4.mp3");
			}
			else if (pDesc->eBarbaraAtkType == BARBARA_ATK_BASE_F)
			{
				m_fAttackTime = 0.5f;
				if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_STATIC, L"Prototype_GameObject_Effect_Barbara_Normal_Star", L"Layer_Effect", &tDesc)))
					return E_FAIL;
			}
		}
		
	}
	else
		return E_FAIL;


	return S_OK;
}

HRESULT CBabaraBasicAttack::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CBabaraBasicAttack::Tick(_float fTimeDelta)
{
	m_fLifeTime += fTimeDelta;
	if(m_fLifeTime > 2.f)
		Set_Destroy(TRUE);

	return S_OK;
}

HRESULT CBabaraBasicAttack::LateTick(_float fTimeDelta)
{
	if (m_fAttackTime <= m_fLifeTime)
	{
		if (m_bAttack == FALSE)
		{
			m_bAttack = TRUE;
			_bool isColl = FALSE;
			vector<CGameObject*> HitMonsters = m_pGameInstance->Find_Collision_Objects(m_pColliderCom, COLL_MONSTER_BODY);
			for (auto& Hit : HitMonsters)
			{
				static_cast<CPartObject*>(Hit)->Set_Damage(m_fDamage, ELEMENT_WATER);
				isColl = TRUE;
			}

			if (isColl == TRUE && m_pGameInstance->IsProgress_StatusEffect(SE_BARABRA_ES, L"Barbara") == TRUE)
			{
				CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Player"));
				if(m_eType == BARBARA_ATK_FORCE)
					pPlayer->Set_AllRatioHeal(0.06f, (_float)m_pGameInstance->Get_Int_Random(416, 684));
				else
					pPlayer->Set_AllRatioHeal(0.015f, (_float)m_pGameInstance->Get_Int_Random(104, 171));
				
			}
		}
	}

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif

	return S_OK;
}

HRESULT CBabaraBasicAttack::Render()
{
	return S_OK;
}

CBabaraBasicAttack* CBabaraBasicAttack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBabaraBasicAttack* pInstance = new CBabaraBasicAttack(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CBabaraBasicAttack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBabaraBasicAttack::Clone(void* pArg)
{
	CBabaraBasicAttack* pInstance = new CBabaraBasicAttack(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CBabaraBasicAttack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBabaraBasicAttack::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);
}
