#include "stdafx.h"
#include "Hili_Club.h"

#include "Bone.h"
#include "Part_Body.h"
#include "Hili_Normal.h"

#include "GameInstance.h"

CHili_Club::CHili_Club(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPart_Weapon(pDevice, pContext)
{
}

CHili_Club::CHili_Club(const CHili_Club& rhs)
    : CPart_Weapon(rhs)
{
}

_matrix CHili_Club::Get_CombinedMatrix()
{
	return _matrix();
}

HRESULT CHili_Club::Invalidate_Animation(_float fTimeDelta)
{
	return S_OK;
}

void CHili_Club::Release_ColliderID()
{
	m_pGameInstance->Release_Collider_Object(COLL_MONSTER_WEAPON, m_iColliderID);
}

HRESULT CHili_Club::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHili_Club::Initialize_Clone(void* pArg)
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

	return S_OK;
}

HRESULT CHili_Club::PreviousTick(_float fTimeDelta)
{
	if (FAILED(__super::PreviousTick(fTimeDelta)))
		return E_FAIL;

    return S_OK;
}

HRESULT CHili_Club::Tick(_float fTimeDelta)
{
	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;

	m_vRootTranslation = static_cast<CHili_Normal*>(m_pParent)->Get_PartObject(CHili_Normal::PART_BODY)->Get_RootTranslation();

	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		XMMatrixTranslation(m_vRootTranslation.m128_f32[0], 0.f, m_vRootTranslation.m128_f32[2]) * XMLoadFloat4x4(&m_CombinedWorldMatrix));

	m_pModelCom->Invalidate_CombinedTransformationMatrix();

	return S_OK;
}

HRESULT CHili_Club::LateTick(_float fTimeDelta)
{
	if (FAILED(__super::LateTick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CHili_Club::Render()
{
	FSM_HILI_STATE eState = static_cast<FSM_HILI_STATE>(m_pParentFSMCom->Get_CurrentState());

	if(eState == FSM_HILI_DANCE || eState == FSM_HILI_SIT_START || eState == FSM_HILI_SIT_IDLE || eState == FSM_HILI_SIT
		|| eState == FSM_HILI_SLEEP_START || eState == FSM_HILI_SLEEP_1 || eState == FSM_HILI_SLEEP_2 || eState == FSM_HILI_SLEEP_IDLE)
		return S_OK;

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CHili_Club* CHili_Club::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHili_Club* pInstance = new CHili_Club(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CHili_Club"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHili_Club::Clone(void* pArg)
{
	CHili_Club* pInstance = new CHili_Club(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CHili_Club"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHili_Club::Free()
{
	__super::Free();
}
