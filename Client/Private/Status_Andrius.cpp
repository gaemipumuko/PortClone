#include "stdafx.h"
#include "Status_Andrius.h"

#include "GameInstance.h"

CStatus_Andrius::CStatus_Andrius(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart_Status(pDevice, pContext)
{
}

CStatus_Andrius::CStatus_Andrius(const CStatus_Andrius& rhs)
	: CPart_Status(rhs)
{
}

HRESULT CStatus_Andrius::Set_Status()
{
	/* 현재, 최대, 증가율, 돌파증가율 */
	m_EntityStatus.eLevelUp_Step	= STEP_ZERO;
	m_EntityStatus.eAtk_Type		= ATK_END;
	m_EntityStatus.fExp				= { 0.f, 0.f, 0.f, 0.f, };
	m_EntityStatus.fLevel			= { 50.f, 0.f, 0.f, 0.f };
	m_EntityStatus.fHp				= { 8993700.f, 8993700.f, 0.f, 0.f };
	m_EntityStatus.fAtk				= { 4535.f, 0.f, 0.f, 0.f };
	m_EntityStatus.fDef				= { 750.f, 0.f, 0.f, 0.f };
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

void CStatus_Andrius::Level_BreakThrough()
{
}

HRESULT CStatus_Andrius::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStatus_Andrius::Initialize_Clone(void* pArg)
{
	PARTSTATUS_DESC* pDesc = static_cast<PARTSTATUS_DESC*>(pArg);

	if (FAILED(__super::Initialize_Clone(&pDesc->tPartObjDesc)))
		return E_FAIL;

	if (FAILED(Set_Status()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStatus_Andrius::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CStatus_Andrius::Tick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CStatus_Andrius::LateTick(_float fTimeDelta)
{
	return S_OK;
}

CStatus_Andrius* CStatus_Andrius::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStatus_Andrius* pInstance = new CStatus_Andrius(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CStatus_Andrius"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CStatus_Andrius::Clone(void* pArg)
{
	CStatus_Andrius* pInstance = new CStatus_Andrius(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CStatus_Andrius"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatus_Andrius::Free()
{
	__super::Free();
}