#include "stdafx.h"
#include "Brute_Shield.h"

#include "Bone.h"
#include "Part_Body.h"
#include "Brute.h"

#include "GameInstance.h"

CBrute_Shield::CBrute_Shield(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPart_Weapon(pDevice, pContext)
{
}

CBrute_Shield::CBrute_Shield(const CBrute_Shield& rhs)
    : CPart_Weapon(rhs)
{
}

_matrix CBrute_Shield::Get_CombinedMatrix()
{
	return _matrix();
}

HRESULT CBrute_Shield::Invalidate_Animation(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CBrute_Shield::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBrute_Shield::Initialize_Clone(void* pArg)
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

	m_pGameInstance->Add_Collider_Object(COLL_PLAYER_WEAPON, &m_iColliderID, m_pColliderCom, this);

	return S_OK;
}

HRESULT CBrute_Shield::PreviousTick(_float fTimeDelta)
{
	if (FAILED(__super::PreviousTick(fTimeDelta)))
		return E_FAIL;

    return S_OK;
}	

HRESULT CBrute_Shield::Tick(_float fTimeDelta)
{
	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;

	m_vRootTranslation = static_cast<CBrute*>(m_pParent)->Get_PartObject(CBrute::PART_BODY)->Get_RootTranslation();
	//m_vRootTranslation *= 1 / 1.48f;
	m_vRootTranslation *= 0.6f;
	//m_vRootTranslation.m128_f32[0] += 0.58f;

	//m_vRootTranslation.m128_f32[0] += 0.2f;
	//m_vRootTranslation.m128_f32[2] += 1.f;

	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		XMMatrixTranslation(m_vRootTranslation.m128_f32[0], 1.7f, m_vRootTranslation.m128_f32[2]) * XMLoadFloat4x4(&m_CombinedWorldMatrix));

	//m_pModelCom->Invalidate_CombinedTransformationMatrix(1.5f, 0.f);
	m_pModelCom->Invalidate_CombinedTransformationMatrix();

	//m_vRootTranslation = static_cast<CBrute*>(m_pParent)->Get_PartObject(CBrute::PART_BODY)->Get_RootTranslation();
	//m_vRootTranslation *= 3.f;
	//m_vRootTranslation *= 0.014f / 1.48f;
	//m_vRootTranslation *= 1 / 1.48f;

	//m_vRootTranslation.m128_f32[0] += 0.2f;
	//m_vRootTranslation.m128_f32[2] += 1.5f;

	//XMStoreFloat4x4(&m_CombinedWorldMatrix,
	//	XMMatrixTranslation(m_vRootTranslation.m128_f32[0], 0.f, m_vRootTranslation.m128_f32[2]) * XMLoadFloat4x4(&m_CombinedWorldMatrix));

	//XMStoreFloat4x4(&m_CombinedWorldMatrix,
	//	m_pTransformCom->Get_WorldMatrix() *
	//	XMMatrixScaling(1.f, 1.f, 1.f) *
	//	//XMMatrixRotationAxis({ 1.f, 0.f, 0.f, 0.f }, XMConvertToRadians(310.f)) *
	//	XMMatrixTranslation(0.2f, 1.7f, 1.5f) *
	//	m_pParentTransformCom->Get_WorldMatrix());

	////XMStoreFloat4x4(&m_CombinedWorldMatrix,
	////	XMMatrixTranslation(m_vRootTranslation.m128_f32[0], 0.f, m_vRootTranslation.m128_f32[2]) * XMLoadFloat4x4(&m_CombinedWorldMatrix));

	//m_pModelCom->Invalidate_CombinedTransformationMatrix();
	//m_pModelCom->Invalidate_CombinedTransformationMatrix(100.f, 0.f);
	//m_pModelCom->Invalidate_CombinedTransformationMatrix(1 /(1 / 0.014f), 0.f);

	return S_OK;
}

HRESULT CBrute_Shield::LateTick(_float fTimeDelta)
{
	if (FAILED(__super::LateTick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBrute_Shield::Render()
{
	//FSM_HILI_STATE eState = static_cast<FSM_HILI_STATE>(m_pParentFSMCom->Get_CurrentState());
	//if(eState == FSM_HILI_DANCE || eState == FSM_HILI_SIT_START || eState == FSM_HILI_SIT_IDLE || eState == FSM_HILI_SIT
	//	|| eState == FSM_HILI_SLEEP_START || eState == FSM_HILI_SLEEP_1 || eState == FSM_HILI_SLEEP_2 || eState == FSM_HILI_SLEEP_IDLE)
	//	return S_OK;

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CBrute_Shield* CBrute_Shield::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBrute_Shield* pInstance = new CBrute_Shield(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CBrute_Shield"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBrute_Shield::Clone(void* pArg)
{
	CBrute_Shield* pInstance = new CBrute_Shield(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CBrute_Shield"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBrute_Shield::Free()
{
	__super::Free();
}
