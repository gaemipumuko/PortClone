#include "stdafx.h"
#include "Status_Raiden.h"

#include "GameInstance.h"

CStatus_Raiden::CStatus_Raiden(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart_Status(pDevice, pContext)
{
}

CStatus_Raiden::CStatus_Raiden(const CStatus_Raiden& rhs)
	: CPart_Status(rhs)
{
}

HRESULT CStatus_Raiden::Set_Status()
{
	/* 현재, 최대, 증가율, 돌파증가율 */
	m_EntityStatus.eLevelUp_Step = STEP_ZERO;
	m_EntityStatus.eAtk_Type = ATK_END;
	m_EntityStatus.fExp = { 0.f, 1000.f, 0.f, 0.f, };
	m_EntityStatus.fLevel = { 1.f, 90.f, 0.f, 0.f };
	m_EntityStatus.fHp = { 12907.f, 12907.f, 134.f, 10.f };
	m_EntityStatus.fAtk = { 337.f, 0.f, 4.f, 10.f };
	m_EntityStatus.fDef = { 61.f, 0.f, 8.f, 10.f };
	m_EntityStatus.fEm = { 0.f, 1000.f, 0.f, 0.f };
	m_EntityStatus.fCr = { 50.f, 100.f, 0.f, 5.f };
	m_EntityStatus.fCd = { 150.f, 300.f, 0.f, 0.f };
	m_EntityStatus.fEr = { 100.f, 0.f, 0.f, 0.f };
	m_EntityStatus.fElementalDmg = { 0.f, 0.f, 0.f, 0.f };
	m_EntityStatus.fRes = { 0.f, 0.f, 0.f, 0.f };
	m_EntityStatus.fHealing_Bonus = { 0.f, 0.f, 0.f, 0.f };
	m_EntityStatus.fReuseTime_reduction = { 0.f, 0.f, 0.f, 0.f }; 
	m_EntityStatus.fShield_Strength = { 0.f, 0.f, 0.f, 0.f };

	m_EntityStatus.fConstellation = 0.f;

	m_EntityStatus.fReuseTime = { 0.f, 0.f, 10.f, 18.f }; /* 원소스킬 대기시간, 원소폭발 대기시간 */
	m_EntityStatus.fCur_Energy = 0.f;
	m_EntityStatus.fElemental_Energy = 90.f;
	m_EntityStatus.bElemental_Skill = FALSE;
	m_EntityStatus.bElemental_Burst = FALSE;

	m_EntityStatus.bLevelUp = FALSE;
	m_EntityStatus.bLevel_BreakThrough = FALSE;

	m_EntityStatus.fStamina = FALSE;

	return S_OK;
}

void CStatus_Raiden::Level_BreakThrough()
{
}

HRESULT CStatus_Raiden::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStatus_Raiden::Initialize_Clone(void* pArg)
{
	PARTSTATUS_DESC* pDesc = static_cast<PARTSTATUS_DESC*>(pArg);

	if (FAILED(__super::Initialize_Clone(&pDesc->tPartObjDesc)))
		return E_FAIL;

	if (FAILED(Set_Status()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStatus_Raiden::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CStatus_Raiden::Tick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CStatus_Raiden::LateTick(_float fTimeDelta)
{
	return S_OK;
}

CStatus_Raiden* CStatus_Raiden::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStatus_Raiden* pInstance = new CStatus_Raiden(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CStatus_Raiden"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CStatus_Raiden::Clone(void* pArg)
{
	CStatus_Raiden* pInstance = new CStatus_Raiden(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CStatus_Raiden"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatus_Raiden::Free()
{
	__super::Free();
}