#include "stdafx.h"
#include "Brute_Axe.h"

#include "Bone.h"
#include "Part_Body.h"
#include "Brute.h"

#include "GameInstance.h"

CBrute_Axe::CBrute_Axe(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPart_Weapon(pDevice, pContext)
{
}

CBrute_Axe::CBrute_Axe(const CBrute_Axe& rhs)
    : CPart_Weapon(rhs)
{
}

_matrix CBrute_Axe::Get_CombinedMatrix()
{
	return _matrix();
}

HRESULT CBrute_Axe::Invalidate_Animation(_float fTimeDelta)
{
	return S_OK;
}

void CBrute_Axe::Release_ColliderID()
{
	m_pGameInstance->Release_Collider_Object(COLL_MONSTER_WEAPON, m_iColliderID);
}

HRESULT CBrute_Axe::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBrute_Axe::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	/* For.Com_Collider_OBB */
	CBounding_OBB::BOUNDING_OBB_DESC		BoundingDesc = {};
	BoundingDesc.vExtents = _float3(1.f, 2.f, 1.f);
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y - 2.f, 0.f);
	BoundingDesc.vRadians = _float3(XMConvertToRadians(90.f), 0.f, 0.f);
	
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_OBB"), (CComponent**)&m_pColliderCom, &BoundingDesc)))
		return E_FAIL;

	m_pGameInstance->Add_Collider_Object(COLL_MONSTER_WEAPON, &m_iColliderID, m_pColliderCom, this);

	return S_OK;
}

HRESULT CBrute_Axe::PreviousTick(_float fTimeDelta)
{
	if (FAILED(__super::PreviousTick(fTimeDelta)))
		return E_FAIL;

    return S_OK;
}

HRESULT CBrute_Axe::Tick(_float fTimeDelta)
{
	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;

	m_vRootTranslation = static_cast<CBrute*>(m_pParent)->Get_PartObject(CBrute::PART_BODY)->Get_RootTranslation();
	m_vRootTranslation *= 1 / 1.48f;
	m_vRootTranslation.m128_f32[0] += 0.58f;

	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		XMMatrixTranslation(m_vRootTranslation.m128_f32[0], 0.15f, m_vRootTranslation.m128_f32[2]) * XMLoadFloat4x4(&m_CombinedWorldMatrix));

	_matrix ColliderMatrix = XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix("hitbox")) *
		XMMatrixRotationAxis({ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(180.f)) *
		XMLoadFloat4x4(&m_CombinedWorldMatrix);
	ColliderMatrix.r[0] = XMVector3Normalize(ColliderMatrix.r[0]);
	ColliderMatrix.r[1] = XMVector3Normalize(ColliderMatrix.r[1]);
	ColliderMatrix.r[2] = XMVector3Normalize(ColliderMatrix.r[2]);
	m_pColliderCom->Update(ColliderMatrix);

	m_pModelCom->Invalidate_CombinedTransformationMatrix();

	return S_OK;
}

HRESULT CBrute_Axe::LateTick(_float fTimeDelta)
{
	if (FAILED(__super::LateTick(fTimeDelta)))
		return E_FAIL;

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif

	return S_OK;
}

HRESULT CBrute_Axe::Render()
{
	//FSM_HILI_STATE eState = static_cast<FSM_HILI_STATE>(m_pParentFSMCom->Get_CurrentState());
	//if(eState == FSM_HILI_DANCE || eState == FSM_HILI_SIT_START || eState == FSM_HILI_SIT_IDLE || eState == FSM_HILI_SIT
	//	|| eState == FSM_HILI_SLEEP_START || eState == FSM_HILI_SLEEP_1 || eState == FSM_HILI_SLEEP_2 || eState == FSM_HILI_SLEEP_IDLE)
	//	return S_OK;

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CBrute_Axe* CBrute_Axe::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBrute_Axe* pInstance = new CBrute_Axe(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CBrute_Axe"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBrute_Axe::Clone(void* pArg)
{
	CBrute_Axe* pInstance = new CBrute_Axe(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CBrute_Axe"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBrute_Axe::Free()
{
	__super::Free();
}