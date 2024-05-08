#include "stdafx.h"
#include "Elemental.h"

#include "GameInstance.h"

CElemental::CElemental(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CDeciderObject(pDevice, pContext)
{
}

CElemental::CElemental(const CElemental& rhs)
	: CDeciderObject(rhs)
{
}

HRESULT CElemental::Invalidate_Elemental(void* pArg)
{
	Safe_Release(m_pTargetTransformCom);

	ELEMENTAL_DESC* pDesc = static_cast<ELEMENTAL_DESC*>(pArg);
	m_pTargetTransformCom = pDesc->pTargetTransform;
	Safe_AddRef(m_pTargetTransformCom);

	_vector	vCollectionPosition = pDesc->vInitPosition;
	vCollectionPosition.m128_f32[0] += m_pGameInstance->Get_Float_Random(-1.f, 1.f);
	vCollectionPosition.m128_f32[2] += m_pGameInstance->Get_Float_Random(-1.f, 1.f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCollectionPosition);

	m_fFrame = 0.f;
	m_bIntersect = FALSE;
	m_eAttribute = pDesc->eAttribute;

	switch (m_eAttribute)
	{
	case ELEMENT_FIRE:      // 불
		m_vAttColor = { 0.6500f, 0.2758f, 0.2758f, 1.0000f };
		break;
	case ELEMENT_WATER:     // 물
		m_vAttColor = { 0.2471f, 0.5019f, 0.9098f, 1.0000f };
		break;
	case ELEMENT_FROST:     // 얼음
		m_vAttColor = { 0.4471f, 0.7373f, 0.8000f, 1.0000f };
		break;
	case ELEMENT_ELECTRIC:  // 번개
		m_vAttColor = { 0.5953f, 0.2674f, 0.7377f, 1.0000f };
		break;
	case ELEMENT_WIND:      // 바람
		m_vAttColor = { 0.0000f, 0.5992f, 0.5177f, 1.0000f };
		break;
	case ELEMENT_EARTH:     // 바위
		m_vAttColor = { 0.5584f, 0.4282f, 0.0025f, 1.0000f };
		break;
	case ELEMENT_END:		// 무속성
		m_vAttColor = { 1.0000f, 1.0000f, 1.0000f, 1.0000f };
		break;
	}

	return S_OK;
}

HRESULT CElemental::Add_Component(void* pArg)
{
	ELEMENTAL_DESC* pDesc = static_cast<ELEMENTAL_DESC*>(pArg);

	/* For.Com_Model */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_ElementalSphere"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_MeshFX"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Elemental"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CElemental::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	ELEMENTAL_DESC* pDesc = static_cast<ELEMENTAL_DESC*>(pArg);
	m_pTargetTransformCom = pDesc->pTargetTransform;
	Safe_AddRef(m_pTargetTransformCom);

	m_fFrame = 0.f;
	m_bIntersect = FALSE;
	m_eAttribute = pDesc->eAttribute;

	switch (m_eAttribute)
	{
	case ELEMENT_FIRE:      // 불
		m_vAttColor = { 0.6500f, 0.2758f, 0.2758f, 1.0000f };
		break;
	case ELEMENT_WATER:     // 물
		m_vAttColor = { 0.2471f, 0.5019f, 0.9098f, 1.0000f };
		break;
	case ELEMENT_FROST:     // 얼음
		m_vAttColor = { 0.4471f, 0.7373f, 0.8000f, 1.0000f };
		break;
	case ELEMENT_ELECTRIC:  // 번개
		m_vAttColor = { 0.5953f, 0.2674f, 0.7377f, 1.0000f };
		break;
	case ELEMENT_WIND:      // 바람
		m_vAttColor = { 0.0000f, 0.5992f, 0.5177f, 1.0000f };
		break;
	case ELEMENT_EARTH:     // 바위
		m_vAttColor = { 0.5584f, 0.4282f, 0.0025f, 1.0000f };
		break;
	case ELEMENT_END:		// 무속성
		m_vAttColor = { 1.0000f, 1.0000f, 1.0000f, 1.0000f };
		break;
	}

	return S_OK;
}

HRESULT CElemental::LateTick(_float fTimeDelta)
{
	if (m_pGameInstance->PointInFrustum_InWorld(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
	}

	return S_OK;
}

void CElemental::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTargetTransformCom);
}
