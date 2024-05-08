#include "stdafx.h"
#include "Glider_Liyue.h"

#include "Part_Body.h"

#include "GameInstance.h"

CGlider_Liyue::CGlider_Liyue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPart_Glider(pDevice, pContext)
{
}

CGlider_Liyue::CGlider_Liyue(const CGlider_Liyue& rhs)
    : CPart_Glider(rhs)
{
}

HRESULT CGlider_Liyue::Invalidate_Animation(_float fTimeDelta)
{
	FSM_STATE eState = static_cast<FSM_STATE>(m_pParentFSMCom->Get_CurrentState());

	if (eState == FSM_FLY_START)
	{
		m_pModelCom->Set_Animation(0);
		m_pModelCom->Play_Animation(FALSE, m_pTransformCom, fTimeDelta * 1.2f);
	}
	else
	{
		m_pModelCom->Set_Animation(1);
		m_pModelCom->Play_Animation(TRUE, m_pTransformCom, fTimeDelta);
	}

	//_int iIndex = static_cast<_int>(static_cast<CPlayer*>(m_pParent)->Get_PlayerIndex(CHARACTER_ZHONGLI));
	//CModel* pModel = static_cast<CModel*>(static_cast<CPlayer*>(m_pParent)->Get_PartObject(CPlayer::PART_BODY, iIndex)->Find_Component(TEXT("Com_Model")));

	//FSM_STATE eState = static_cast<FSM_STATE>(m_pParentFSMCom->Get_CurrentState());
	//
	//if (eState == FSM_IDLE || eState == FSM_WALK || eState == FSM_SPLINT || eState == FSM_JUMP 
	//	|| eState == FSM_RUN || eState == FSM_DASH
	//	|| eState == FSM_WALK_RUN || eState == FSM_WALK_IDLE || eState == FSM_RUN_WALK || eState == FSM_RUN_STOP || eState == FSM_SPLINT_RUN
	//	|| eState == FSM_FLY_START || eState == FSM_FLY_NORMAL || eState == FSM_FLY_LEFT || eState == FSM_FLY_RIGHT
	//	|| eState == FSM_FALL_LOW || eState == FSM_FALL_HIGH || eState == FSM_DROP_DOWN)
	//{
	//	if (m_bHand == TRUE)
	//	{
	//		m_bHand = FALSE;
	//		m_fDissolve = 1.f;
	//		m_pGameInstance->PlaySoundW(L"CreateWeapon.mp3", 1.f);
	//	}

	//	if (m_fDissolve > 0.f)
	//		m_fDissolve -= 0.02f;
	//	else
	//		m_fDissolve = 0.f;

	//	m_pModelCom->Set_PartRootBone("RootNode", pModel->Get_Bone("PRIVATE_WeaponRootSword"));
	//	m_fAttackEndTime = 0.f;
	//}
	//else if (eState == FSM_ATTACK_END)
	//{
	//	m_fAttackEndTime += fTimeDelta;
	//	if (m_fAttackEndTime >= 2.2f && m_fAttackEndTime < 4.4f)
	//	{
	//		if (m_fDissolve < 1.f)
	//			m_fDissolve += 0.04f;
	//		else
	//			m_fDissolve = 1.f;
	//		m_pModelCom->Set_PartRootBone("RootNode", pModel->Get_Bone("WeaponR"));
	//	}
	//	else if (m_fAttackEndTime >= 4.4f) /* 이펙트 효과 추가해야함*/
	//		m_pModelCom->Set_PartRootBone("RootNode", pModel->Get_Bone("PRIVATE_WeaponRootSword"));
	//}
	//else
	//{
	//	m_bHand = TRUE;
	//	m_fDissolve = 0.f;
	//	m_pModelCom->Set_PartRootBone("RootNode", pModel->Get_Bone("WeaponR"));
	//	m_fAttackEndTime = 0.f;
	//}

	return S_OK;
}

HRESULT CGlider_Liyue::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGlider_Liyue::Initialize_Clone(void* pArg)
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

	return S_OK;
}

HRESULT CGlider_Liyue::PreviousTick(_float fTimeDelta)
{
	if (FAILED(__super::PreviousTick(fTimeDelta)))
		return E_FAIL;

    return S_OK;
}

HRESULT CGlider_Liyue::Tick(_float fTimeDelta)
{
	Invalidate_Animation(fTimeDelta);

	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;

	_int iIndex = static_cast<_int>(static_cast<CPlayer*>(m_pParent)->Get_CurrentIndex());
	m_vRootTranslation = static_cast<CPlayer*>(m_pParent)->Get_PartObject(CPlayer::PART_BODY, iIndex)->Get_RootTranslation();

	m_pColliderCom->Update(
		XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix("RootNode")) *
		XMMatrixRotationAxis({ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(180.f)) *
		XMMatrixTranslation(m_vRootTranslation.m128_f32[0], 0.f, m_vRootTranslation.m128_f32[2]) *
		XMLoadFloat4x4(&m_CombinedWorldMatrix)
	);

	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		XMMatrixTranslation(m_vRootTranslation.m128_f32[0], 0.f, m_vRootTranslation.m128_f32[2]) * XMLoadFloat4x4(&m_CombinedWorldMatrix));

	m_pModelCom->Invalidate_CombinedTransformationMatrix();

	return S_OK;
}

HRESULT CGlider_Liyue::LateTick(_float fTimeDelta)
{
	if (FAILED(__super::LateTick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGlider_Liyue::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CGlider_Liyue* CGlider_Liyue::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGlider_Liyue* pInstance = new CGlider_Liyue(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CGlider_Liyue"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGlider_Liyue::Clone(void* pArg)
{
	CGlider_Liyue* pInstance = new CGlider_Liyue(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CGlider_Liyue"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGlider_Liyue::Free()
{
	__super::Free();
}
