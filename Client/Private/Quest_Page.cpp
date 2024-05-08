#include "Quest_Page.h"
#include "stdafx.h"
#include "Quest_Page.h"

#include "GameInstance.h"
#include "UI_Manager.h"
#include "Inventory.h"

CQuest_Page::CQuest_Page(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : COrtho(pDevice, pContext)
{
}

CQuest_Page::CQuest_Page(const CQuest_Page& rhs)
    : COrtho(rhs)
{
}

HRESULT CQuest_Page::Add_Component(void* pArg)
{
    if (FAILED(__super::Add_Component(pArg)))
        return E_FAIL;

    /* For.Com_Texture */

    //if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Quest_Atlas"),
    //    TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
    //    return E_FAIL;


    return S_OK;
}

HRESULT CQuest_Page::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &m_vUVScale, sizeof(_float2))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &m_vUVPosition, sizeof(_float2))))
        return E_FAIL;

    return S_OK;
}

HRESULT CQuest_Page::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CQuest_Page::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component(pArg)))
        return E_FAIL;

    COrtho::Initialize_Matrix();


    _float4x4 IconMatrx = { };
    XMStoreFloat4x4(&IconMatrx, XMMatrixIdentity());

    /* Set Icon Translation */
    for (_uint iIndex = 0; iIndex < TAB_END-1; ++iIndex)
    {
        _float4x4 IconMatrix = { };
        XMStoreFloat4x4(&IconMatrix, XMMatrixIdentity());

        IconMatrix._11 = 48.f;
        IconMatrix._22 = 48.f;
        IconMatrix._41 = -static_cast<_float>(g_iWinSizeX) * 0.075f + iIndex * static_cast<_float>(g_iWinSizeX) * 0.05f;
        IconMatrix._42 = static_cast<_float>(g_iWinSizeY) * 0.46f;

        m_IconMatrix.push_back(IconMatrix);
    }


    m_eRenderType = RENDER_TYPE::RTYPE_UI;

    return S_OK;
}

HRESULT CQuest_Page::PreviousTick(_float fTimeDelta)
{
    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {

            if (FAILED(__super::PreviousTick(fTimeDelta)))
                return E_FAIL;

    }
    return S_OK;
}

HRESULT CQuest_Page::Tick(_float fTimeDelta)
{
    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {

        if (FAILED(__super::Tick(fTimeDelta)))
            return E_FAIL;

        POINT       ptMouse;
        RECT        rcDimentions;

        GetCursorPos(&ptMouse);
        ScreenToClient(g_hWnd, &ptMouse);

        ptMouse.x = ptMouse.x - g_iWinSizeX / 2;
        ptMouse.y = (-ptMouse.y + g_iWinSizeY / 2);

        _bool bOvered = { FALSE };
        for (_int iIndex = 0; iIndex < 1; ++iIndex)
        {
            rcDimentions.left = static_cast<LONG>(m_IconMatrix[iIndex + 2]._41 - m_IconMatrix[iIndex + 2]._11 / 2.f);
            rcDimentions.top = static_cast<LONG>(m_IconMatrix[iIndex + 2]._42 - m_IconMatrix[iIndex + 2]._22 / 2.f);
            rcDimentions.right = static_cast<LONG>(m_IconMatrix[iIndex + 2]._41 + m_IconMatrix[iIndex + 2]._11 / 2.f);
            rcDimentions.bottom = static_cast<LONG>(m_IconMatrix[iIndex + 2]._42 + m_IconMatrix[iIndex + 2]._22 / 2.f);

            if (PtInRect(&rcDimentions, ptMouse))
            {
                bOvered = TRUE;
                m_iMOverIndex = iIndex;
                break;
            }
        }
    }

    return S_OK;
}

HRESULT CQuest_Page::LateTick(_float fTimeDelta)
{
    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {

            if (FAILED(__super::LateTick(fTimeDelta)))
                return E_FAIL;

    }

    return S_OK;
}

HRESULT CQuest_Page::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _float4x4 CombinedMatrix = {};

        _uint     iMainIconIndex = 0 ;
        //Base

        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 1)))
            return E_FAIL;

        CombinedMatrix = m_IconMatrix[0];

        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;

        //Blue_Strip

        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
            return E_FAIL;

        CombinedMatrix = m_IconMatrix[1];


        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;


    return S_OK;
}

CQuest_Page* CQuest_Page::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CQuest_Page* pInstance = new CQuest_Page(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CQuest_Page"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CQuest_Page::Clone(void* pArg)
{
    CQuest_Page* pInstance = new CQuest_Page(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CQuest_Page"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CQuest_Page::Free()
{
    __super::Free();

    if (m_bCloned)
    {

    }
}
