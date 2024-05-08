#include "stdafx.h"
#include "PartObject.h"

#include "Creature.h"

#include "GameInstance.h"

CPartObject::CPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CDeciderObject(pDevice, pContext)
{
}

CPartObject::CPartObject(const CPartObject& rhs)
    : CDeciderObject(rhs)
{
}

void CPartObject::Set_Petrification(_bool bPertrification)
{
    m_bPetrification = bPertrification;
    if (m_bPetrification == FALSE)
        m_fPetrificationTime = 0.f;
}

void CPartObject::Set_Freezing(_bool bFreezing)
{
    m_bFreezing = bFreezing;
    if (m_bFreezing == FALSE)
        m_fFreezingTime = 0.f;
}

HRESULT CPartObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPartObject::Initialize_Clone(void* pArg) 
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    PARTOBJ_DESC* pDesc = static_cast<PARTOBJ_DESC*>(pArg);
    m_pParent = pDesc->pParent;

    /* For.Com_Texture */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dissolve"),
        TEXT("Com_Texture"), (CComponent**)&m_pDissolveCom)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Petrification"),
        TEXT("Com_Texture2"), (CComponent**)&m_pPetrificationCom)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Freezing"),
        TEXT("Com_Texture3"), (CComponent**)&m_pFreezingCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CPartObject::PreviousTick(_float fTimeDelta)
{
    return S_OK;
}

HRESULT CPartObject::Tick(_float fTimeDelta)
{
    return S_OK;
}

HRESULT CPartObject::LateTick(_float fTimeDelta)
{
    return S_OK;
}

HRESULT CPartObject::Render()
{
    return S_OK;
}

void CPartObject::Free()
{
    __super::Free();

    Safe_Release(m_pDissolveCom);
    Safe_Release(m_pPetrificationCom);
    Safe_Release(m_pFreezingCom);
}
