#include "stdafx.h"
#include "Raiden_ElementAttack.h"

#include "Player.h"
#include "Part_Body.h"

#include "Eff_Impact_Glow.h"
#include "Effect.h"
#include "GameInstance.h"

CRaiden_ElementAttack::CRaiden_ElementAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkillObj(pDevice, pContext)
{
}

CRaiden_ElementAttack::CRaiden_ElementAttack(const CRaiden_ElementAttack& rhs)
	: CSkillObj(rhs)
{
}

HRESULT CRaiden_ElementAttack::Add_Component(void* pArg)
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

HRESULT CRaiden_ElementAttack::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CRaiden_ElementAttack::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRaiden_ElementAttack::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(CRaiden_ElementAttack::Add_Component(pArg)))
		return E_FAIL;

	if (pArg == nullptr)
		return E_FAIL;

	RAIDEN_SKILL_DESC* pDesc = (RAIDEN_SKILL_DESC*)pArg;
	m_eType = pDesc->eRaidenSkillType;

	/* For.Com_Collider_SPHERE */
	CBounding_SPHERE::BOUNDING_SPHERE_DESC tBoundingDesc;
	tBoundingDesc.fRadius = pDesc->tSkillDesc.vScale.m128_f32[0];
	tBoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
		TEXT("Com_Collider_SPHERE"), (CComponent**)&m_pColliderCom, &tBoundingDesc)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->tSkillDesc.vSpawnPos);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CRaiden_ElementAttack::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CRaiden_ElementAttack::Tick(_float fTimeDelta)
{
	m_fLifeTime += fTimeDelta;
	if (m_fLifeTime > 2.f)
		Set_Destroy(TRUE);

	return S_OK;
}

HRESULT CRaiden_ElementAttack::LateTick(_float fTimeDelta)
{
	if (m_bAttack == FALSE)
	{
		m_bAttack = TRUE;
		vector<CGameObject*> HitMonsters = m_pGameInstance->Find_Collision_Objects(m_pColliderCom, COLL_MONSTER_BODY);
		CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Player"));
		for (auto& Hit : HitMonsters)
		{
			if(m_eType == RAIDEN_ELEMENTAL_SKILL)
				static_cast<CPart_Body*>(Hit)->Set_Damage(pPlayer->Get_ElementDamage(), ELEMENT_ELECTRIC);
			else if(m_eType == RAIDEN_ELEMENTAL_BURST)
				static_cast<CPart_Body*>(Hit)->Set_Damage(pPlayer->Get_BurstDamage(), ELEMENT_ELECTRIC);		}
	}

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif

	return S_OK;
}

HRESULT CRaiden_ElementAttack::Render()
{
	return S_OK;
}

CRaiden_ElementAttack* CRaiden_ElementAttack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRaiden_ElementAttack* pInstance = new CRaiden_ElementAttack(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CRaiden_ElementAttack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRaiden_ElementAttack::Clone(void* pArg)
{
	CRaiden_ElementAttack* pInstance = new CRaiden_ElementAttack(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CRaiden_ElementAttack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRaiden_ElementAttack::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);
}
