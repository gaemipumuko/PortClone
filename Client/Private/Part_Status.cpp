#include "stdafx.h"
#include "Part_Status.h"

#include "GameInstance.h"

CPart_Status::CPart_Status(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{
}

CPart_Status::CPart_Status(const CPart_Status& rhs)
	: CPartObject(rhs)
{
}

_bool CPart_Status::Can_LevelUp()
{
	if(m_EntityStatus.bLevel_BreakThrough  == FALSE && (m_EntityStatus.fLevel.m128_f32[0] == 20.f 
		|| m_EntityStatus.fLevel.m128_f32[0] == 40.f || m_EntityStatus.fLevel.m128_f32[0] == 50.f 
		|| m_EntityStatus.fLevel.m128_f32[0] == 60.f || m_EntityStatus.fLevel.m128_f32[0] == 70.f 
		|| m_EntityStatus.fLevel.m128_f32[0] == 80.f || m_EntityStatus.fLevel.m128_f32[0] == m_EntityStatus.fLevel.m128_f32[1]))
		return FALSE;
	else
		return TRUE;
}

void CPart_Status::Level_Up(_float fExp)
{
	_float fAddExp = fExp;

	while (fAddExp > 0.f)
	{
		if (m_EntityStatus.fLevel.m128_f32[0] == 90.f && m_EntityStatus.fExp.m128_f32[0] == m_EntityStatus.fExp.m128_f32[1])
			break;

		if (m_bSelectMaxExp != TRUE)
		{
			if (m_EntityStatus.fLevel.m128_f32[0] == 1.f) // 레벨1 최대경험치
				m_EntityStatus.fExp.m128_f32[1] = 1000.f;
			else
				m_EntityStatus.fExp.m128_f32[1] *= (1 + (rand() % 10 + 20.5f) / 100.f);

			m_bSelectMaxExp = TRUE;
		}

		_float fLevelUpExp = m_EntityStatus.fExp.m128_f32[1];
		_float fAfterExp = m_EntityStatus.fExp.m128_f32[0] + fAddExp;

		if (fAfterExp >= fLevelUpExp) // 다음레벨로가기위한 경험치보다 들어온게 더 많을 경우
		{
			fAddExp -= (fLevelUpExp - m_EntityStatus.fExp.m128_f32[0]);

			if (m_EntityStatus.fLevel.m128_f32[0] == m_EntityStatus.fLevel.m128_f32[1] ||
				(m_EntityStatus.bLevel_BreakThrough == FALSE && (m_EntityStatus.fLevel.m128_f32[0] == 20.f
					|| m_EntityStatus.fLevel.m128_f32[0] == 40.f || m_EntityStatus.fLevel.m128_f32[0] == 50.f
					|| m_EntityStatus.fLevel.m128_f32[0] == 60.f || m_EntityStatus.fLevel.m128_f32[0] == 70.f
					|| m_EntityStatus.fLevel.m128_f32[0] == 80.f)))
			{
				m_EntityStatus.fExp.m128_f32[0] = fLevelUpExp;
				m_EntityStatus.bLevelUp = FALSE;
			}
			else
			{
				m_EntityStatus.fExp.m128_f32[0] = 0.f;
				m_EntityStatus.fLevel.m128_f32[0] += 1.f; // 레벨업

				/* 스탯증가 */
				m_EntityStatus.fHp.m128_f32[1] += m_EntityStatus.fHp.m128_f32[2]; // 경보 수정
				m_EntityStatus.fHp.m128_f32[0] = m_EntityStatus.fHp.m128_f32[1]; // 경보 수정
				m_EntityStatus.fAtk.m128_f32[0] += m_EntityStatus.fAtk.m128_f32[2];
				m_EntityStatus.fDef.m128_f32[0] += m_EntityStatus.fDef.m128_f32[2];

				m_bSelectMaxExp = FALSE;
				m_EntityStatus.bLevelUp = TRUE;
			}
		}
		else // 채워야할 경험치가 더 클경우
		{
			m_EntityStatus.fExp.m128_f32[0] += fAddExp;
			fAddExp = 0.f;
			m_EntityStatus.bLevelUp = FALSE;
		}
	}
}

_float CPart_Status::Get_Attack(ATK_TYPE eATk)
{
	_float fCurrentAtk = { };

	switch (eATk)
	{
	case ATK_NORMAL:
		fCurrentAtk = m_EntityStatus.fAtk.m128_f32[0] * (rand() % 5 + 1.f);
	break;
	case ATK_CHARGED:
		fCurrentAtk = m_EntityStatus.fAtk.m128_f32[0] * (rand() % 5 + 3.f);
		break;
	case ATK_ELEMENTAL:
	{
		fCurrentAtk = m_EntityStatus.fAtk.m128_f32[0] * (1 + m_EntityStatus.fElementalDmg.m128_f32[0] / 100.f);
		fCurrentAtk *= 1.f + (rand() % 3 + 0.9f);

		if ((rand() % 100) / 100.f <= m_EntityStatus.fCr.m128_f32[0] / 100.f)
			fCurrentAtk *= (1 + m_EntityStatus.fCd.m128_f32[0] / 100.f);
	}
	break;
	case ATK_BURST:
	{
		fCurrentAtk = m_EntityStatus.fAtk.m128_f32[0] * (1 + m_EntityStatus.fElementalDmg.m128_f32[0] / 100.f);
		fCurrentAtk *= 1.f + (rand() % 3 + 1.8f);

		if ((rand() % 100) / 100.f <= m_EntityStatus.fCr.m128_f32[0] / 100.f)
			fCurrentAtk *= (1 + m_EntityStatus.fCd.m128_f32[0] / 100.f);
	}
	break;
	}

	return fCurrentAtk;
}

void CPart_Status::Set_Damage(_float fDamage)
{
	if (m_EntityStatus.fShield_Strength.m128_f32[0] > 0.f)
	{
		_float fDamage_Absorb = m_EntityStatus.fShield_Strength.m128_f32[0] / 100.f;
		fDamage_Absorb = fDamage * fDamage_Absorb;

		_float fCurrentDamage = fDamage - fDamage_Absorb;
		fCurrentDamage -= m_EntityStatus.fDef.m128_f32[0];

		m_EntityStatus.fHp.m128_f32[0] = max(0.f, m_EntityStatus.fHp.m128_f32[0] - max(0.f, fCurrentDamage));
		
	}
	else
	{
		_float fCurrentDamage = fDamage - m_EntityStatus.fDef.m128_f32[0];
		m_EntityStatus.fHp.m128_f32[0] = max(0.f, m_EntityStatus.fHp.m128_f32[0] - max(0.f, fCurrentDamage));
	}
}

void CPart_Status::Set_Heal(_float fHeal)
{
	m_EntityStatus.fHp.m128_f32[0] += fHeal;
	if (m_EntityStatus.fHp.m128_f32[0] > m_EntityStatus.fHp.m128_f32[1])
		m_EntityStatus.fHp.m128_f32[0] = m_EntityStatus.fHp.m128_f32[1];

}

_float CPart_Status::Set_HpRatioHeal(_float fHpRatio, _float fAddHeal)
{
	_float fHeal = m_EntityStatus.fHp.m128_f32[1] * fHpRatio + fAddHeal;
	m_EntityStatus.fHp.m128_f32[0] += fHeal;
	if (m_EntityStatus.fHp.m128_f32[0] > m_EntityStatus.fHp.m128_f32[1])
		m_EntityStatus.fHp.m128_f32[0] = m_EntityStatus.fHp.m128_f32[1];

	return fHeal;
}

void CPart_Status::Set_MaxHP()
{
	m_EntityStatus.fHp.m128_f32[0] = m_EntityStatus.fHp.m128_f32[1];
}

HRESULT CPart_Status::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPart_Status::Initialize_Clone(void* pArg)
{
	PARTSTATUS_DESC* pDesc = static_cast<PARTSTATUS_DESC*>(pArg);

	if (FAILED(__super::Initialize_Clone(&pDesc->tPartObjDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPart_Status::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CPart_Status::Tick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CPart_Status::LateTick(_float fTimeDelta)
{
	return S_OK;
}

CPart_Status* CPart_Status::Create_Status(CREATUREID eID, PARTSTATUS_DESC tStatusDesc)
{
	CPart_Status* pStatus = { nullptr };

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	switch (eID)
	{
	case CHARACTER_DILUC:
		pStatus =
			static_cast<CPart_Status*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Status_Diluc"))->Clone(&tStatusDesc));
		break;
	case CHARACTER_ZHONGLI:
		pStatus =
			static_cast<CPart_Status*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Status_Zhongli"))->Clone(&tStatusDesc));
		break;
	case CHARACTER_BARBARA:
		pStatus =
			static_cast<CPart_Status*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Status_Barbara"))->Clone(&tStatusDesc));
		break;
	case CHARACTER_RAIDEN:
		pStatus =
			static_cast<CPart_Status*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Status_Raiden"))->Clone(&tStatusDesc));
		break;
	case MONSTER_ANDRIUS:
		pStatus = 
			static_cast<CPart_Status*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Status_Andrius"))->Clone(&tStatusDesc));
		break;
	case MONSTER_EFFIGY_FIRE:
		//MSG_BOX(TEXT("Status 까먹지 말고 만들어라 정인아"));
		//pStatus = 
		//	static_cast<CPart_Status*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Status_Andrius"))->Clone(&tStatusDesc));
		break;
	case MONSTER_DVALIN:
		pStatus = 
			static_cast<CPart_Status*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Status_Dvalin"))->Clone(&tStatusDesc));
		break;
	}

	RELEASE_INSTANCE(CGameInstance);

	return pStatus;
}

void CPart_Status::Free()
{
	__super::Free();
}