#include "stdafx.h"
#include "SelRect.h"

#include "GameInstance.h"

#include "InteractionBox.h"
#include "AlphaRect.h"

#include "Player.h"
#include "Part_Status.h"

CSelRect::CSelRect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : COrtho(pDevice, pContext)
{
}

CSelRect::CSelRect(const CSelRect& rhs)
    : COrtho(rhs)
{
}

HRESULT CSelRect::Add_Component(void* pArg)
{
    /* For.Com_Shader */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    /* For.Com_Renderer */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
        TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
        return E_FAIL;

    /* For.Com_Transform */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
        return E_FAIL;

    /* For.Com_VIBuffer */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Select_Box"),
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    /* For.Com_Texture_Button */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Button"),
        TEXT("Com_Texture_Button"), (CComponent**)&m_pButtonTex)))
        return E_FAIL;

    /* For.Com_Texture_Rarity */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item_Rarity"),
        TEXT("Com_Texture_Rarity"), (CComponent**)&m_pRarityTexCom)))
        return E_FAIL;

    /* For.Com_Texture_Portrait */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Char_Portraits"),
        TEXT("Com_Texture_Portrait"), (CComponent**)&m_pPortraitTexCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CSelRect::Bind_ShaderResources()
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

HRESULT CSelRect::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CSelRect::Initialize_Clone(void* pArg)
{
     if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component(pArg)))
        return E_FAIL;

    COrtho::Initialize_Matrix();


    _float4x4 SelRectMat = { };
    XMStoreFloat4x4(&SelRectMat, XMMatrixIdentity());

    //BOX Dimentions
    //TOP
    SelRectMat._11 = 715.f; SelRectMat._22 = 66.f;
    SelRectMat._41 = static_cast<_float>(800.f - g_iWinSizeX / 2);
    SelRectMat._42 = static_cast<_float>(-(200.f - g_iWinSizeY / 2));
    m_vecSelRect.push_back(SelRectMat);
    //MID
    SelRectMat._11 = 715.f; SelRectMat._22 = 400.f;
    SelRectMat._41 = static_cast<_float>(800.f - g_iWinSizeX / 2);
    SelRectMat._42 = static_cast<_float>(-(433.f - g_iWinSizeY / 2));
    m_vecSelRect.push_back(SelRectMat);
    //BOTTOM
    SelRectMat._11 = 715.f; SelRectMat._22 = 66.f;
    SelRectMat._41 = static_cast<_float>(800.f - g_iWinSizeX / 2);
    SelRectMat._42 = static_cast<_float>(-(666.f - g_iWinSizeY / 2));
    m_vecSelRect.push_back(SelRectMat);

    //PLUS_MINUS BASE
    SelRectMat._11 = 530.f; SelRectMat._22 = 60.f;
    SelRectMat._41 = static_cast<_float>(800.f - g_iWinSizeX / 2);
    SelRectMat._42 = static_cast<_float>(-(492.f - g_iWinSizeY / 2));
    m_vecSelRect.push_back(SelRectMat);


    //TRUE BUTTON
    SelRectMat._11 = 236.f; SelRectMat._22 = 62.f;
    SelRectMat._41 = static_cast<_float>(665.f - g_iWinSizeX / 2);
    SelRectMat._42 = static_cast<_float>(-(591.f - g_iWinSizeY / 2));
    m_vecButton.push_back(SelRectMat);
    // 1040.f 500.f
    //FALSE BUTTON
    SelRectMat._11 = 236.f; SelRectMat._22 = 62.f;
    SelRectMat._41 = static_cast<_float>(957.f - g_iWinSizeX / 2);
    SelRectMat._42 = static_cast<_float>(-(591.f - g_iWinSizeY / 2));
    m_vecButton.push_back(SelRectMat);


    //PLUS BUTTON
    SelRectMat._11 = 36.f; SelRectMat._22 = 36.f;
    SelRectMat._41 = static_cast<_float>(1030.f - g_iWinSizeX / 2);
    SelRectMat._42 = static_cast<_float>(-(493.f - g_iWinSizeY / 2));
    m_vecButton.push_back(SelRectMat);


    //MINUS BUTTON
    SelRectMat._11 = 36.f; SelRectMat._22 = 36.f;
    SelRectMat._41 = static_cast<_float>(865.f - g_iWinSizeX / 2);
    SelRectMat._42 = static_cast<_float>(-(493.f - g_iWinSizeY / 2));
    m_vecButton.push_back(SelRectMat);


    //m_vecPortrait;
    SelRectMat._11 = 100.f; SelRectMat._22 = 120.f;
    SelRectMat._41 = static_cast<_float>(630.f - (g_iWinSizeX / 2));
    SelRectMat._42 = static_cast<_float>(-(360.f - g_iWinSizeY / 2));
    m_vecPortrait.push_back(SelRectMat);

    SelRectMat._11 = 100.f; SelRectMat._22 = 120.f;
    SelRectMat._41 = static_cast<_float>(750.f - (g_iWinSizeX / 2));
    SelRectMat._42 = static_cast<_float>(-(360.f - g_iWinSizeY / 2));
    m_vecPortrait.push_back(SelRectMat);

    SelRectMat._11 = 100.f; SelRectMat._22 = 120.f;
    SelRectMat._41 = static_cast<_float>(870.f - g_iWinSizeX / 2);
    SelRectMat._42 = static_cast<_float>(-(360.f - g_iWinSizeY / 2));
    m_vecPortrait.push_back(SelRectMat);

    SelRectMat._11 = 100.f; SelRectMat._22 = 120.f;
    SelRectMat._41 = static_cast<_float>(990.f - g_iWinSizeX / 2);
    SelRectMat._42 = static_cast<_float>(-(360.f - g_iWinSizeY / 2));
    m_vecPortrait.push_back(SelRectMat);



    //ScrollBar
    //ScrollButton

    m_eRenderType = RTYPE_UI;

    return S_OK;
}

HRESULT CSelRect::PreviousTick(_float fTimeDelta)
{
    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {
        if (FAILED(__super::PreviousTick(fTimeDelta)))
            return E_FAIL;
    }
    return S_OK;
}

HRESULT CSelRect::Tick(_float fTimeDelta)
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
        for (_int iIndex = 0; iIndex < m_vecButton.size(); ++iIndex)
        {
            rcDimentions.left = static_cast<LONG>(m_vecButton[iIndex]._41 - m_vecButton[iIndex]._11 / 2.f);
            rcDimentions.top = static_cast<LONG>(m_vecButton[iIndex]._42 - m_vecButton[iIndex]._22 / 2.f);
            rcDimentions.right = static_cast<LONG>(m_vecButton[iIndex]._41 + m_vecButton[iIndex]._11 / 2.f);
            rcDimentions.bottom = static_cast<LONG>(m_vecButton[iIndex]._42 + m_vecButton[iIndex]._22 / 2.f);

            if (PtInRect(&rcDimentions, ptMouse))
            {
                bOvered = TRUE;
                m_iMOverIndex = iIndex;
                if (m_pGameInstance->Key_Down(VK_LBUTTON) && bOvered == TRUE)
                {
                    m_iSelectedIdx = m_iMOverIndex;

                    if (m_eSelRectType == SELRECT_SHOP)
                    {
                        if (m_iSelectedIdx == 3)
                        {
                            if (m_iSelectedQty > 1)
                            {
                                m_iSelectedQty--;
                                m_iSelectedIdx = -1;

                            }
                        }
                        else if (m_iSelectedIdx == 2)
                        {
                            _int iPrice = m_pDisplayItem->Get_Item_Price();

                            if (iPrice * (m_iSelectedQty+1) <= m_iOwnedQty)
                            {
                                m_iSelectedQty++;
                                m_iSelectedIdx = -1;
                            }
                        }
                    }
                    else
                    {
                        if (m_iSelectedIdx == 3)
                        {
                            if (m_iSelectedQty > 1)
                            {
                                m_iSelectedQty--;
                                m_iSelectedIdx = -1;
                                
                            }
                        }
                        else if (m_iSelectedIdx == 2)
                        {
                            if (m_iSelectedQty < m_iOwnedQty)
                            {
                                m_iSelectedQty++;
                                m_iSelectedIdx = -1;
                            }
                        }
                    }

                }


                break;
            }
        }

        if (bOvered == FALSE)
            m_iMOverIndexChar = -1;

        for (_int iIndex = 0; iIndex < m_vecPortrait.size(); ++iIndex)
        {
            rcDimentions.left = static_cast<LONG>(m_vecPortrait[iIndex]._41 - m_vecPortrait[iIndex]._11 / 2.f);
            rcDimentions.top = static_cast<LONG>(m_vecPortrait[iIndex]._42 - m_vecPortrait[iIndex]._22 / 2.f);
            rcDimentions.right = static_cast<LONG>(m_vecPortrait[iIndex]._41 + m_vecPortrait[iIndex]._11 / 2.f);
            rcDimentions.bottom = static_cast<LONG>(m_vecPortrait[iIndex]._42 + m_vecPortrait[iIndex]._22 / 2.f);

            if (PtInRect(&rcDimentions, ptMouse))
            {
                bOvered = TRUE;
                m_iMOverIndexChar = iIndex;
                if (m_pGameInstance->Key_Down(VK_LBUTTON) && bOvered == TRUE)
                {
                    m_iSelectedCharIdx = m_iMOverIndexChar;
                }

                break;
            }
        }
    }

    return S_OK;
}

HRESULT CSelRect::LateTick(_float fTimeDelta)
{

    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {

        if (FAILED(__super::LateTick(fTimeDelta)))
            return E_FAIL;



    }
    return S_OK;
}

HRESULT CSelRect::Render()
{

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _float  fSlide = 20.f;
    _float4x4 CombinedMatrix = {};
    _float2 vUVPos = { 0.f,0.f };
    _float2 vUVScale = { 1.f,1.f };

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    CombinedMatrix = m_vecSelRect[0];

    switch (m_eSelRectColor)
    {
    case Client::CSelRect::SELRECT_NAVY:
        break;
    case Client::CSelRect::SELRECT_IVORY:
        break;
    case Client::CSelRect::SELRECT_END:
        break;
    default:
        break;
    }

    vUVPos = {0.f,0.f};
    vUVScale = {1.f,65.f/300.f};

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &vUVScale, sizeof(_float2))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &vUVPos, sizeof(_float2))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    CombinedMatrix = m_vecSelRect[1];
    vUVPos = { 0.f,65.f/300.f };
    vUVScale = { 1.f,20.f / 300.f };

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &vUVScale, sizeof(_float2))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &vUVPos, sizeof(_float2))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    CombinedMatrix = m_vecSelRect[2];
    vUVPos = { 0.f,85.f/300.f };
    vUVScale = { 1.f,65.f / 300.f };

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &vUVScale, sizeof(_float2))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &vUVPos, sizeof(_float2))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    //Buttons

    CombinedMatrix = m_vecButton[0];
    if (m_iMOverIndex == 0)
    {
        CombinedMatrix._11 *= 1.1f;
        CombinedMatrix._22 *= 1.1f;
    }
    vUVPos = { 0.f,0.f};
    vUVScale = { 1.f,77.f / 155.f };


    if (FAILED(m_pButtonTex->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &vUVScale, sizeof(_float2))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &vUVPos, sizeof(_float2))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    CombinedMatrix = m_vecButton[1];
    if (m_iMOverIndex == 1)
    {
        CombinedMatrix._11 *= 1.1f;
        CombinedMatrix._22 *= 1.1f;
    }


    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &vUVScale, sizeof(_float2))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &vUVPos, sizeof(_float2))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;


    if (m_eSelRectType == SELRECT_CONSUMABLE)
    {

        vUVPos = { 0.f,0.f };
        vUVScale = { 1.f,1.f };

        for (_int i = 0; i < PLAYER_END; i++)
        {

        CombinedMatrix = m_vecPortrait[i];

        if (i == 2)
        {
            if (FAILED(m_pRarityTexCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 3)))
                return E_FAIL;
        }
        else
        {
            if (FAILED(m_pRarityTexCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 4)))
                return E_FAIL;
        }

        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &vUVScale, sizeof(_float2))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &vUVPos, sizeof(_float2))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;


            CombinedMatrix._11 *= 0.9f;
            CombinedMatrix._22 *= 0.8f;
            CombinedMatrix._42 += 7.3f;
  
        if (FAILED(m_pPortraitTexCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", i)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &vUVScale, sizeof(_float2))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &vUVPos, sizeof(_float2))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;

        if (i == m_iSelectedCharIdx)
        {
            CombinedMatrix = m_vecPortrait[i];
            CombinedMatrix._11 += 10.f;
            CombinedMatrix._22 += 10.f;


        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 1)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;
        }


        //Additional Base

        //CombinedMatrix = m_vecSelRect[3];

        //if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 1)))
        //    return E_FAIL;

        //if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
        //    return E_FAIL;

        //if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
        //    return E_FAIL;

        //if (FAILED(m_pVIBufferCom->Render()))
        //    return E_FAIL;

        //PLUS


        CombinedMatrix = m_vecButton[2];
        if (m_iMOverIndex == 2)
        {
            CombinedMatrix._11 *= 1.1f;
            CombinedMatrix._22 *= 1.1f;
        }
        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 2)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;
        //MINUS

        CombinedMatrix = m_vecButton[3];
        if (m_iMOverIndex == 3)
        {
            CombinedMatrix._11 *= 1.1f;
            CombinedMatrix._22 *= 1.1f;
        }
        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 3)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;

        }

    }
    else if (m_eSelRectType == SELRECT_EQUIP)
    {

        vUVPos = { 0.f,0.f };
        vUVScale = { 1.f,1.f };

        for (_int i = 0; i < PLAYER_END; i++)
        {

            CombinedMatrix = m_vecPortrait[i];

            if (i == 2)
            {
                if (FAILED(m_pRarityTexCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 3)))
                    return E_FAIL;
            }
            else
            {
                if (FAILED(m_pRarityTexCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 4)))
                    return E_FAIL;
            }

            if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &vUVScale, sizeof(_float2))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &vUVPos, sizeof(_float2))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
                return E_FAIL;

            if (FAILED(m_pVIBufferCom->Render()))
                return E_FAIL;


            CombinedMatrix._11 *= 0.9f;
            CombinedMatrix._22 *= 0.8f;
            CombinedMatrix._42 += 7.3f;

            if (FAILED(m_pPortraitTexCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", i)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &vUVScale, sizeof(_float2))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &vUVPos, sizeof(_float2))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
                return E_FAIL;

            if (FAILED(m_pVIBufferCom->Render()))
                return E_FAIL;

            if (i == m_iSelectedCharIdx)
            {
                CombinedMatrix = m_vecPortrait[i];
                CombinedMatrix._11 += 10.f;
                CombinedMatrix._22 += 10.f;


                if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 1)))
                    return E_FAIL;

                if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
                    return E_FAIL;

                if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
                    return E_FAIL;

                if (FAILED(m_pVIBufferCom->Render()))
                    return E_FAIL;
            }


        }


    }
    else if (m_eSelRectType == SELRECT_SHOP)
    {

        vUVPos = { 0.f,0.f };
        vUVScale = { 1.f,1.f };

        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &vUVScale, sizeof(_float2))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &vUVPos, sizeof(_float2))))
            return E_FAIL;

        if (m_pDisplayItem != nullptr)
        {
            m_pDisplayItem->Render(_float2(0.f,100.f),_float2(100.f,128.f));

        }

            CombinedMatrix = m_vecButton[2];
            if (m_iMOverIndex == 2)
            {
                CombinedMatrix._11 *= 1.1f;
                CombinedMatrix._22 *= 1.1f;
            }
            if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 2)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
                return E_FAIL;

            if (FAILED(m_pVIBufferCom->Render()))
                return E_FAIL;
            //MINUS

            CombinedMatrix = m_vecButton[3];
            if (m_iMOverIndex == 3)
            {
                CombinedMatrix._11 *= 1.1f;
                CombinedMatrix._22 *= 1.1f;
            }
            if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 3)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
                return E_FAIL;

            if (FAILED(m_pVIBufferCom->Render()))
                return E_FAIL;


    }
    else if (m_eSelRectType == SELRECT_ALCHEMY)
    {

        vUVPos = { 0.f,0.f };
        vUVScale = { 1.f,1.f };

        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &vUVScale, sizeof(_float2))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &vUVPos, sizeof(_float2))))
            return E_FAIL;

        if (m_pDisplayItem != nullptr)
        {
            m_pDisplayItem->Render(_float2(0.f, 50.f), _float2(100.f, 128.f));

        }

    }
    
    //42,434 , 202,77 1000x1000

    //Render Text

    if (m_eSelRectType == SELRECT_PLAIN || m_eSelRectType== SELRECT_ALCHEMY)
    {
        m_pGameInstance->Render_Font(L"Font_Default", m_strText, { 545.f,285.f }, 0.7f, m_vTextColor, 0.f, { 0.f,0.f }, false);
    }
    else if (m_eSelRectType == SELRECT_CONSUMABLE)
    {
        m_strText = TEXT("사용할 캐릭터");
        m_pGameInstance->Render_Font(L"Font_Default", m_strText, { 720.f,247.f }, 0.7f, m_vTextColor, 0.f, { 0.f,0.f }, false);

        m_strText = TEXT("사용 수량");
        m_pGameInstance->Render_Font(L"Font_Default", m_strText, { 580.f,480.f }, 0.5f, Colors::White, 0.f, { 0.f,0.f }, false);

        _tchar szQty[MAX_PATH];
        wsprintf(szQty, TEXT("%d"), m_iSelectedQty);
        m_pGameInstance->Render_Font(L"Font_Default", szQty, { 940.f,480.f }, 0.5f, Colors::White, 0.f, { 0.f,0.f }, false);
        
    }
    else if (m_eSelRectType == SELRECT_EQUIP)
    {
        m_strText = TEXT("장착할 캐릭터");
        m_pGameInstance->Render_Font(L"Font_Default", m_strText, { 720.f,247.f }, 0.7f, m_vTextColor, 0.f, { 0.f,0.f }, false);
    }
    else if (m_eSelRectType == SELRECT_SHOP)
    {
        m_strText = TEXT("아이템 구매");
        m_pGameInstance->Render_Font(L"Font_Default", m_strText, { 720.f,247.f }, 0.7f, m_vTextColor, 0.f, { 0.f,0.f }, false);

        m_strText = TEXT("구매 수량");
        m_pGameInstance->Render_Font(L"Font_Default", m_strText, { 580.f,480.f }, 0.5f, Colors::White, 0.f, { 0.f,0.f }, false);

        _tchar szQty[MAX_PATH];
        wsprintf(szQty, TEXT("%d"), m_iSelectedQty);
        m_pGameInstance->Render_Font(L"Font_Default", szQty, { 940.f,480.f }, 0.5f, Colors::White, 0.f, { 0.f,0.f }, false);
    }
    //Button Text
        m_pGameInstance->Render_Font(L"Font_Default", TEXT("확인"), {640.f,575.f}, 0.7f, m_vColor, 0.f, {0.f,0.f}, false);
        m_pGameInstance->Render_Font(L"Font_Default", TEXT("취소"), {935.f,575.f}, 0.7f, m_vColor, 0.f, {0.f,0.f}, false);
    // NAVY = 62 69 86 
    // IVORY =  237 230 223

    return S_OK;
}


void CSelRect::Set_SelRectColor(_int iSelrectIdx)
{
    m_eSelRectColor = (SELRECT_COLOR)iSelrectIdx;

    switch (m_eSelRectColor)
    {
    case Client::CSelRect::SELRECT_NAVY:

        m_vTextColor = { 237.f / 255.f,230.f / 255.f,223.f / 255.f,1.f };
        m_vColor = { 62.f / 255.f,69.f / 255.f,86.f/255.f,1.f };

        break;
    case Client::CSelRect::SELRECT_IVORY:
        m_vTextColor = { 62.f / 255.f,69.f / 255.f,86.f / 255.f,1.f };
        m_vColor = { 237.f / 255.f,230.f / 255.f,223.f / 255.f,1.f };

        break;
    case Client::CSelRect::SELRECT_END:
        break;
    default:
        break;
    }




}

void CSelRect::Set_SelRectType(SELRECT_TPYE eType)
{
    m_eSelRectType = eType;
    if (m_eSelRectType == SELRECT_CONSUMABLE)
    {
        //CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
        //if (pPlayer == nullptr)
        //{
        //    MSG_BOX(TEXT("Failed to Find : Player"));
        //    return;
        //}
        //_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());
        //for (_int i = 0; i < PLAYER_FOUR; i++)
        //{
        //    CPart_Status* pStatus = static_cast<CPart_Status*>(static_cast<CPlayer*>(pPlayer)->Get_PartObject(CPlayer::PART_STATUS, iIndex));
        //    pStatus->Get_CurrentHp();
        //    pStatus->Get_MaxHp();
        //}
    }
}

CSelRect* CSelRect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSelRect* pInstance = new CSelRect(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CSelRect"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSelRect::Clone(void* pArg)
{
    CSelRect* pInstance = new CSelRect(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CSelRect"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSelRect::Free()
{
    __super::Free();

    if (m_bCloned)
    {

        Safe_Release(m_pButtonTex);
        Safe_Release(m_pRarityTexCom);
        Safe_Release(m_pPortraitTexCom);

    }
}
