#include "stdafx.h"
#include "Glider_Thunderclap.h"

#include "Part_Body.h"

#include "GameInstance.h"

CGlider_Thunderclap::CGlider_Thunderclap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPart_Glider(pDevice, pContext)
{
}

CGlider_Thunderclap::CGlider_Thunderclap(const CGlider_Thunderclap& rhs)
    : CPart_Glider(rhs)
{
}

HRESULT CGlider_Thunderclap::Invalidate_Animation(_float fTimeDelta)
{
	FSM_STATE eState = static_cast<FSM_STATE>(m_pParentFSMCom->Get_CurrentState());

	if (eState == FSM_FLY_START)
	{
		m_pModelCom->Set_Animation(1);
		m_pModelCom->Play_Animation(FALSE, m_pTransformCom, fTimeDelta * 1.2f);
	}
	else
	{
		m_pModelCom->Set_Animation(0);
		m_pModelCom->Play_Animation(TRUE, m_pTransformCom, fTimeDelta);
	}

	return S_OK;
}

HRESULT CGlider_Thunderclap::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGlider_Thunderclap::Initialize_Clone(void* pArg)
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

HRESULT CGlider_Thunderclap::PreviousTick(_float fTimeDelta)
{
	if (FAILED(__super::PreviousTick(fTimeDelta)))
		return E_FAIL;

    return S_OK;
}

HRESULT CGlider_Thunderclap::Tick(_float fTimeDelta)
{
	Invalidate_Animation(fTimeDelta);

	FSM_STATE eState = static_cast<FSM_STATE>(m_pParentFSMCom->Get_CurrentState());
	FSM_STATE eFrevState = static_cast<FSM_STATE>(m_pParentFSMCom->Get_PrevState());

	_int iIndex = static_cast<_int>(static_cast<CPlayer*>(m_pParent)->Get_CurrentIndex());
	CModel* pModel = static_cast<CModel*>(static_cast<CPlayer*>(m_pParent)->Get_PartObject(CPlayer::PART_BODY, iIndex)->Find_Component(TEXT("Com_Model")));

	if (eState == FSM_FLY_START)
	{
		m_fMargin = 0.1f;
		XMStoreFloat4x4(&m_CombinedWorldMatrix,
			m_pTransformCom->Get_WorldMatrix() *
			XMMatrixScaling(0.1f, 0.1f, 0.1f) *
			XMMatrixRotationAxis({ 1.f, 0.f, 0.f, 0.f }, XMConvertToRadians(310.f)) *
			XMMatrixTranslation(0.f, 1.1f, m_fMargin) *
			m_pParentTransformCom->Get_WorldMatrix());
	}

	else if (eFrevState == FSM_FLY_START && pModel->Get_ChangeAnimation() == TRUE)
	{
		m_fMargin = 0.1f;
		XMStoreFloat4x4(&m_CombinedWorldMatrix,
			m_pTransformCom->Get_WorldMatrix() *
			XMMatrixScaling(0.1f, 0.1f, 0.1f) *
			XMMatrixRotationAxis({ 1.f, 0.f, 0.f, 0.f }, XMConvertToRadians(310.f)) *
			XMMatrixTranslation(0.f, 1.1f, m_fMargin) *
			m_pParentTransformCom->Get_WorldMatrix());
	}
	else
	{
		if (m_fMargin >= 0.2f)
			m_fMargin = 0.2f;
		else
			m_fMargin += fTimeDelta * 0.1f;

		XMStoreFloat4x4(&m_CombinedWorldMatrix,
			m_pTransformCom->Get_WorldMatrix() *
			XMMatrixScaling(0.1f, 0.1f, 0.1f) *
			XMMatrixRotationAxis({ 1.f, 0.f, 0.f, 0.f }, XMConvertToRadians(310.f)) *
			XMMatrixTranslation(0.f, 1.1f, m_fMargin) *
			m_pParentTransformCom->Get_WorldMatrix());
	}

	//_int iIndex = static_cast<_int>(static_cast<CPlayer*>(m_pParent)->Get_CurrentIndex());
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

HRESULT CGlider_Thunderclap::LateTick(_float fTimeDelta)
{
	if (FAILED(__super::LateTick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGlider_Thunderclap::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CGlider_Thunderclap* CGlider_Thunderclap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGlider_Thunderclap* pInstance = new CGlider_Thunderclap(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CGlider_Thunderclap"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGlider_Thunderclap::Clone(void* pArg)
{
	CGlider_Thunderclap* pInstance = new CGlider_Thunderclap(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CGlider_Thunderclap"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGlider_Thunderclap::Free()
{
	__super::Free();
}
