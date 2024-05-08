#include "stdafx.h"
#include "Staff_Elec.h"

#include "Bone.h"
#include "Part_Body.h"
#include "Abyss_Mage.h"
#include "Body_Abyss_Mage.h"

#include "GameInstance.h"

CStaff_Elec::CStaff_Elec(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPart_Weapon(pDevice, pContext)
{
}

CStaff_Elec::CStaff_Elec(const CStaff_Elec& rhs)
    : CPart_Weapon(rhs)
{
}

_matrix CStaff_Elec::Get_CombinedMatrix()
{
	return _matrix();
}

HRESULT CStaff_Elec::Invalidate_Animation(_float fTimeDelta)
{
	return S_OK;
}

void CStaff_Elec::Release_ColliderID()
{
	m_pGameInstance->Release_Collider_Object(COLL_MONSTER_WEAPON, m_iColliderID);
}

HRESULT CStaff_Elec::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStaff_Elec::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	///* For.Com_Collider_OBB */
	//CBounding_OBB::BOUNDING_OBB_DESC		BoundingDesc = {};
	//BoundingDesc.vExtents = _float3(1.f, 2.f, 1.f);
	//BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y - 2.f, 0.f);
	//BoundingDesc.vRadians = _float3(XMConvertToRadians(90.f), 0.f, 0.f);
	//
	//if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
	//	TEXT("Com_Collider_OBB"), (CComponent**)&m_pColliderCom, &BoundingDesc)))
	//	return E_FAIL;

	//m_pGameInstance->Add_Collider_Object(COLL_MONSTER_WEAPON, &m_iColliderID, m_pColliderCom, this);

	return S_OK;
}

HRESULT CStaff_Elec::PreviousTick(_float fTimeDelta)
{
	if (FAILED(__super::PreviousTick(fTimeDelta)))
		return E_FAIL;

    return S_OK;
}

HRESULT CStaff_Elec::Tick(_float fTimeDelta)
{
	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;

	m_vRootTranslation = static_cast<CAbyss_Mage*>(m_pParent)->Get_PartObject(CAbyss_Mage::PART_BODY)->Get_RootTranslation();

	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		XMMatrixTranslation(m_vRootTranslation.m128_f32[0], m_vRootTranslation.m128_f32[1] - 1.5f, m_vRootTranslation.m128_f32[2]) * XMLoadFloat4x4(&m_CombinedWorldMatrix));

	m_pModelCom->Invalidate_CombinedTransformationMatrix();

	return S_OK;
}

HRESULT CStaff_Elec::LateTick(_float fTimeDelta)
{
	if (FAILED(__super::LateTick(fTimeDelta)))
		return E_FAIL;

#ifdef _DEBUG
	//m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif

	return S_OK;
}

HRESULT CStaff_Elec::Render()
{
	if (static_cast<CAbyss_Mage*>(m_pParent)->Get_PartObject(CAbyss_Mage::PART_BODY)->Get_RenderEnable() == TRUE)
	{
		if (FAILED(__super::Render()))
			return E_FAIL;
	}

	return S_OK;
}

CStaff_Elec* CStaff_Elec::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStaff_Elec* pInstance = new CStaff_Elec(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CStaff_Elec"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CStaff_Elec::Clone(void* pArg)
{
	CStaff_Elec* pInstance = new CStaff_Elec(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CStaff_Elec"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStaff_Elec::Free()
{
	__super::Free();
}