#include "stdafx.h"
#include "UI_Page.h"

#include "GameInstance.h"

CPage::CPage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : COrtho(pDevice, pContext)
{
}

CPage::CPage(const CPage& rhs)
    : COrtho(rhs)
{
}

HRESULT CPage::Add_Component(void* pArg)
{
    if (FAILED(__super::Add_Component(pArg)))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(CGameObject::Add_Component(m_tOrthoDesc.eTextureLevel, m_tOrthoDesc.strTextureTag,
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CPage::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CPage::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component(pArg)))
        return E_FAIL;

    COrtho::Initialize_Matrix();


    return S_OK;
}

HRESULT CPage::PreviousTick(_float fTimeDelta)
{
    if (FAILED(__super::PreviousTick(fTimeDelta)))
        return E_FAIL;

    return S_OK;
}

HRESULT CPage::Tick(_float fTimeDelta)
{
    if (FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;

    return S_OK;
}

HRESULT CPage::LateTick(_float fTimeDelta)
{
    if (FAILED(__super::LateTick(fTimeDelta)))
        return E_FAIL;
    
    return S_OK;
}

HRESULT CPage::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(VTXTEX_ALPHA)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

CPage* CPage::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPage* pInstance = new CPage(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CPage"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPage::Clone(void* pArg)
{
    CPage* pInstance = new CPage(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CPage"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPage::Free()
{
    __super::Free();
}
