#include "Quest_Page.h"
#include "stdafx.h"
#include "Alchemy_Page.h"

#include "GameInstance.h"
#include "UI_Manager.h"
#include "Inventory.h"
#include "Item_Material.h"

CAlchemy_Page::CAlchemy_Page(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : COrtho(pDevice, pContext)
{
}

CAlchemy_Page::CAlchemy_Page(const CAlchemy_Page& rhs)
    : COrtho(rhs)
{
}

HRESULT CAlchemy_Page::Add_Component(void* pArg)
{
    if (FAILED(__super::Add_Component(pArg)))
        return E_FAIL;

    /* For.Com_Texture */

    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Alchemy_Elements"),
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    /* For.Com_Texture_Button */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Button"),
        TEXT("Com_Texture_Button"), (CComponent**)&m_pButtonTex)))
        return E_FAIL;

    return S_OK;
}

HRESULT CAlchemy_Page::Bind_ShaderResources()
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

HRESULT CAlchemy_Page::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CAlchemy_Page::Initialize_Clone(void* pArg)
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
    m_pSelect_Box->Set_SelRectType(CSelRect::SELRECT_ALCHEMY);
    m_pSelect_Box->Set_BoxText(TEXT("다음 아이템의 조합을 진행하시겠습니까?"));
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

    // + Decor
    IconMatrx._11 = 48.f; IconMatrx._22 = 48.f;
    IconMatrx._41 = static_cast<_float>(1290.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(522.f - g_iWinSizeY / 2));
    m_ButtonMat.push_back(IconMatrx);   
    
    // - Decor
    IconMatrx._11 = 48.f; IconMatrx._22 = 48.f;
    IconMatrx._41 = static_cast<_float>(970.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(522.f - g_iWinSizeY / 2));
    m_ButtonMat.push_back(IconMatrx);


    //Shop Slots

    _float4x4 SlotPosMat = { };
    XMStoreFloat4x4(&SlotPosMat, XMMatrixIdentity());

    //Max Store = 20
    for (_int iIndex = 0; iIndex < 20; iIndex++)
    {

        SlotPosMat._11 = 100.f;
        SlotPosMat._22 = 128.f;
        SlotPosMat._41 = -static_cast<_float>(g_iWinSizeX) * 0.408f + iIndex % 4 * (SlotPosMat._11 + static_cast<_float>(g_iWinSizeX) * 0.014f);
        SlotPosMat._42 = static_cast<_float>(g_iWinSizeY) * 0.32f + iIndex / 4 * (SlotPosMat._22 + static_cast<_float>(g_iWinSizeY) * 0.02f) * -1.f;


        m_SlotMatrix.push_back(SlotPosMat);
    }


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


        CItem_Material* pItem;
        pItem = static_cast<CItem_Material*>(CItem::Create_Item(132));
        m_ItemList.push_back(pItem);
        pItem = static_cast<CItem_Material*>(CItem::Create_Item(133));
        m_ItemList.push_back(pItem);
        pItem = static_cast<CItem_Material*>(CItem::Create_Item(134));
        m_ItemList.push_back(pItem);
        pItem = static_cast<CItem_Material*>(CItem::Create_Item(136));
        m_ItemList.push_back(pItem);
        pItem = static_cast<CItem_Material*>(CItem::Create_Item(137));
        m_ItemList.push_back(pItem);
        pItem = static_cast<CItem_Material*>(CItem::Create_Item(138));
        m_ItemList.push_back(pItem);
        pItem = static_cast<CItem_Material*>(CItem::Create_Item(140));
        m_ItemList.push_back(pItem);
        pItem = static_cast<CItem_Material*>(CItem::Create_Item(141));
        m_ItemList.push_back(pItem);
        pItem = static_cast<CItem_Material*>(CItem::Create_Item(142));
        m_ItemList.push_back(pItem);
        pItem = static_cast<CItem_Material*>(CItem::Create_Item(144));
        m_ItemList.push_back(pItem);
        pItem = static_cast<CItem_Material*>(CItem::Create_Item(145));
        m_ItemList.push_back(pItem);
        pItem = static_cast<CItem_Material*>(CItem::Create_Item(146));
        m_ItemList.push_back(pItem);


        pItem = static_cast<CItem_Material*>(CItem::Create_Item(124));
        m_VecMaterials.push_back(pItem);
        pItem = static_cast<CItem_Material*>(CItem::Create_Item(124));
        m_VecMaterials.push_back(pItem);
        pItem = static_cast<CItem_Material*>(CItem::Create_Item(124));
        m_VecMaterials.push_back(pItem);

    m_ItemList.sort([](const CItem* Weapona, const CItem* Weaponb) {
        return Weapona->compareByItemVariable(Weaponb);
        });

    pItem = static_cast<CItem_Material*>(CItem::Create_Item(128));
    m_ItemList.push_back(pItem);

    DetailBoxMat._11 = 150.f;
    DetailBoxMat._22 = 35.f;
    DetailBoxMat._41 = static_cast<_float>(1305.f - g_iWinSizeX / 2);
    DetailBoxMat._42 = static_cast<_float>(-(50.f - g_iWinSizeY / 2));
    m_DecorMat.push_back(DetailBoxMat);

    DetailBoxMat._11 = 460.f;
    DetailBoxMat._22 = 460.f;
    DetailBoxMat._41 = static_cast<_float>(1105.f - g_iWinSizeX / 2);
    DetailBoxMat._42 = static_cast<_float>(-(300.f - g_iWinSizeY / 2));
    m_DecorMat.push_back(DetailBoxMat);

    //Box
    DetailBoxMat._11 = 800.f;
    DetailBoxMat._22 = 50.f;
    DetailBoxMat._41 = static_cast<_float>(1105.f - g_iWinSizeX / 2);
    DetailBoxMat._42 = static_cast<_float>(-(580.f - g_iWinSizeY / 2));
    m_DecorMat.push_back(DetailBoxMat);

    DetailBoxMat._11 = 100.f;
    DetailBoxMat._22 = 128.f;
    DetailBoxMat._41 = static_cast<_float>(1115.f - g_iWinSizeX / 2);
    DetailBoxMat._42 = static_cast<_float>(-(675.f - g_iWinSizeY / 2));
    m_DisplayMat = DetailBoxMat;

    m_eRenderType = RENDER_TYPE::RTYPE_UI;

    return S_OK;
}

HRESULT CAlchemy_Page::PreviousTick(_float fTimeDelta)
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

HRESULT CAlchemy_Page::Tick(_float fTimeDelta)
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

                m_pSelectedItem->Get_Item_ID();

                pInven->Add_Item(CItem::Create_Item(m_pSelectedItem->Get_Item_ID(), m_iCombineQty));
                pInven->Use_Item(TEXT("모라"), m_AlchemyCost * m_iCombineQty);
                pInven->Use_Item(m_iMtrlID0, m_iMtrlQty0* m_iCombineQty);
                m_iMtrl0Owned = pInven->Search_Inventory(m_iMtrlID0);
                if (m_iNumMaterials == 2)
                {
                    pInven->Use_Item(m_iMtrlID1, m_iMtrlQty1* m_iCombineQty);
                    m_iMtrl1Owned = pInven->Search_Inventory(m_iMtrlID1);
                }

                m_bDisplaySelectBox = false;
                m_pSelect_Box->Reset_Box();
                m_iCombineQty = 1;

            }
        }
        else
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

                    auto& iter = m_ItemList.begin();
                    if (m_ItemList.empty() != true)
                    {
                        std::advance(iter, m_iSelectedIndex);
                    }
                    m_pSelectedItem = (*iter);

                    Ready_Combination(m_pSelectedItem->Get_Item_ID());

                }

            }
        }

        bOvered = { FALSE };
        m_iMOverIndex = -1;
        for (_uint iIndex = 0; iIndex < m_ButtonMat.size(); ++iIndex)
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

            if (m_iMtrl0Owned >= m_iMtrlQty0 * m_iCombineQty && m_iMtrl1Owned >= m_iMtrlQty1 * m_iCombineQty && m_iMora >= m_AlchemyCost * m_iCombineQty)
            {
                pDisplayItem = (*iter);
                m_bDisplaySelectBox = true;
                m_pSelect_Box->Set_DisplayItem(pDisplayItem);
            }
            m_iSelectedButton = -1;
        }
        if (m_iSelectedButton == 3)
        {
            if(m_iMtrl0Owned>= m_iMtrlQty0* (m_iCombineQty+1) && m_iMtrl1Owned >= m_iMtrlQty1* (m_iCombineQty+1) && m_iMora>= m_AlchemyCost*(m_iCombineQty+1))
            m_iCombineQty++;

            m_iSelectedButton = -1;
        }
        if (m_iSelectedButton == 4)
        {
            if(m_iCombineQty >1)
            m_iCombineQty--;

            m_iSelectedButton = -1;
        }
        }
    }

    return S_OK;
}

HRESULT CAlchemy_Page::LateTick(_float fTimeDelta)
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

HRESULT CAlchemy_Page::Render()
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
        

        CombinedMatrix = m_DecorMat[0];

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

        CombinedMatrix = m_DecorMat[1];

        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            return E_FAIL;

        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 2)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &m_vUVScale, sizeof(_float2))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &m_vUVPosition, sizeof(_float2))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;

        //Selected Item

        if (m_pSelectedItem != nullptr)
        {
            m_pSelectedItem->Render(_float2(CombinedMatrix._41+5.f,CombinedMatrix._42));

            //Selected Item Materials

            CombinedMatrix = m_DisplayMat;
            if (m_iNumMaterials == 1)
            {
                m_VecMaterials[0]->Render(_float2(CombinedMatrix._41 - (CombinedMatrix._11/2.f)- 5.f, CombinedMatrix._42));
                m_VecMaterials[1]->Render(_float2(CombinedMatrix._41 + (CombinedMatrix._11 / 2.f) + 5.f, CombinedMatrix._42));


     
                wstring resultString = std::to_wstring(m_iMtrl0Owned) + L"/" + std::to_wstring(m_iMtrlQty0* m_iCombineQty);
                if(m_iMtrl0Owned< m_iMtrlQty0 * m_iCombineQty)
                    m_pGameInstance->Render_Font(L"Font_Number", resultString, { 1090 - (CombinedMatrix._11 / 2.f) - 5.f,716.f }, 0.55f, Colors::Red, 0.f, { 0.f,0.f }, false);
                else
                    m_pGameInstance->Render_Font(L"Font_Number", resultString, { 1090 - (CombinedMatrix._11 / 2.f) - 5.f,716.f }, 0.55f, Colors::White, 0.f, { 0.f,0.f }, false);

                _tchar szQty[MAX_PATH];
                wsprintf(szQty, TEXT("%d"), m_AlchemyCost* m_iCombineQty);
                if(m_iMora<m_AlchemyCost*m_iCombineQty)
                    m_pGameInstance->Render_Font(L"Font_Number", szQty, { 1090 + (CombinedMatrix._11 / 2.f) + 5.f,716.f }, 0.55f, Colors::Red, 0.f, { 0.f,0.f }, false);
                else
                    m_pGameInstance->Render_Font(L"Font_Number", szQty, { 1090 + (CombinedMatrix._11 / 2.f) + 5.f,716.f }, 0.55f, Colors::White, 0.f, { 0.f,0.f }, false);

            }
            if (m_iNumMaterials == 2)
            {
                m_VecMaterials[0]->Render(_float2(CombinedMatrix._41 - CombinedMatrix._11-5.f, CombinedMatrix._42));
                m_VecMaterials[1]->Render(_float2(CombinedMatrix._41, CombinedMatrix._42));
                m_VecMaterials[2]->Render(_float2(CombinedMatrix._41 + CombinedMatrix._11+5.f, CombinedMatrix._42));

                wstring resultString = std::to_wstring(m_iMtrl1Owned) + L"/" + std::to_wstring(m_iMtrlQty1 * m_iCombineQty);
                wstring resultString2 = std::to_wstring(m_iMtrl1Owned) + L"/" + std::to_wstring(m_iMtrlQty1 * m_iCombineQty);
                if (m_iMtrl0Owned < m_iMtrlQty0 * m_iCombineQty)
                    m_pGameInstance->Render_Font(L"Font_Number", resultString, { 1090.f - CombinedMatrix._11 - 5.f,716.f }, 0.6f, Colors::Red, 0.f, { 0.f,0.f }, false);
                else
                    m_pGameInstance->Render_Font(L"Font_Number", resultString, { 1090.f - CombinedMatrix._11 - 5.f,716.f }, 0.6f, Colors::White, 0.f, { 0.f,0.f }, false);
                if (m_iMtrl1Owned < m_iMtrlQty1 * m_iCombineQty)
                    m_pGameInstance->Render_Font(L"Font_Number", resultString2, { 1090.f,716.f }, 0.6f, Colors::Red, 0.f, { 0.f,0.f }, false);
                else
                    m_pGameInstance->Render_Font(L"Font_Number", resultString2, { 1090.f,716.f }, 0.6f, Colors::White, 0.f, { 0.f,0.f }, false);

                _tchar szQty[MAX_PATH];
                wsprintf(szQty, TEXT("%d"), m_AlchemyCost * m_iCombineQty);
                if (m_iMora < m_AlchemyCost * m_iCombineQty)
                    m_pGameInstance->Render_Font(L"Font_Number", szQty, { 1090.f + CombinedMatrix._11 + 5.f,716.f }, 0.6f, Colors::Red, 0.f, { 0.f,0.f }, false);
                else
                    m_pGameInstance->Render_Font(L"Font_Number", szQty, { 1090.f + CombinedMatrix._11 + 5.f,716.f }, 0.6f, Colors::White, 0.f, { 0.f,0.f }, false);
            }


        }


        //Box
        CombinedMatrix = m_DecorMat[2];

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

        m_pGameInstance->Render_Font(L"Font_Default", TEXT("합성 재료"), { 715.f,565.f }, 0.6f,Colors::White, 0.f, { 0.f,0.f }, false);

        //Render Buttons
         
        CombinedMatrix = m_ButtonMat[0];
        if (m_iMOverIndex == 0)
        {
            CombinedMatrix._11 *= 1.1f;
            CombinedMatrix._22 *= 1.1f;

        }

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
        if (m_iMOverIndex == 1)
        {
            CombinedMatrix._11 *= 1.1f;
            CombinedMatrix._22 *= 1.1f;

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

        m_pGameInstance->Render_Font(L"Font_Default", TEXT("조합한다"), { 1330.f,785.f }, 0.6f, { 62.f / 255.f,69.f / 255.f,86.f / 255.f,1.f }, 0.f, { 0.f,0.f }, false);

        CombinedMatrix = m_ButtonMat[2];
        if (m_iMOverIndex == 1)
        {
            CombinedMatrix._11 *= 1.1f;
            CombinedMatrix._22 *= 1.1f;

        }
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

        // +Button
        CombinedMatrix = m_ButtonMat[3];
        if (m_iMOverIndex == 3)
        {
            CombinedMatrix._11 *= 1.1f;
            CombinedMatrix._22 *= 1.1f;

        }
        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            return E_FAIL;

        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 7)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &m_vUVScale, sizeof(_float2))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &m_vUVPosition, sizeof(_float2))))
            return E_FAIL;


        if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;
        // -Button
        CombinedMatrix = m_ButtonMat[4];
        if (m_iMOverIndex == 4)
        {
            CombinedMatrix._11 *= 1.1f;
            CombinedMatrix._22 *= 1.1f;

        }
        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
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

        //QTY FONTS
        wsprintf(szQty, TEXT("%d"), m_iCombineQty);
        m_pGameInstance->Render_Font(L"Font_Number", szQty, { 1115.f,507.f }, 0.65f, Colors::Black, 0.f, { 0.f,0.f }, false);

        //Render Item BackGround


        //Render Items
        _uint iSlotIndex = { 0 };
        _float4x4 Slotmatrix = m_SlotMatrix[0];
        for (auto& iter : m_ItemList)
        {
            if (iSlotIndex == 12)
                continue;
            Slotmatrix = m_SlotMatrix[iSlotIndex];
            iter->Render(_float2(Slotmatrix._41, Slotmatrix._42));

            iSlotIndex++;
        }

    return S_OK;
}

void CAlchemy_Page::Ready_Combination(_int iItemID)
{
    CInventory* pInven = (CInventory*)m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Inventory"));
    pInven->Search_Inventory(1);

    switch (iItemID)
    {
    case 128:
        m_iNumMaterials = 2;
        m_iMtrlID0 = {126};
        m_iMtrlQty0 = { 1 };
        m_iMtrlID1 = {127};
        m_iMtrlQty1 = { 1 };
        m_AlchemyCost = 10000;
        break;
    case 132:
        m_iNumMaterials = 1;
        m_iMtrlID0 = 131;
        m_iMtrlQty0 =  3;
        m_AlchemyCost = 100;
        break;
    case 133:
        m_iNumMaterials = 1;
        m_iMtrlID0 = { 132 };
        m_iMtrlQty0 = 3;
        m_AlchemyCost = 100;
        break;
    case 134:
        m_iNumMaterials = 1;
        m_iMtrlID0 = { 133 };
        m_iMtrlQty0 = 3;
        m_AlchemyCost = 1000;
        break;
    case 136:
        m_iNumMaterials = 1;
        m_iMtrlID0 = { 135 };
        m_iMtrlQty0 = 3;
        m_AlchemyCost = 100;
        break;
    case 137:
        m_iNumMaterials = 1;
        m_iMtrlID0 = { 136 };
        m_iMtrlQty0 = 3;
        m_AlchemyCost = 100;
        break;
    case 138:
        m_iNumMaterials = 1;
        m_iMtrlID0 = { 137 };
        m_iMtrlQty0 = 3;
        m_AlchemyCost = 1000;
        break;
    case 140:
        m_iNumMaterials = 1;
        m_iMtrlID0 = 139;
        m_iMtrlQty0 = 3;
        m_AlchemyCost = 100;
        break;
    case 141:
        m_iNumMaterials = 1;
        m_iMtrlID0 = 140;
        m_iMtrlQty0 = 3;
        m_AlchemyCost = 100;
        break;
    case 142:
        m_iNumMaterials = 1;
        m_iMtrlID0 = 141;
        m_iMtrlQty0 = 3;
        m_AlchemyCost = 1000;
        break;
    case 144:
        m_iNumMaterials = 1;
        m_iMtrlID0 = 143;
        m_iMtrlQty0 = 3;
        m_AlchemyCost = 100;
        break;
    case 145:
        m_iNumMaterials = 1;
        m_iMtrlID0 = 144;
        m_iMtrlQty0 = 3;
        m_AlchemyCost = 100;
        break;
    case 146:
        m_iNumMaterials = 1;
        m_iMtrlID0 = 145;
        m_iMtrlQty0 = 3;
        m_AlchemyCost = 1000;
        break;
    default:
        m_iNumMaterials = 0;
        break;
    }


    m_VecMaterials[0]->Set_Current_Texture(m_iMtrlID0);
    m_VecMaterials[0]->Set_Grade(static_cast<CItem::ITEM_RARE>(m_pGameInstance->Find_JsonIntValue(L"Item_Info", vector<string>{to_string(m_iMtrlID0), "grade"})));
    m_iMtrl0Owned = pInven->Search_Inventory(m_iMtrlID0);

    if (m_iNumMaterials == 2)
    {
        m_VecMaterials[1]->Set_Current_Texture(m_iMtrlID1);
        m_VecMaterials[1]->Set_Grade(static_cast<CItem::ITEM_RARE>(m_pGameInstance->Find_JsonIntValue(L"Item_Info", vector<string>{to_string(m_iMtrlID1), "grade"})));
        m_iMtrl1Owned = pInven->Search_Inventory(m_iMtrlID1);
    }

    m_VecMaterials[m_iNumMaterials]->Set_Current_Texture(124);
    m_VecMaterials[m_iNumMaterials]->Set_Grade(CItem::ITEM_GRADE_RARE);
}

CAlchemy_Page* CAlchemy_Page::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CAlchemy_Page* pInstance = new CAlchemy_Page(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CAlchemy_Page"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CAlchemy_Page::Clone(void* pArg)
{
    CAlchemy_Page* pInstance = new CAlchemy_Page(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CAlchemy_Page"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CAlchemy_Page::Free()
{
    __super::Free();

    if (m_bCloned)
    {
        for (auto& Items : m_ItemList)
            Safe_Release(Items);
        m_ItemList.clear();

        for (auto& Items : m_VecMaterials)
            Safe_Release(Items);
        m_VecMaterials.clear();

        Safe_Release(m_pSelect_Box);
        Safe_Release(m_pButtonTex);
    }
}
