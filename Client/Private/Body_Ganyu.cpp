#include "stdafx.h"
#include "Body_Ganyu.h"

#include "GameInstance.h"

CBody_Ganyu::CBody_Ganyu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart_Body(pDevice, pContext)
{
}

CBody_Ganyu::CBody_Ganyu(const CBody_Ganyu& rhs)
	: CPart_Body(rhs)
{
}

HRESULT CBody_Ganyu::Invalidate_Animation(_float fTimeDelta)
{
	FSM_STATE eState = static_cast<FSM_STATE>(m_pParentFSMCom->Get_CurrentState());

	switch (eState)
	{
	case FSM_IDLE:
		m_pModelCom->Set_Animation(0);
		m_pModelCom->Play_Animation(TRUE, m_pParentTransformCom, fTimeDelta);
		break;
	case FSM_WALK:
		m_pModelCom->Set_Animation(2);
		m_pModelCom->Play_Animation(TRUE, m_pParentTransformCom, fTimeDelta);
		break;
	}

	return S_OK;
}

HRESULT CBody_Ganyu::Add_Component(void* pArg)
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

HRESULT CBody_Ganyu::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Ganyu::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Ganyu::PreviousTick(_float fTimeDelta)
{
	if (FAILED(__super::PreviousTick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Ganyu::Tick(_float fTimeDelta)
{
	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Ganyu::LateTick(_float fTimeDelta)
{
	if (FAILED(__super::LateTick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Ganyu::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CBody_Ganyu* CBody_Ganyu::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Ganyu* pInstance = new CBody_Ganyu(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CBody_Ganyu"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_Ganyu::Clone(void* pArg)
{
	CBody_Ganyu* pInstance = new CBody_Ganyu(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CBody_Ganyu"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Ganyu::Free()
{
	__super::Free();
}
