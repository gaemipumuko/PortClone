#include "stdafx.h"
#include "TeamConfig_Page.h"

#include "GameInstance.h"

CTeamConfig_Page::CTeamConfig_Page(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : COrtho(pDevice, pContext)
{
}

CTeamConfig_Page::CTeamConfig_Page(const CTeamConfig_Page& rhs)
    : COrtho(rhs)
{
}

HRESULT CTeamConfig_Page::Add_Component(void* pArg)
{
    if (FAILED(__super::Add_Component(pArg)))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(CGameObject::Add_Component(m_tOrthoDesc.eTextureLevel, m_tOrthoDesc.strTextureTag,
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CTeamConfig_Page::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CTeamConfig_Page::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component(pArg)))
        return E_FAIL;

    COrtho::Initialize_Matrix();

    COrtho::ORTHO_DESC tOrthoDesc = { };
    tOrthoDesc.eTextureLevel = LEVEL_WORLD;
    tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_Paimon_Button");
    tOrthoDesc.fPosition = { static_cast<_float>(156.f - g_iWinSizeX / 2), static_cast<_float>(-(325.f - g_iWinSizeY / 2)) };
    tOrthoDesc.fSize = { 120.f, 98.f };
    if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, TEXT("Prototype_GameObject_Rect"), TEXT("Layer_Icon"), &tOrthoDesc)))
        return E_FAIL;


    return S_OK;
}

HRESULT CTeamConfig_Page::PreviousTick(_float fTimeDelta)
{
    if (FAILED(__super::PreviousTick(fTimeDelta)))
        return E_FAIL;

    return S_OK;
}

HRESULT CTeamConfig_Page::Tick(_float fTimeDelta)
{
    if (FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;

    return S_OK;
}

HRESULT CTeamConfig_Page::LateTick(_float fTimeDelta)
{
    if (FAILED(__super::LateTick(fTimeDelta)))
        return E_FAIL;
    
    return S_OK;
}

HRESULT CTeamConfig_Page::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

CTeamConfig_Page* CTeamConfig_Page::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTeamConfig_Page* pInstance = new CTeamConfig_Page(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CTeamConfig_Page"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTeamConfig_Page::Clone(void* pArg)
{
    CTeamConfig_Page* pInstance = new CTeamConfig_Page(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CTeamConfig_Page"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTeamConfig_Page::Free()
{
    __super::Free();
}
