#include "Quest_Page.h"
#include "stdafx.h"
#include "Shop_Page.h"

#include "GameInstance.h"
#include "UI_Manager.h"
#include "Inventory.h"
#include "Item_Consumable.h"

CShop_Page::CShop_Page(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : COrtho(pDevice, pContext)
{
}

CShop_Page::CShop_Page(const CShop_Page& rhs)
    : COrtho(rhs)
{
}

HRESULT CShop_Page::Add_Component(void* pArg)
{
    if (FAILED(__super::Add_Component(pArg)))
        return E_FAIL;

    /* For.Com_Texture */

    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Shop_Elements"),
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    /* For.Com_Texture_Button */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Button"),
        TEXT("Com_Texture_Button"), (CComponent**)&m_pButtonTex)))
        return E_FAIL;

    return S_OK;
}

HRESULT CShop_Page::Bind_ShaderResources()
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

HRESULT CShop_Page::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CShop_Page::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component(pArg)))
        return E_FAIL;

    m_tOrthoDesc.fPosition = { 0.f, 0.f };
    m_tOrthoDesc.fSize = { static_cast<_float>(g_iWinSizeX), static_cast<_float>(g_iWinSizeY) };

    COrtho::Initialize_Matrix();    

    COrtho::ORTHO_DESC tOrthoDesc = { };
    tOrthoDesc.ePage = PAGE_SHOP;
    tOrthoDesc.iDepth = 2;
    m_pSelect_Box = (CSelRect*)m_pGameInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Select_Box"))->Clone(&tOrthoDesc);
    m_pSelect_Box->Set_SelRectType(CSelRect::SELRECT_SHOP);
    m_pSelect_Box->Set_SelRectColor(0);
    //Buttons
    _float4x4 IconMatrx = { };
    XMStoreFloat4x4(&IconMatrx, XMMatrixIdentity());

    //X
    IconMatrx._11 = 40; IconMatrx._22 = 40;
    IconMatrx._41 = static_cast<_float>(1535.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(40.f - g_iWinSizeY / 2));
    m_ButtonMat.push_back(IconMatrx);

    //Purchase
    IconMatrx._11 = 236.f; IconMatrx._22 = 62.f;
    IconMatrx._41 = static_cast<_float>(1370.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(800.f - g_iWinSizeY / 2));
    m_ButtonMat.push_back(IconMatrx);

    // O Decor
    IconMatrx._11 = 48.f; IconMatrx._22 = 48.f;
    IconMatrx._41 = static_cast<_float>(1290.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(802.f - g_iWinSizeY / 2));
    m_ButtonMat.push_back(IconMatrx);
    //Shop Slots

    _float4x4 SlotPosMat = { };
    XMStoreFloat4x4(&SlotPosMat, XMMatrixIdentity());

    for (_int iIndex = 0; iIndex < m_iNumItems; iIndex++)
    {

        SlotPosMat._11 = 800.f;
        SlotPosMat._22 = 100.f;
        SlotPosMat._41 = static_cast<_float>(450.f - g_iWinSizeX / 2);
        SlotPosMat._42 = static_cast<_float>(-(180.f + 120.f * iIndex - g_iWinSizeY / 2));


        m_SlotMatrix.push_back(SlotPosMat);
    }
    //Detail Boxes
    _float4x4 DetailBoxMat = { };
    XMStoreFloat4x4(&DetailBoxMat, XMMatrixIdentity());

    DetailBoxMat._11 = 410.f;
    DetailBoxMat._22 = 45.f;
    DetailBoxMat._41 = static_cast<_float>(1295.f - g_iWinSizeX / 2);
    DetailBoxMat._42 = static_cast<_float>(-(123.f - g_iWinSizeY / 2));
    m_DetailBoxMatrix.push_back(DetailBoxMat);

    DetailBoxMat._11 = 410.f;
    DetailBoxMat._22 = 170.f;
    DetailBoxMat._41 = static_cast<_float>(1295.f - g_iWinSizeX / 2);
    DetailBoxMat._42 = static_cast<_float>(-(230.f - g_iWinSizeY / 2));
    m_DetailBoxMatrix.push_back(DetailBoxMat);

    DetailBoxMat._11 = 410.f;
    DetailBoxMat._22 = 410.f;
    DetailBoxMat._41 = static_cast<_float>(1295.f - g_iWinSizeX / 2);
    DetailBoxMat._42 = static_cast<_float>(-(520.f - g_iWinSizeY / 2));
    m_DetailBoxMatrix.push_back(DetailBoxMat);

    DetailBoxMat._11 = 180.f;
    DetailBoxMat._22 = 170.f;
    DetailBoxMat._41 = static_cast<_float>(1410.f - g_iWinSizeX / 2);
    DetailBoxMat._42 = static_cast<_float>(-(230.f - g_iWinSizeY / 2));
    m_DetailBoxMatrix.push_back(DetailBoxMat);



    CItem_Consumable* pItem;
    pItem = static_cast<CItem_Consumable*>(CItem::Create_Item(151));
    m_ItemList.push_back(pItem);
    pItem = static_cast<CItem_Consumable*>(CItem::Create_Item(171));
    m_ItemList.push_back(pItem);
    pItem = static_cast<CItem_Consumable*>(CItem::Create_Item(152));
    m_ItemList.push_back(pItem);
    pItem = static_cast<CItem_Consumable*>(CItem::Create_Item(169));
    m_ItemList.push_back(pItem);
    pItem = static_cast<CItem_Consumable*>(CItem::Create_Item(154));
    m_ItemList.push_back(pItem);


    DetailBoxMat._11 = 150.f;
    DetailBoxMat._22 = 35.f;
    DetailBoxMat._41 = static_cast<_float>(1305.f - g_iWinSizeX / 2);
    DetailBoxMat._42 = static_cast<_float>(-(50.f - g_iWinSizeY / 2));
    m_DecorMat = DetailBoxMat;



    m_eRenderType = RENDER_TYPE::RTYPE_UI;

    return S_OK;
}

HRESULT CShop_Page::PreviousTick(_float fTimeDelta)
{
    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {

            if (FAILED(__super::PreviousTick(fTimeDelta)))
                return E_FAIL;

            if (m_bDisplaySelectBox == true)
                m_pSelect_Box->PreviousTick(fTimeDelta);

    }
    return S_OK;
}

HRESULT CShop_Page::Tick(_float fTimeDelta)
{
    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {
        CInventory* pInven = (CInventory*)m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Inventory"));
        m_iMora = pInven->Get_CurrentMora();

        if (m_pGameInstance->Key_Down(VK_LBUTTON))
        {
            m_pGameInstance->PlaySoundW(L"InteractionBox.mp3", 0.7f);
        }

        if (m_bDisplaySelectBox == true)
        {
            _int iBoxSelect = -1;

            m_pSelect_Box->Tick(fTimeDelta);
            iBoxSelect = m_pSelect_Box->Get_SelectedIdx();

            if (iBoxSelect == 1)
            {
                m_bDisplaySelectBox = false;
                m_pSelect_Box->Reset_Box();
            }
            else if (m_pSelect_Box->Get_SelectedIdx() == 0)
            {
                CItem* pDisplayItem = nullptr;
                _int   iItemPrice = {}, iItemD = {};

                auto& iter = m_ItemList.begin();
                if (m_ItemList.empty() != true)
                {
                    std::advance(iter, m_iSelectedIndex);
                }
                iItemPrice = (*iter)->Get_Item_Price();
                iItemD = (*iter)->Get_Item_ID();
                _int iQty = m_pSelect_Box->Get_SelectedQty();
                pInven->Use_Item(TEXT("모라"), iItemPrice*iQty);
                
                pInven->Add_Item(CItem::Create_Item(iItemD, iQty));

                m_bDisplaySelectBox = false;
                m_pSelect_Box->Reset_Box();

            }
        }
        else
        {

        if (FAILED(__super::Tick(fTimeDelta)))
             return E_FAIL;


        auto& iter = m_ItemList.begin();
        if (m_ItemList.empty() != true)
        {
            std::advance(iter, m_iSelectedIndex);
            m_iSelectedGrade = (*iter)->Get_Item_Rarity();
        }

        switch (m_iSelectedGrade)
        {
        case 0:
            m_vStripUVPos = { 0.f,0.7833f };
            m_vStripUVScale = { 1.f,0.1875f };
            m_vItemBgUVPos = { 0.5234375f , 0.484375f };
            m_vItemBgUVScale = { 0.3203125f,0.2578125f };
            break;
        case 1:
            m_vStripUVPos = { 0.f,0.5875f };
            m_vStripUVScale = { 1.f,0.1875f };
            m_vItemBgUVPos = { 0.f , 0.7421875f };
            m_vItemBgUVScale = { 0.2578125f,0.2578125f };
            break;
        case 2:
            m_vStripUVPos = { 0.f,0.39166f };
            m_vStripUVScale = { 1.f,0.1875f };
            m_vItemBgUVPos = { 0.f , 0.21875f };
            m_vItemBgUVScale = { 0.3203f,0.19531f };
            break;
        case 3:
            m_vStripUVPos = { 0.f,0.19583f };
            m_vStripUVScale = { 1.f,0.1875f };
            m_vItemBgUVPos = { 0.2578125f , 0.4765625f };
            m_vItemBgUVScale = { 0.265625f,0.265625f };
            break;
        case 4:
            m_vStripUVPos = { 0.f,0.f };
            m_vStripUVScale = { 1.f,0.1875f };
            m_vItemBgUVPos = { 0.5234375f , 0.75f };
            m_vItemBgUVScale = { .328125f,0.25f };
            break;
        default:
            break;
        }

        POINT       ptMouse;
        RECT        rcDimentions;

        GetCursorPos(&ptMouse);
        ScreenToClient(g_hWnd, &ptMouse);

        ptMouse.x = ptMouse.x - g_iWinSizeX / 2;
        ptMouse.y = (-ptMouse.y + g_iWinSizeY / 2);

        _bool bOvered = { FALSE };

        for (_int iSlot = 0; iSlot < m_iNumItems; ++iSlot)
        {
            rcDimentions.left = static_cast<LONG>(m_SlotMatrix[iSlot]._41 - m_SlotMatrix[iSlot]._11 / 2.f);
            rcDimentions.top = static_cast<LONG>(m_SlotMatrix[iSlot]._42 - m_SlotMatrix[iSlot]._22 / 2.f);
            rcDimentions.right = static_cast<LONG>(m_SlotMatrix[iSlot]._41 + m_SlotMatrix[iSlot]._11 / 2.f);
            rcDimentions.bottom = static_cast<LONG>(m_SlotMatrix[iSlot]._42 + m_SlotMatrix[iSlot]._22 / 2.f);

            if (PtInRect(&rcDimentions, ptMouse))
            {
                bOvered = TRUE;
                m_iMOverSlot = iSlot;


                if (m_pGameInstance->Key_Down(VK_LBUTTON))
                {
                    m_iSelectedIndex = m_iMOverSlot;
                }

            }
        }

        bOvered = { FALSE };
        m_iMOverIndex = -1;
        for (_uint iIndex = 0; iIndex < m_ButtonMat.size()-1; ++iIndex)
        {
            rcDimentions.left = static_cast<LONG>(m_ButtonMat[iIndex]._41 - m_ButtonMat[iIndex]._11 / 2.f);
            rcDimentions.top = static_cast<LONG>(m_ButtonMat[iIndex]._42 - m_ButtonMat[iIndex]._22 / 2.f);
            rcDimentions.right = static_cast<LONG>(m_ButtonMat[iIndex]._41 + m_ButtonMat[iIndex]._11 / 2.f);
            rcDimentions.bottom = static_cast<LONG>(m_ButtonMat[iIndex]._42 + m_ButtonMat[iIndex]._22 / 2.f);

            if (PtInRect(&rcDimentions, ptMouse))
            {
                bOvered = TRUE;
                m_iMOverIndex = iIndex;


                if (m_pGameInstance->Key_Down(VK_LBUTTON))
                {
                    m_iSelectedButton = m_iMOverIndex;

                }

            }
        }

        if (m_iSelectedButton == 0)
        {
            CUI_Manager* UI_Manager = (CUI_Manager*)m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_UIManager"));
            UI_Manager->Show_Page(PAGE_MAIN);
            m_iSelectedButton = -1;
        }
        if (m_iSelectedButton == 1)
        {
            CItem* pDisplayItem = nullptr;
            _int   iItemPrice = 0;

            auto& iter = m_ItemList.begin();
            if (m_ItemList.empty() != true)
            {
                std::advance(iter, m_iSelectedIndex);
            }
            iItemPrice = (*iter)->Get_Item_Price();

            if (iItemPrice <= m_iMora)
            {
                pDisplayItem = (*iter);
                m_bDisplaySelectBox = true;
                m_pSelect_Box->Set_DisplayItem(pDisplayItem);
                m_pSelect_Box->Set_OwnedQty(m_iMora);
            }
            m_iSelectedButton = -1;
        }

        }
    }

    return S_OK;
}

HRESULT CShop_Page::LateTick(_float fTimeDelta)
{
    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {

        if (FAILED(__super::LateTick(fTimeDelta)))
            return E_FAIL;

        if (m_bDisplaySelectBox == true)
            m_pSelect_Box->LateTick(fTimeDelta);

    }

    return S_OK;
}

HRESULT CShop_Page::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _float4x4 CombinedMatrix = {};

        _uint     iMainIconIndex = 0 ;

        //Render Top
        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
            return E_FAIL;
        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;
        

        CombinedMatrix = m_DecorMat;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            return E_FAIL;

        if (FAILED(m_pButtonTex->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
            return E_FAIL;

        _vector vUVPos = { 0.f,77.f / 155.f };
        _vector vUVScale = { 1.f,77.f / 155.f };

        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &vUVScale, sizeof(_float2))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &vUVPos, sizeof(_float2))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;

        CombinedMatrix._11 = 30.f;
        CombinedMatrix._22 = 30.f;
        CombinedMatrix._41 -= 58.f;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            return E_FAIL;

        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 5)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &m_vUVScale, sizeof(_float2))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &m_vUVPosition, sizeof(_float2))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;

        //보유한 모라
        
        _tchar szQty[MAX_PATH];
        wsprintf(szQty, TEXT("%d"), m_iMora);
        m_pGameInstance->Render_Font(L"Font_Number", szQty, { 1265.f,36.f }, 0.6f,Colors::White, 0.f, { 0.f,0.f }, false);


        //Render Buttons
         
        CombinedMatrix = m_ButtonMat[0];

        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            return E_FAIL;

        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 3)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &m_vUVScale, sizeof(_float2))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &m_vUVPosition, sizeof(_float2))))
            return E_FAIL;


        if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;

        CombinedMatrix = m_ButtonMat[1];
        if (m_iSelectedButton == 1)
        {
            CombinedMatrix._11 *= 1.05f;
            CombinedMatrix._22 *= 1.05f;

        }
        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            return E_FAIL;

        //SelRect Button
        if (FAILED(m_pButtonTex->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
            return E_FAIL;

        vUVPos = { 0.f,0.f };
        vUVScale = { 1.f,77.f / 155.f };

        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &vUVPos, sizeof(_float2))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &vUVScale, sizeof(_float2))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;

        m_pGameInstance->Render_Font(L"Font_Default", TEXT("구매한다"), { 1330.f,785.f }, 0.6f, { 62.f / 255.f,69.f / 255.f,86.f / 255.f,1.f }, 0.f, { 0.f,0.f }, false);

        CombinedMatrix = m_ButtonMat[2];

        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            return E_FAIL;

        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 4)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &m_vUVScale, sizeof(_float2))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &m_vUVPosition, sizeof(_float2))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;

        //Render Item BackGrounds

        for (_int i = 0; i < m_iNumItems; i++)
        {
            CombinedMatrix = m_SlotMatrix[i];

            if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
                return E_FAIL;

            if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 1)))
                return E_FAIL;
                
            if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &m_vUVScale, sizeof(_float2))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &m_vUVPosition, sizeof(_float2))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
                return E_FAIL;

            if (FAILED(m_pVIBufferCom->Render()))
                return E_FAIL;

            CombinedMatrix._11 = 100.f;
            CombinedMatrix._22 = 30.f;
            CombinedMatrix._41 += 310.f;
            CombinedMatrix._42 -= 20.f;

            if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
                return E_FAIL;

            if (FAILED(m_pButtonTex->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
                return E_FAIL;

            vUVPos = { 0.f,77.f / 155.f };
            vUVScale = { 1.f,77.f / 155.f };

            if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &vUVScale, sizeof(_float2))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &vUVPos, sizeof(_float2))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
                return E_FAIL;

            if (FAILED(m_pVIBufferCom->Render()))
                return E_FAIL;

            CombinedMatrix._11 = 25.f;
            CombinedMatrix._22 = 25.f;
            CombinedMatrix._41 -= 32.f;

            if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
                return E_FAIL;

            if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 5)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &m_vUVScale, sizeof(_float2))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &m_vUVPosition, sizeof(_float2))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
                return E_FAIL;

            if (FAILED(m_pVIBufferCom->Render()))
                return E_FAIL;

            _tchar szPrice[MAX_PATH];
            _int    iPrice = 0;
            
            auto& iter = m_ItemList.begin();
            if (m_ItemList.empty() != true)
            {
                std::advance(iter, i);
                iPrice = (*iter)->Get_Item_Price();
            }

            wsprintf(szPrice, TEXT("%d"), iPrice); 
            m_pGameInstance->Render_Font(L"Font_Number", szPrice, {738.f,189.f+i*120.f }, 0.5f, Colors::White, 0.f, { 0.f,0.f }, false);

        }

        //Render Items
        if (m_ItemList.empty() != true)
        {
            //7 8 9 6
            _float4x4 detailBoxmat = m_DetailBoxMatrix[0];
            if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", detailBoxmat)))
                return E_FAIL;

            if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 7)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &m_vStripUVScale, sizeof(_float2))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &m_vStripUVPos, sizeof(_float2))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
                return E_FAIL;

            if (FAILED(m_pVIBufferCom->Render()))
                return E_FAIL;

            detailBoxmat = m_DetailBoxMatrix[1];
            if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", detailBoxmat)))
                return E_FAIL;

            if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 8)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &m_vItemBgUVScale, sizeof(_float2))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &m_vItemBgUVPos, sizeof(_float2))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
                return E_FAIL;

            if (FAILED(m_pVIBufferCom->Render()))
                return E_FAIL;

            detailBoxmat = m_DetailBoxMatrix[1];
            if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", detailBoxmat)))
                return E_FAIL;

            if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 9)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &m_vUVScale, sizeof(_float2))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &m_vUVScale, sizeof(_float2))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
                return E_FAIL;

            if (FAILED(m_pVIBufferCom->Render()))
                return E_FAIL;

            detailBoxmat = m_DetailBoxMatrix[2];
            if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", detailBoxmat)))
                return E_FAIL;

            if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 6)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &m_vUVScale, sizeof(_float2))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &m_vUVPosition, sizeof(_float2))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
                return E_FAIL;

            if (FAILED(m_pVIBufferCom->Render()))
                return E_FAIL;

            //RENDER ITEM TEXTURE

            _float4x4 Slotmatrix = m_SlotMatrix[0];
            detailBoxmat = m_DetailBoxMatrix[3];
            _uint iSlotIndex = { 0 };
            for (auto& iter : m_ItemList)
            {
                Slotmatrix = m_SlotMatrix[iSlotIndex];
                iter->Render(_float2(Slotmatrix._41-347.f, Slotmatrix._42),_float2(100.f,100.f));
                wstring strName = iter->Get_Item_Name();

                m_pGameInstance->Render_Font(L"Font_Default", strName, { 180.f,148.f + iSlotIndex * 120.f }, 0.6f, Colors::White, 0.f, { 0.f,0.f }, false);

                if (m_iSelectedIndex == iSlotIndex)
                    iter->Render_Detail(detailBoxmat);

                iSlotIndex++;
            }

        }

    return S_OK;
}

CShop_Page* CShop_Page::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CShop_Page* pInstance = new CShop_Page(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CShop_Page"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CShop_Page::Clone(void* pArg)
{
    CShop_Page* pInstance = new CShop_Page(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CShop_Page"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CShop_Page::Free()
{
    __super::Free();

    if (m_bCloned)
    {
        for (auto& Items : m_ItemList)
            Safe_Release(Items);
        m_ItemList.clear();

        Safe_Release(m_pSelect_Box);
        Safe_Release(m_pButtonTex);
    }
}
