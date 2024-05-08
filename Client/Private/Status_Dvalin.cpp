#include "stdafx.h"
#include "Status_Dvalin.h"

#include "GameInstance.h"

CStatus_Dvalin::CStatus_Dvalin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart_Status(pDevice, pContext)
{
}

CStatus_Dvalin::CStatus_Dvalin(const CStatus_Dvalin& rhs)
	: CPart_Status(rhs)
{
}

HRESULT CStatus_Dvalin::Set_Status()
{
	/* 현재, 최대, 증가율, 돌파증가율 */
	m_EntityStatus.eLevelUp_Step	= STEP_ZERO;
	m_EntityStatus.eAtk_Type		= ATK_END;
	m_EntityStatus.fExp				= { 0.f, 0.f, 0.f, 0.f, };
	m_EntityStatus.fLevel			= { 90.f, 0.f, 0.f, 0.f };
	m_EntityStatus.fHp				= { 219187.f, 219187.f, 0.f, 0.f };
	m_EntityStatus.fAtk				= { 11419.f, 0.f, 0.f, 0.f };
	m_EntityStatus.fDef				= { 950.f, 0.f, 0.f, 0.f };
	m_EntityStatus.fEm				= { 0.f, 0.f, 0.f, 0.f };
	m_EntityStatus.fCr				= { 0.f, 0.f, 0.f, 0.f };
	m_EntityStatus.fCd				= { 0.f, 0.f, 0.f, 0.f };
	m_EntityStatus.fEr				= { 0.f, 0.f, 0.f, 0.f };
	m_EntityStatus.fElementalDmg	= { 0.f, 0.f, 0.f, 0.f };
	m_EntityStatus.fRes				= { 0.f, 0.f, 0.f, 0.f };
	m_EntityStatus.fHealing_Bonus	= { 0.f, 0.f, 0.f, 0.f };
	m_EntityStatus.fReuseTime_reduction = { 0.f, 0.f, 0.f, 0.f };
	m_EntityStatus.fShield_Strength = { 0.f, 0.f, 0.f, 0.f };

	m_EntityStatus.fConstellation = 0.f;
	
	m_EntityStatus.fReuseTime = { 0.f, 0.f, 0.f, 0.f };
	m_EntityStatus.fCur_Energy = 0.f;
	m_EntityStatus.fElemental_Energy = 00.f;
	m_EntityStatus.bElemental_Skill = FALSE;
	m_EntityStatus.bElemental_Burst = FALSE;

	m_EntityStatus.bLevelUp = FALSE;
	m_EntityStatus.bLevel_BreakThrough = FALSE;

	return S_OK;
}

void CStatus_Dvalin::Level_BreakThrough()
{
}

HRESULT CStatus_Dvalin::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStatus_Dvalin::Initialize_Clone(void* pArg)
{
	PARTSTATUS_DESC* pDesc = static_cast<PARTSTATUS_DESC*>(pArg);

	if (FAILED(__super::Initialize_Clone(&pDesc->tPartObjDesc)))
		return E_FAIL;

	if (FAILED(Set_Status()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStatus_Dvalin::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CStatus_Dvalin::Tick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CStatus_Dvalin::LateTick(_float fTimeDelta)
{
	return S_OK;
}

CStatus_Dvalin* CStatus_Dvalin::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStatus_Dvalin* pInstance = new CStatus_Dvalin(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CStatus_Dvalin"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CStatus_Dvalin::Clone(void* pArg)
{
	CStatus_Dvalin* pInstance = new CStatus_Dvalin(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CStatus_Dvalin"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatus_Dvalin::Free()
{
	__super::Free();
}