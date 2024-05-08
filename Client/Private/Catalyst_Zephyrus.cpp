#include "stdafx.h"
#include "Catalyst_Zephyrus.h"

#include "Part_Body.h"
#include "Player.h"

#include "GameInstance.h"

CCatalyst_Zephyrus::CCatalyst_Zephyrus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPart_Weapon(pDevice, pContext)
{
}

CCatalyst_Zephyrus::CCatalyst_Zephyrus(const CCatalyst_Zephyrus& rhs)
    : CPart_Weapon(rhs)
{
}

HRESULT CCatalyst_Zephyrus::Invalidate_Animation(_float fTimeDelta)
{
	_int iIndex = static_cast<_int>(static_cast<CPlayer*>(m_pParent)->Get_PlayerIndex(CHARACTER_BARBARA));
	CModel* pModel = static_cast<CModel*>(static_cast<CPlayer*>(m_pParent)->Get_PartObject(CPlayer::PART_BODY, iIndex)->Find_Component(TEXT("Com_Model")));

	FSM_STATE eState = static_cast<FSM_STATE>(m_pParentFSMCom->Get_CurrentState());
	
	if (eState == FSM_IDLE || eState == FSM_WALK || eState == FSM_SPLINT || eState == FSM_JUMP 
		|| eState == FSM_RUN || eState == FSM_DASH
		|| eState == FSM_WALK_RUN || eState == FSM_WALK_IDLE || eState == FSM_RUN_WALK || eState == FSM_RUN_STOP || eState == FSM_SPLINT_RUN
		|| eState == FSM_FLY_START || eState == FSM_FLY_NORMAL || eState == FSM_FLY_LEFT || eState == FSM_FLY_RIGHT
		|| eState == FSM_FALL_LOW || eState == FSM_FALL_HIGH || eState == FSM_DROP_DOWN || eState == FSM_ATTACK_END2 || eState == FSM_ATTACK_READY)
	{
		m_pModelCom->Set_Animation(0);

		if (m_bHand == TRUE)
		{
			m_bHand = FALSE;
			m_fDissolve = 1.f;
			m_pGameInstance->PlaySoundW(L"CreateWeapon.mp3", 1.f);
		}

		if (m_fDissolve > 0.f)
			m_fDissolve -= 0.02f;
		else
			m_fDissolve = 0.f;

		m_pModelCom->Set_PartRootBone("RootNode", pModel->Get_Bone("PRIVATE_WeaponRootCatalyst"));
		m_fAttackEndTime = 0.f;

		m_bMargin = FALSE;
	}
	else if (eState == FSM_ATTACK_END)
	{
		m_pModelCom->Set_Animation(1);

		m_fAttackEndTime += fTimeDelta;
		if (m_fAttackEndTime >= 1.3f && m_fAttackEndTime < 1.7f)
		{
			if (m_fDissolve < 1.f)
				m_fDissolve += 0.04f;
			else
				m_fDissolve = 1.f;
			m_pModelCom->Set_PartRootBone("RootNode", pModel->Get_Bone("CatalystL"));

			m_bMargin = TRUE;
		}
		//else if (m_fAttackEndTime >= 1.8f) /* 이펙트 효과 추가해야함*/
		//{
		//	m_pModelCom->Set_Animation(0);

		//	m_pModelCom->Set_PartRootBone("RootNode", pModel->Get_Bone("PRIVATE_WeaponRootCatalyst"));
		//	m_bMargin = FALSE;
		//}	
	}
	else
	{
		if(eState == FSM_ATTACK_IDLE)
			m_pModelCom->Set_Animation(1);
		else
			m_pModelCom->Set_Animation(2);

		m_bHand = TRUE;
		m_fDissolve = 0.f;
		m_pModelCom->Set_PartRootBone("RootNode", pModel->Get_Bone("CatalystL"));
		m_fAttackEndTime = 0.f;
		m_bMargin = TRUE;
	}

	return S_OK;
}

HRESULT CCatalyst_Zephyrus::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCatalyst_Zephyrus::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	/* For.Com_Collider_OBB */
	CBounding_OBB::BOUNDING_OBB_DESC		BoundingDesc = {};
	BoundingDesc.vExtents = _float3(1.f, 1.5f, 1.f);
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y - 2.f, -1.f);
	BoundingDesc.vRadians = _float3(XMConvertToRadians(90.f), 0.f, 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_OBB"), (CComponent**)&m_pColliderCom, &BoundingDesc)))
		return E_FAIL;

	m_pGameInstance->Add_Collider_Object(COLL_PLAYER_WEAPON, &m_iColliderID, m_pColliderCom, this);

	m_vDissolveColor = _float3(1.f, 1.f, 0.f);
	m_fDissolveSize = 0.2f;

	m_pModelCom->Set_Animation(0);

	return S_OK;
}

HRESULT CCatalyst_Zephyrus::PreviousTick(_float fTimeDelta)
{
	if (FAILED(__super::PreviousTick(fTimeDelta)))
		return E_FAIL;

    return S_OK;
}

HRESULT CCatalyst_Zephyrus::Tick(_float fTimeDelta)
{
	/*if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;*/

	if (m_bMargin)
		XMStoreFloat4x4(&m_CombinedWorldMatrix,
			m_pTransformCom->Get_WorldMatrix() *
			XMMatrixScaling(0.8f, 0.8f, 0.8f) *
			XMMatrixTranslation(-0.2f, 0.7f, 0.2f) *
			m_pParentTransformCom->Get_WorldMatrix());
	else
		XMStoreFloat4x4(&m_CombinedWorldMatrix,
			m_pTransformCom->Get_WorldMatrix() *
			m_pParentTransformCom->Get_WorldMatrix());

	m_pModelCom->Play_Animation(TRUE, m_pTransformCom, fTimeDelta);

	_int iIndex = static_cast<_int>(static_cast<CPlayer*>(m_pParent)->Get_PlayerIndex(CHARACTER_BARBARA));
	m_vRootTranslation = static_cast<CPlayer*>(m_pParent)->Get_PartObject(CPlayer::PART_BODY, iIndex)->Get_RootTranslation();

	m_pColliderCom->Update(
		XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix("RootNode")) *
		XMMatrixRotationAxis({ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(180.f)) *
		XMMatrixTranslation(m_vRootTranslation.m128_f32[0], 0.f, m_vRootTranslation.m128_f32[2]) *
		XMLoadFloat4x4(&m_CombinedWorldMatrix)
	);

	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		XMMatrixTranslation(m_vRootTranslation.m128_f32[0], 0.f, m_vRootTranslation.m128_f32[2]) * XMLoadFloat4x4(&m_CombinedWorldMatrix));


	Invalidate_Animation(fTimeDelta);

	m_pModelCom->Invalidate_CombinedTransformationMatrix();

	return S_OK;
}

HRESULT CCatalyst_Zephyrus::LateTick(_float fTimeDelta)
{
	if (FAILED(__super::LateTick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCatalyst_Zephyrus::Render()
{
	FSM_STATE eState = static_cast<FSM_STATE>(m_pParentFSMCom->Get_CurrentState());

	if (eState == FSM_HIT_THROW ||
		eState == FSM_HIT_THROW_FLY || eState == FSM_HIT_ROLLING || eState == FSM_SHOW)
		return S_OK;

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CCatalyst_Zephyrus* CCatalyst_Zephyrus::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCatalyst_Zephyrus* pInstance = new CCatalyst_Zephyrus(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CCatalyst_Zephyrus"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCatalyst_Zephyrus::Clone(void* pArg)
{
	CCatalyst_Zephyrus* pInstance = new CCatalyst_Zephyrus(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CCatalyst_Zephyrus"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCatalyst_Zephyrus::Free()
{
	__super::Free();
}
