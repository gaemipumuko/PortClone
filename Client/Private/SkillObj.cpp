#include "stdafx.h"
#include "SkillObj.h"

#include "GameInstance.h"

CSkillObj::CSkillObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CDeciderObject(pDevice, pContext)
{
}

CSkillObj::CSkillObj(const CSkillObj& rhs)
	: CDeciderObject(rhs)
{
}

HRESULT CSkillObj::Add_Component(void* pArg)
{
	return S_OK;
}

HRESULT CSkillObj::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkillObj::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkillObj::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CSkillObj::Tick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CSkillObj::LateTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CSkillObj::Render()
{
	return S_OK;
}

CGameObject* CSkillObj::Clone(void* pArg)
{
	return nullptr;
}

HRESULT CSkillObj::Render_Shadow(_float4x4* pLightViewProj, _int iNumArray)
{
	return S_OK;
}

void CSkillObj::Free()
{
	__super::Free();
}