#include "stdafx.h"
#include "Rect.h"

#include "GameInstance.h"

CRect::CRect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : COrtho(pDevice, pContext)
{
}

CRect::CRect(const CRect& rhs)
    : COrtho(rhs)
{
}

HRESULT CRect::Add_Component(void* pArg)
{
    if (FAILED(__super::Add_Component(pArg)))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(CGameObject::Add_Component(m_tOrthoDesc.eTextureLevel, m_tOrthoDesc.strTextureTag,
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CRect::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRect::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component(pArg)))
        return E_FAIL;

    COrtho::Initialize_Matrix();

    return S_OK;
}

HRESULT CRect::PreviousTick(_float fTimeDelta)
{
    if (m_bWindowTrue == true)
    {
        if (FAILED(__super::PreviousTick(fTimeDelta)))
            return E_FAIL;
    }
    return S_OK;
}

HRESULT CRect::Tick(_float fTimeDelta)
{
    if (m_bWindowTrue == true)
    {
        if (FAILED(__super::Tick(fTimeDelta)))
            return E_FAIL;
    }
    return S_OK;
}

HRESULT CRect::LateTick(_float fTimeDelta)
{
    if (m_bWindowTrue == true)
    {
        if (FAILED(__super::LateTick(fTimeDelta)))
            return E_FAIL;
    }
    return S_OK;
}

HRESULT CRect::Render()
{
    if (m_bWindowTrue == true)
    {
        if (FAILED(__super::Render()))
            return E_FAIL;
    }
    return S_OK;
}

CRect* CRect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CRect* pInstance = new CRect(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CRect"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CRect::Clone(void* pArg)
{
    CRect* pInstance = new CRect(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CRect"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRect::Free()
{
    __super::Free();
}
