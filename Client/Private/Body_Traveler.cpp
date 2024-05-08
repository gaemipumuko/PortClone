#include "stdafx.h"
#include "Body_Traveler.h"

#include "GameInstance.h"

CBody_Traveler::CBody_Traveler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart_Body(pDevice, pContext)
{
}

CBody_Traveler::CBody_Traveler(const CBody_Traveler& rhs)
	: CPart_Body(rhs)
{
}

HRESULT CBody_Traveler::Invalidate_Animation(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(TRUE, m_pParentTransformCom, fTimeDelta);

	return S_OK;
}

HRESULT CBody_Traveler::Add_Component(void* pArg)
{
	if (FAILED(__super::Add_Component(pArg)))
		return E_FAIL;

	/* For.Com_Collider_OBB */
	CBounding_OBB::BOUNDING_OBB_DESC		BoundingDesc = {};
	BoundingDesc.vExtents = _float3(0.7f, 0.7f, 0.7f);
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y, 0.f);
	BoundingDesc.vRadians = _float3(0.f, XMConvertToRadians(45.f), 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_OBB"), (CComponent**)&m_pColliderCom, &BoundingDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Traveler::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Traveler::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Traveler::PreviousTick(_float fTimeDelta)
{
	if (FAILED(__super::PreviousTick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Traveler::Tick(_float fTimeDelta)
{
	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Traveler::LateTick(_float fTimeDelta)
{
	if (FAILED(__super::LateTick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Traveler::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CBody_Traveler* CBody_Traveler::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Traveler* pInstance = new CBody_Traveler(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CBody_Traveler"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_Traveler::Clone(void* pArg)
{
	CBody_Traveler* pInstance = new CBody_Traveler(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CBody_Traveler"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Traveler::Free()
{
	__super::Free();
}
