#include "stdafx.h"
#include "Slime_IceCone.h"

#include "Bone.h"
#include "Part_Body.h"
#include "Slime_Big.h"

#include "GameInstance.h"

CSlime_IceCone::CSlime_IceCone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPart_Weapon(pDevice, pContext)
{
}

CSlime_IceCone::CSlime_IceCone(const CSlime_IceCone& rhs)
    : CPart_Weapon(rhs)
{
}

_matrix CSlime_IceCone::Get_CombinedMatrix()
{
	return _matrix();
}

HRESULT CSlime_IceCone::Invalidate_Animation(_float fTimeDelta)
{
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
	//	m_pModelCom->Set_PartRootBone("RootNode", pModel->Get_Bone("PRIVATE_WeaponRootSword"));
	//	m_fAttackEndTime = 0.f;
	//}
	//else if (eState == FSM_ATTACK_END)
	//{
	//	m_fAttackEndTime += fTimeDelta;
	//	if (m_fAttackEndTime >= 2.2f && m_fAttackEndTime < 4.4f)
	//		m_pModelCom->Set_PartRootBone("RootNode", pModel->Get_Bone("WeaponR"));
	//	else if (m_fAttackEndTime >= 4.4f) /* 이펙트 효과 추가해야함*/
	//		m_pModelCom->Set_PartRootBone("RootNode", pModel->Get_Bone("PRIVATE_WeaponRootSword"));
	//}
	//else
	//{
	//	m_pModelCom->Set_PartRootBone("RootNode", pModel->Get_Bone("WeaponR"));
	//	m_fAttackEndTime = 0.f;
	//}

	return S_OK;
}

HRESULT CSlime_IceCone::Break_IceCone(_int iConeIndex)
{
	if (iConeIndex > 2)
		return S_OK;

	Set_MeshIndex(iConeIndex);

	return S_OK;
}

void CSlime_IceCone::Release_ColliderID()
{
	m_pGameInstance->Release_Collider_Object(COLL_MONSTER_WEAPON, m_iColliderID);
}

HRESULT CSlime_IceCone::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSlime_IceCone::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	/* For.Com_Collider_OBB */
	CBounding_OBB::BOUNDING_OBB_DESC		BoundingDesc = {};
	BoundingDesc.vExtents = _float3(0.5f, 1.f, 0.5f);
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y - 1.f, -1.f);
	BoundingDesc.vRadians = _float3(XMConvertToRadians(90.f), 0.f, 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_OBB"), (CComponent**)&m_pColliderCom, &BoundingDesc)))
		return E_FAIL;

	m_pGameInstance->Add_Collider_Object(COLL_MONSTER_WEAPON, &m_iColliderID, m_pColliderCom, this);


	int a = 0;
	return S_OK;
}

HRESULT CSlime_IceCone::PreviousTick(_float fTimeDelta)
{
	if (FAILED(__super::PreviousTick(fTimeDelta)))
		return E_FAIL;

    return S_OK;
}

HRESULT CSlime_IceCone::Tick(_float fTimeDelta)
{
	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;

	m_vRootTranslation = static_cast<CSlime_Big*>(m_pParent)->Get_PartObject(CSlime_Big::PART_BODY)->Get_RootTranslation();

	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		XMMatrixTranslation(m_vRootTranslation.m128_f32[0], 0.f, m_vRootTranslation.m128_f32[2]) * XMLoadFloat4x4(&m_CombinedWorldMatrix));

	m_pModelCom->Invalidate_CombinedTransformationMatrix(10.f, 20.f);

	return S_OK;
}

HRESULT CSlime_IceCone::LateTick(_float fTimeDelta)
{
	if (FAILED(__super::LateTick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSlime_IceCone::Render()
{
	if (FAILED(CPart_Weapon::Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	_bool bLightMap = { };
	for (size_t siMesh = 0; siMesh < iNumMeshes; siMesh++)
	{
		if (m_iMeshIndex - 1 == siMesh || m_iMeshIndex - 2 == siMesh)
			continue;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(siMesh), TEXTURETYPE::aiTextureType_DIFFUSE)))
			continue;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_LightMapTexture", static_cast<_uint>(siMesh), TEXTURETYPE::aiTextureType_LIGHTMAP)))
		{
			if (FAILED(m_pShaderCom->Bind_RawValue("g_bLightMapTexture", &(bLightMap = FALSE), sizeof _bool)))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pShaderCom->Bind_RawValue("g_bLightMapTexture", &(bLightMap = TRUE), sizeof _bool)))
				return E_FAIL;
		}

		if (FAILED(m_pModelCom->Bind_BoneMatrices(static_cast<_uint>(siMesh), m_pShaderCom, "g_BoneMatrices")))
			return E_FAIL;

		if (m_bPetrification == FALSE)
		{
			if (m_fDissolve == 0.f)
			{
				if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_DEFAULT)))
					return E_FAIL;
			}
			else
			{
				if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolve", &m_fDissolve, sizeof _float)))
					return E_FAIL;

				if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveSize", &m_fDissolveSize, sizeof _float)))
					return E_FAIL;

				if (FAILED(m_pShaderCom->Bind_RawValue("g_vDissolveColor", &m_vDissolveColor, sizeof _float3)))
					return E_FAIL;

				if (FAILED(m_pDissolveCom->Bind_ShaderCurrentTexture(m_pShaderCom, "g_DissolveTexture")))
					return E_FAIL;

				if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_DISSOLVE)))
					return E_FAIL;
			}
		}
		else
		{
			if (FAILED(m_pPetrificationCom->Bind_ShaderCurrentTexture(m_pShaderCom, "g_PetrificationTexture")))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_PETRIFICATION)))
				return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render(static_cast<_uint>(siMesh))))
			return E_FAIL;
	}

	return S_OK;

	return S_OK;
}

CSlime_IceCone* CSlime_IceCone::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSlime_IceCone* pInstance = new CSlime_IceCone(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CSlime_IceCone"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSlime_IceCone::Clone(void* pArg)
{
	CSlime_IceCone* pInstance = new CSlime_IceCone(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CSlime_IceCone"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSlime_IceCone::Free()
{
	__super::Free();
}
