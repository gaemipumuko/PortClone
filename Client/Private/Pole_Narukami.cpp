#include "stdafx.h"
#include "Pole_Narukami.h"

#include "Part_Body.h"
#include "Player.h"
#include "Piercing_Trail.h"
#include "Slash_Trail.h"

#include "GameInstance.h"

CPole_Narukami::CPole_Narukami(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPart_Weapon(pDevice, pContext)
{
}

CPole_Narukami::CPole_Narukami(const CPole_Narukami& rhs)
    : CPart_Weapon(rhs)
{
}

HRESULT CPole_Narukami::Add_ExtraComponent()
{
	/* For.Com_Model */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, L"Prototype_Component_Model_Katana",
		TEXT("Com_SwordModel"), (CComponent**)&m_pSwordModelCom)))
		return E_FAIL;

	_int iIndex = static_cast<_int>(static_cast<CPlayer*>(m_pParent)->Get_PlayerIndex(CHARACTER_RAIDEN));
	CModel* pModel = static_cast<CModel*>(static_cast<CPlayer*>(m_pParent)->Get_PartObject(CPlayer::PART_BODY, iIndex)->Find_Component(TEXT("Com_Model")));

	m_pSwordModelCom->Set_PartRootBone("RootNode", pModel->Get_Bone("WeaponR"));

	return S_OK;
}

HRESULT CPole_Narukami::Render_Sword()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;
	_uint		iNumMeshes = m_pSwordModelCom->Get_NumMeshes();

	_bool bLightMap = { };
	for (size_t siMesh = 0; siMesh < iNumMeshes; siMesh++)
	{
		if (m_iMeshIndex - 1 == siMesh || m_iMeshIndex - 2 == siMesh)
			continue;

		if (FAILED(m_pSwordModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(siMesh), TEXTURETYPE::aiTextureType_DIFFUSE)))
			continue;

		if (FAILED(m_pSwordModelCom->Bind_Material(m_pShaderCom, "g_LightMapTexture", static_cast<_uint>(siMesh), TEXTURETYPE::aiTextureType_LIGHTMAP)))
		{
			if (FAILED(m_pShaderCom->Bind_RawValue("g_bLightMapTexture", &(bLightMap = FALSE), sizeof _bool)))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pShaderCom->Bind_RawValue("g_bLightMapTexture", &(bLightMap = TRUE), sizeof _bool)))
				return E_FAIL;
		}

		if (FAILED(m_pSwordModelCom->Bind_BoneMatrices(static_cast<_uint>(siMesh), m_pShaderCom, "g_BoneMatrices")))
			return E_FAIL;

		if (m_bPetrification == FALSE)
		{
			if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_DEFAULT)))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pPetrificationCom->Bind_ShaderCurrentTexture(m_pShaderCom, "g_PetrificationTexture")))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_PETRIFICATION)))
				return E_FAIL;
		}

		if (FAILED(m_pSwordModelCom->Render(static_cast<_uint>(siMesh))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CPole_Narukami::Invalidate_Animation(_float fTimeDelta)
{
	_int iIndex = static_cast<_int>(static_cast<CPlayer*>(m_pParent)->Get_PlayerIndex(CHARACTER_RAIDEN));
	CModel* pModel = static_cast<CModel*>(static_cast<CPlayer*>(m_pParent)->Get_PartObject(CPlayer::PART_BODY, iIndex)->Find_Component(TEXT("Com_Model")));

	FSM_STATE eState = static_cast<FSM_STATE>(m_pParentFSMCom->Get_CurrentState());

	if (eState != FSM_ELEMENTAL_BURST)
	{
		if (eState == FSM_IDLE || eState == FSM_WALK || eState == FSM_SPLINT || eState == FSM_JUMP
			|| eState == FSM_RUN || eState == FSM_DASH
			|| eState == FSM_WALK_RUN || eState == FSM_WALK_IDLE || eState == FSM_RUN_WALK || eState == FSM_RUN_STOP || eState == FSM_SPLINT_RUN
			|| eState == FSM_FLY_START || eState == FSM_FLY_NORMAL || eState == FSM_FLY_LEFT || eState == FSM_FLY_RIGHT
			|| eState == FSM_FALL_LOW || eState == FSM_FALL_HIGH || eState == FSM_DROP_DOWN
			|| eState == FSM_FALL_ATTACK_END1 || eState == FSM_FALL_ATTACK_END2 || eState == FSM_ATTACK_READY
			|| eState == FSM_ELEMENTAL_SKILL1 || eState == FSM_ELEMENTAL_SKILL1_IDLE)
		{
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

			m_pModelCom->Set_PartRootBone("RootNode", pModel->Get_Bone("PRIVATE_WeaponRootPole"));
			m_fAttackEndTime = 0.f;
		}
		else if (eState == FSM_ATTACK3)
		{
			m_fAttackEndTime += fTimeDelta;
			if (m_fAttackEndTime >= 2.2f && m_fAttackEndTime < 6.f)
			{
				if (m_fDissolve < 1.f)
					m_fDissolve += 0.04f;
				else
					m_fDissolve = 1.f;
				m_pModelCom->Set_PartRootBone("RootNode", pModel->Get_Bone("WeaponR"));
			}
			else if (m_fAttackEndTime >= 6.f) /* 이펙트 효과 추가해야함*/
				m_pModelCom->Set_PartRootBone("RootNode", pModel->Get_Bone("PRIVATE_WeaponRootPole"));

			m_bInitTime = FALSE;
		}
		else if (eState == FSM_ATTACK4)
		{
			if (m_bInitTime == FALSE)
			{
				m_fAttackEndTime = 0.f;
				m_bInitTime = TRUE;
			}
			m_fAttackEndTime += fTimeDelta;
			if (m_fAttackEndTime >= 2.2f && m_fAttackEndTime < 6.5f)
			{
				if (m_fDissolve < 1.f)
					m_fDissolve += 0.04f;
				else
					m_fDissolve = 1.f;
				m_pModelCom->Set_PartRootBone("RootNode", pModel->Get_Bone("WeaponR"));
			}
			else if (m_fAttackEndTime >= 6.5f) /* 이펙트 효과 추가해야함*/
				m_pModelCom->Set_PartRootBone("RootNode", pModel->Get_Bone("PRIVATE_WeaponRootPole"));
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
				m_pModelCom->Set_PartRootBone("RootNode", pModel->Get_Bone("PRIVATE_WeaponRootPole"));
		}
		else
		{
			m_bHand = TRUE;
			m_fDissolve = 0.f;
			m_pModelCom->Set_PartRootBone("RootNode", pModel->Get_Bone("WeaponR"));
			m_fAttackEndTime = 0.f;
		}
	}

	return S_OK;
}

HRESULT CPole_Narukami::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPole_Narukami::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(CPole_Narukami::Add_ExtraComponent()))
		return E_FAIL;

	/* For.Com_Collider_OBB */
	CBounding_OBB::BOUNDING_OBB_DESC		BoundingDesc = {};
	BoundingDesc.vExtents = _float3(1.f, 1.5f, 1.f);
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y - 1.f, -1.f);
	BoundingDesc.vRadians = _float3(XMConvertToRadians(90.f), 0.f, 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_OBB"), (CComponent**)&m_pColliderCom, &BoundingDesc)))
		return E_FAIL;

	m_pGameInstance->Add_Collider_Object(COLL_PLAYER_WEAPON, &m_iColliderID, m_pColliderCom, this);

	m_vDissolveColor = _float3(1.f, 1.f, 0.f);
	m_fDissolveSize = 0.2f;

	return S_OK;
}

HRESULT CPole_Narukami::PreviousTick(_float fTimeDelta)
{
	if (FAILED(__super::PreviousTick(fTimeDelta)))
		return E_FAIL;

    return S_OK;
}

HRESULT CPole_Narukami::Tick(_float fTimeDelta)
{
	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;

	FSM_STATE eState = static_cast<FSM_STATE>(m_pParentFSMCom->Get_CurrentState());

	if (eState == FSM_ELEMENTAL_BURST)
	{
		_int iIndex = static_cast<_int>(static_cast<CPlayer*>(m_pParent)->Get_PlayerIndex(CHARACTER_RAIDEN));
		m_vRootTranslation = static_cast<CPlayer*>(m_pParent)->Get_PartObject(CPlayer::PART_BODY, iIndex)->Get_RootTranslation();

		_matrix CombineWorldMatrix = XMLoadFloat4x4(&m_CombinedWorldMatrix);
		CombineWorldMatrix.r[0] = XMVector3Normalize(CombineWorldMatrix.r[0]);
		CombineWorldMatrix.r[1] = XMVector3Normalize(CombineWorldMatrix.r[1]);
		CombineWorldMatrix.r[2] = XMVector3Normalize(CombineWorldMatrix.r[2]);

		m_pColliderCom->Update(XMLoadFloat4x4(m_pSwordModelCom->Get_CombinedMatrix("RootNode")) *
			XMMatrixRotationAxis({ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(180.f)) *
			XMMatrixTranslation(m_vRootTranslation.m128_f32[0], 0.f, m_vRootTranslation.m128_f32[2]) *
			CombineWorldMatrix);

		XMStoreFloat4x4(&m_CombinedWorldMatrix,
			XMMatrixTranslation(m_vRootTranslation.m128_f32[0], 0.f, m_vRootTranslation.m128_f32[2]) * XMLoadFloat4x4(&m_CombinedWorldMatrix));

		m_pSwordModelCom->Invalidate_CombinedTransformationMatrix();
		Invalidate_Animation(fTimeDelta);
	}
	else
	{
		_int iIndex = static_cast<_int>(static_cast<CPlayer*>(m_pParent)->Get_PlayerIndex(CHARACTER_RAIDEN));
		m_vRootTranslation = static_cast<CPlayer*>(m_pParent)->Get_PartObject(CPlayer::PART_BODY, iIndex)->Get_RootTranslation();

		_matrix CombineWorldMatrix = XMLoadFloat4x4(&m_CombinedWorldMatrix);
		CombineWorldMatrix.r[0] = XMVector3Normalize(CombineWorldMatrix.r[0]);
		CombineWorldMatrix.r[1] = XMVector3Normalize(CombineWorldMatrix.r[1]);
		CombineWorldMatrix.r[2] = XMVector3Normalize(CombineWorldMatrix.r[2]);

		m_pColliderCom->Update(XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix("RootNode")) *
			XMMatrixRotationAxis({ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(180.f)) *
			XMMatrixTranslation(m_vRootTranslation.m128_f32[0], 0.f, m_vRootTranslation.m128_f32[2]) *
			CombineWorldMatrix);

		XMStoreFloat4x4(&m_CombinedWorldMatrix,
			XMMatrixTranslation(m_vRootTranslation.m128_f32[0], 0.f, m_vRootTranslation.m128_f32[2]) * XMLoadFloat4x4(&m_CombinedWorldMatrix));

		Invalidate_Animation(fTimeDelta);

		m_pModelCom->Invalidate_CombinedTransformationMatrix();
	}
	
	return S_OK;
}

HRESULT CPole_Narukami::LateTick(_float fTimeDelta)
{
	if (FAILED(__super::LateTick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPole_Narukami::Render()
{
	FSM_STATE eState = static_cast<FSM_STATE>(m_pParentFSMCom->Get_CurrentState());

	if (eState == FSM_ELEMENTAL_BURST_IDLE || eState == FSM_HIT_THROW ||
		eState == FSM_HIT_THROW_FLY || eState == FSM_HIT_ROLLING || eState == FSM_SHOW)
		return S_OK;

	if (eState == FSM_ELEMENTAL_BURST)
	{
		if (FAILED(CPole_Narukami::Render_Sword()))
			return E_FAIL;
	}
	else
	{
		if (FAILED(__super::Render()))
			return E_FAIL;
	}

//#ifdef _DEBUG
//	m_pColliderCom->Render();
//#endif

	return S_OK;
}

CPole_Narukami* CPole_Narukami::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPole_Narukami* pInstance = new CPole_Narukami(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CPole_Narukami"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPole_Narukami::Clone(void* pArg)
{
	CPole_Narukami* pInstance = new CPole_Narukami(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CPole_Narukami"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPole_Narukami::Free()
{
	__super::Free();

	Safe_Release(m_pSwordModelCom);
}
