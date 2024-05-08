#include "stdafx.h"
#include "Effect.h"

#include "GameInstance.h"


CEffect::CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CDeciderObject(pDevice, pContext)
{
}

CEffect::CEffect(const CEffect& rhs)
	: CDeciderObject(rhs)
{
}

HRESULT CEffect::Add_Component(void* pArg)
{
	EFFECT_DESC* pDesc = static_cast<EFFECT_DESC*>(pArg);

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

HRESULT CEffect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CEffect::Tick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CEffect::LateTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CEffect::Render()
{
	return S_OK;
}

void CEffect::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
