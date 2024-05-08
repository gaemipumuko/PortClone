#include "stdafx.h"
#include "Claymore_Wolfmound.h"

#include "Part_Body.h"
#include "Eff_Flame_Enchant_Cross.h"
#include "Player.h"

#include "GameInstance.h"

CClaymore_Wolfmound::CClaymore_Wolfmound(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPart_Weapon(pDevice, pContext)
{
}

CClaymore_Wolfmound::CClaymore_Wolfmound(const CClaymore_Wolfmound& rhs)
    : CPart_Weapon(rhs)
{
}

HRESULT CClaymore_Wolfmound::Bind_ShaderResources_Trail()
{
	return S_OK;
}

_matrix CClaymore_Wolfmound::Get_CombinedMatrix()
{
	_matrix CombinedMatrix = (XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix("Trail_Low"))) * XMMatrixRotationAxis({ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(180.f)) * m_pParentTransformCom->Get_WorldMatrix();

	return CombinedMatrix;
}

HRESULT CClaymore_Wolfmound::Invalidate_Animation(_float fTimeDelta)
{
	_int iIndex = static_cast<_int>(static_cast<CPlayer*>(m_pParent)->Get_PlayerIndex(CHARACTER_DILUC));
	CModel* pModel = static_cast<CModel*>(static_cast<CPlayer*>(m_pParent)->Get_PartObject(CPlayer::PART_BODY, iIndex)->Find_Component(TEXT("Com_Model")));

	FSM_STATE eState = static_cast<FSM_STATE>(m_pParentFSMCom->Get_CurrentState());
	
	if (eState == FSM_IDLE || eState == FSM_WALK || eState == FSM_SPLINT || eState == FSM_JUMP 
		|| eState == FSM_RUN || eState == FSM_DASH
		|| eState == FSM_WALK_RUN || eState == FSM_WALK_IDLE || eState == FSM_RUN_WALK || eState == FSM_RUN_STOP || eState == FSM_SPLINT_RUN
		|| eState == FSM_FLY_START || eState == FSM_FLY_NORMAL || eState == FSM_FLY_LEFT || eState == FSM_FLY_RIGHT
		|| eState == FSM_FALL_LOW || eState == FSM_FALL_HIGH || eState == FSM_DROP_DOWN)
	{
		if (m_bHand == TRUE)
		{
			m_bHand = FALSE;
			m_fDissolve = 1.f;
			m_pGameInstance->PlaySoundW(L"CreateWeapon.mp3", 1.f, SOUND_CH25);
		}

		if (m_fDissolve > 0.f)
			m_fDissolve -= 0.02f;
		else
			m_fDissolve = 0.f;

		m_pModelCom->Set_PartRootBone("RootNode", pModel->Get_Bone("PRIVATE_WeaponRootSword"));
		m_fAttackEndTime = 0.f;
	}
	else if (eState == FSM_ATTACK_END)
	{
		m_fAttackEndTime += fTimeDelta;
		if (m_fAttackEndTime >= 2.2f && m_fAttackEndTime < 4.4f)
		{
			if (m_fDissolve < 1.f)
				m_fDissolve += 0.04f;
			else
				m_fDissolve = 1.f;
			m_pModelCom->Set_PartRootBone("RootNode", pModel->Get_Bone("WeaponR"));
		}
		else if (m_fAttackEndTime >= 4.4f) /* 이펙트 효과 추가해야함*/
			m_pModelCom->Set_PartRootBone("RootNode", pModel->Get_Bone("PRIVATE_WeaponRootSword"));
	}
	else
	{
		if(m_bForceDissolve == FALSE)
			m_fDissolve = 0.f;
		m_bHand = TRUE;
		m_pModelCom->Set_PartRootBone("RootNode", pModel->Get_Bone("WeaponR"));
		m_fAttackEndTime = 0.f;
	}

	return S_OK;
}

HRESULT CClaymore_Wolfmound::Create_FireBlade()
{
	//Prototype_GameObject_Effect_Flame_Enchant_Cross
	CEff_Flame_Enchant_Cross::FLAMEENCHANTCROSS_DESC tDesc = {};
	tDesc.pParentModelCom = m_pModelCom;
	tDesc.pCombineWorldMatrix = &m_CombinedWorldMatrix;
	if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Flame_Enchant_Cross", L"Layer_Effect", &tDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CClaymore_Wolfmound::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CClaymore_Wolfmound::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	/* For.Com_Collider_OBB */
	CBounding_OBB::BOUNDING_OBB_DESC		BoundingDesc = {};
	BoundingDesc.vExtents = _float3(1.5f, 2.5f, 1.5f);
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y - 0.5f, 0.f);
	BoundingDesc.vRadians = _float3(0.f, 0.f, 0.f); // XMConvertToRadians(45.f)

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_OBB"), (CComponent**)&m_pColliderCom, &BoundingDesc)))
		return E_FAIL;

	m_pGameInstance->Add_Collider_Object(COLL_PLAYER_WEAPON, &m_iColliderID, m_pColliderCom, this);

	m_vDissolveColor = _float3(1.f, 1.f, 0.f);
	m_fDissolveSize = 0.2f;

	//CClaymore_Wolfmound::Create_FireBlade();

	return S_OK;
}

HRESULT CClaymore_Wolfmound::PreviousTick(_float fTimeDelta)
{
	if (FAILED(__super::PreviousTick(fTimeDelta)))
		return E_FAIL;

    return S_OK;
}

HRESULT CClaymore_Wolfmound::Tick(_float fTimeDelta)
{
	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;


	_int iIndex = static_cast<_int>(static_cast<CPlayer*>(m_pParent)->Get_PlayerIndex(CHARACTER_DILUC));
	m_vRootTranslation = static_cast<CPlayer*>(m_pParent)->Get_PartObject(CPlayer::PART_BODY, iIndex)->Get_RootTranslation();

	_matrix ColliderMatrix = XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix("Trail_High")) *
		XMMatrixRotationAxis({ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(180.f)) *
		XMMatrixTranslation(m_vRootTranslation.m128_f32[0], 0.f, m_vRootTranslation.m128_f32[2]) *
		XMLoadFloat4x4(&m_CombinedWorldMatrix);

	ColliderMatrix.r[0] = XMVector3Normalize(ColliderMatrix.r[0]);
	ColliderMatrix.r[1] = XMVector3Normalize(ColliderMatrix.r[1]);
	ColliderMatrix.r[2] = XMVector3Normalize(ColliderMatrix.r[2]);

	m_pColliderCom->Update(ColliderMatrix);

	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		XMMatrixTranslation(m_vRootTranslation.m128_f32[0], 0.f, m_vRootTranslation.m128_f32[2]) * XMLoadFloat4x4(&m_CombinedWorldMatrix));

	Invalidate_Animation(fTimeDelta);

	m_pModelCom->Invalidate_CombinedTransformationMatrix();

	return S_OK;
}

HRESULT CClaymore_Wolfmound::LateTick(_float fTimeDelta)
{
	if (FAILED(__super::LateTick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CClaymore_Wolfmound::Render()
{
	FSM_STATE eState = static_cast<FSM_STATE>(m_pParentFSMCom->Get_CurrentState());

	if (eState == FSM_HIT_THROW ||
		eState == FSM_HIT_THROW_FLY || eState == FSM_HIT_ROLLING || eState == FSM_SHOW)
		return S_OK;

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CClaymore_Wolfmound* CClaymore_Wolfmound::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CClaymore_Wolfmound* pInstance = new CClaymore_Wolfmound(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CClaymore_Wolfmound"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CClaymore_Wolfmound::Clone(void* pArg)
{
	CClaymore_Wolfmound* pInstance = new CClaymore_Wolfmound(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CClaymore_Wolfmound"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CClaymore_Wolfmound::Free()
{
	__super::Free();
}
