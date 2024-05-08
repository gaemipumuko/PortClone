#include "stdafx.h"
#include "Inventory.h"

#include "Item_Weapon.h"
#include "Item_Artifact.h" 
#include "Item_Consumable.h"
#include "Item_Material.h"


#include "UI_Manager.h"
#include "Page_Main.h"

#include "GameInstance.h"

CInventory::CInventory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: COrtho(pDevice, pContext)
{
}

CInventory::CInventory(const CInventory& rhs)
	: COrtho(rhs)
{
}

HRESULT CInventory::Add_Component(void* pArg)
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
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Inventory"),
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    /* For.Com_Texture_Button */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Button"),
        TEXT("Com_Texture_Button"), (CComponent**)&m_pButtonTex)))
        return E_FAIL;

	return S_OK;
}

HRESULT CInventory::Bind_ShaderResources()
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

HRESULT CInventory::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CInventory::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component(pArg)))
        return E_FAIL;

    COrtho::Initialize_Matrix();

    /* Set Icon Translation */
    for (_uint iIndex = 0; iIndex < RIV_END - 1; ++iIndex)
    {
        _float4x4 IconMatrix = { };
        XMStoreFloat4x4(&IconMatrix, XMMatrixIdentity());

        IconMatrix._11 = 48.f;
        IconMatrix._22 = 48.f;
        IconMatrix._41 = -static_cast<_float>(g_iWinSizeX) * 0.075f + iIndex * static_cast<_float>(g_iWinSizeX) * 0.05f;
        IconMatrix._42 = static_cast<_float>(g_iWinSizeY) * 0.46f;

        m_IconMatrix.push_back(IconMatrix);
    }

    _float4x4 DetailBoxMat = { };
    XMStoreFloat4x4(&DetailBoxMat, XMMatrixIdentity());

    DetailBoxMat._11 = 410.f;
    DetailBoxMat._22 = 45.f;
    DetailBoxMat._41 = static_cast<_float>(1295.f - g_iWinSizeX/2);
    DetailBoxMat._42 = static_cast<_float>(-(123.f - g_iWinSizeY/2));
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

    _float4x4 SelRectMat = { };
    XMStoreFloat4x4(&SelRectMat, XMMatrixIdentity());

    //BOX Dimentions
    //TOP
    SelRectMat._11 = 236.f; SelRectMat._22 = 62.f;
    SelRectMat._41 = static_cast<_float>(1370.f - g_iWinSizeX / 2);
    SelRectMat._42 = static_cast<_float>(-(800.f - g_iWinSizeY / 2));
    m_ButtonMat = SelRectMat;

    SelRectMat._11 = 48.f; SelRectMat._22 = 48.f;
    SelRectMat._41 = static_cast<_float>(1290.f - g_iWinSizeX / 2);
    SelRectMat._42 = static_cast<_float>(-(802.f - g_iWinSizeY / 2));
    m_ButtonIconMat = SelRectMat;

    /* Set Decider Info */
    m_eRenderType = RENDER_TYPE::RTYPE_UI;
    m_eUIState = RENDER_UI::RUI_INVEN;

    //for (_int iIndex = 0; iIndex < 56; ++iIndex)
    //     Add_Item(static_cast<CItem_Weapon*>(CItem::Create_Item(m_pGameInstance->Get_Int_Random(0, 59))));
    //for (_int iIndex = 0; iIndex < 32; ++iIndex)
    //    Add_Item(static_cast<CItem_Artifact*>(CItem::Create_Item(m_pGameInstance->Get_Int_Random(60, 109))));
    //for (_int iIndex = 0; iIndex < 3; ++iIndex)
    //    Add_Item(static_cast<CItem_Material*>(CItem::Create_Item(m_pGameInstance->Get_Int_Random(110, 112))));
    //for (_int iIndex = 0; iIndex < 25; ++iIndex)
    //    Add_Item(static_cast<CItem_Consumable*>(CItem::Create_Item(m_pGameInstance->Get_Int_Random(113, 123))));
    //for (_int iIndex = 0; iIndex < 25; ++iIndex)
    //    Add_Item(static_cast<CItem_Material*>(CItem::Create_Item(m_pGameInstance->Get_Int_Random(124, 125))));

    //m_WeaponStorage.sort([](const CItem* Weapona, const CItem* Weaponb) {
    //    return Weapona->compareByItemVariable(Weaponb);
    //    });
    //m_ArtifactStorage.sort([](const CItem* Itema, const CItem* Itemb) {
    //    return Itema->compareByItemVariable(Itemb);
    //    });
    //m_MaterialStorage.sort([](const CItem* Itema, const CItem* Itemb) {
    //    return Itema->compareByItemVariable(Itemb);
    //    });
    //m_CookingStorage.sort([](const CItem* Itema, const CItem* Itemb) {
    //    return Itema->compareByItemVariable(Itemb);
    //    });
    
    _float4x4 SlotPosMat = { };
    XMStoreFloat4x4(&DetailBoxMat, XMMatrixIdentity());

    //Max Store = 100
    for (_int iIndex = 0; iIndex < 100; iIndex++)
    {

        SlotPosMat._11 = 100.f;
        SlotPosMat._22 = 128.f;
        SlotPosMat._41 = -static_cast<_float>(g_iWinSizeX) * 0.408f + iIndex % 8 * (SlotPosMat._11 + static_cast<_float>(g_iWinSizeX) * 0.014f);
        SlotPosMat._42 = static_cast<_float>(g_iWinSizeY) * 0.32f + iIndex / 8 * (SlotPosMat._22 + static_cast<_float>(g_iWinSizeY) * 0.02f) * -1.f;


        m_SlotMatrix.push_back(SlotPosMat);
    }

    COrtho::ORTHO_DESC tOrthoDesc = { };
    tOrthoDesc.ePage = PAGE_INVEN;
    tOrthoDesc.iDepth = 2;
    m_pSelect_Box = (CSelRect*)m_pGameInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Select_Box"))->Clone(&tOrthoDesc);

    return S_OK;
}

HRESULT CInventory::PreviousTick(_float fTimeDelta)
{

    if (m_pGameInstance->Key_Down(VK_F4))
    {
        CUI_Manager* pUI_Manager = (CUI_Manager*)m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_UIManager"));
        pUI_Manager->Show_Page(PAGE_MAIN);

    }

    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {


        if (m_bShowSelectBox == true)
            m_pSelect_Box->PreviousTick(fTimeDelta);
    }

	return S_OK;
}

HRESULT CInventory::Tick(_float fTimeDelta)
{
    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {
        _int iSelect = 0;

        if (m_pGameInstance->Key_Down(VK_LBUTTON))
        {
            m_pGameInstance->PlaySoundW(L"InteractionBox.mp3", 0.7f);
        }


        if (m_bShowSelectBox == true)
        {
            _int iBoxSelect = -1;

            m_pSelect_Box->Tick(fTimeDelta);
            iBoxSelect = m_pSelect_Box->Get_SelectedIdx();

            if (iBoxSelect == 1)
            {
                m_bShowSelectBox = false;
                m_pSelect_Box->Reset_Box();
            }
            else if (m_pSelect_Box->Get_SelectedIdx() == 0)
            {
            
                _int iCharIdx = m_pSelect_Box->Get_SelectedCharIdx();
                _int iQty = m_pSelect_Box->Get_SelectedQty();

            if (m_eRenderInvenType == RIV_COOKING)
            {
                auto& iter = m_CookingStorage.begin();
                m_iNumCookingStorage = static_cast<_uint>(m_CookingStorage.size());
                m_iNumRows = m_iNumCookingStorage / 8;
                m_iNumCurrentStorage = m_iNumCookingStorage;
                if (m_CookingStorage.empty() != true)
                {
                    std::advance(iter, m_iSelectedIndex);
                    (*iter)->Use_item(iQty, (PLAYERINDEX)iCharIdx);

                }
                m_bShowSelectBox = false;
                m_pSelect_Box->Reset_Box();
            }
                
            }
        }
        else
        {

        //m_fScrollView -> Single Row = 15.f
        if (m_pGameInstance->Mouse_Move(MMS_WHEEL) > 0)
        {
            //Wheel UP
                m_fScrollView -= 2.f;

            if (m_fScrollView <= -2.f)
            {
                m_fScrollView += 2.f;
            }

        }
        else if (m_pGameInstance->Mouse_Move(MMS_WHEEL) < 0)
        {
            //Wheel Down
            m_fScrollView += 2.f;

            if (m_fScrollView >= m_iNumRows*15.f)
            {
                m_fScrollView -= 2.f;
            }

        }

        //CombinedMatrix._41 = -static_cast<_float>(g_iWinSizeX) * 0.408f + iSlotIndex % 8 * (CombinedMatrix._11 + static_cast<_float>(g_iWinSizeX) * 0.014f);
    //CombinedMatrix._42 = static_cast<_float>(g_iWinSizeY) * 0.32f + iSlotIndex / 8 * (CombinedMatrix._22 + static_cast<_float>(g_iWinSizeY) * 0.02f) * -1.f
    //	+ fScrollView * 10.f;

        // 각 위치 갱신
        m_bDisplayButton = false;

        if (m_eRenderInvenType == RIV_WEAPON)
        {
            auto& iter = m_WeaponStorage.begin();
            m_iNumWeaponStorage = static_cast<_uint>(m_WeaponStorage.size());
            m_iNumCurrentStorage = m_iNumWeaponStorage;
            m_iNumRows = m_iNumWeaponStorage / 8;

            for (auto& iter : m_WeaponStorage)
            {
                if (iter->Get_item_Qty() < 0)
                {
                    Safe_Release(iter);
                }
            }
            if(m_WeaponStorage.empty() !=true)
            {
            std::advance(iter, m_iSelectedIndex);
            m_iSelectedGrade = (*iter)->Get_Item_Rarity();
                m_bDisplayButton = true;
            }
        }
        else if (m_eRenderInvenType == RIV_ARTIFACT)
        {
            auto& iter = m_ArtifactStorage.begin();
            m_iNumArtifactStorage = static_cast<_uint>(m_ArtifactStorage.size());
            m_iNumRows = m_iNumArtifactStorage / 8;
            m_iNumCurrentStorage = m_iNumArtifactStorage;
            for (auto& iter : m_ArtifactStorage)
            {
                if (iter->Get_item_Qty() < 0)
                {
                    Safe_Release(iter);
                }
            }
            if (m_ArtifactStorage.empty() != true)
            {
                std::advance(iter, m_iSelectedIndex);
                m_iSelectedGrade = (*iter)->Get_Item_Rarity();
                m_bDisplayButton = true;
            }
        }
        else if (m_eRenderInvenType == RIV_MATERIAL)
        {
            m_iNumCookingStorage = static_cast<_uint>(m_MaterialStorage.size());
            m_iNumRows = m_iNumCookingStorage / 8;
            m_iNumCurrentStorage = m_iNumCookingStorage;
            for (auto it = m_MaterialStorage.begin(); it != m_MaterialStorage.end();)
            {
                if ((*it)->Get_item_Qty() <= 0) {
                    Safe_Release(*it);
                    it = m_MaterialStorage.erase(it);
                    m_iSelectedIndex = 0;
                }
                else {
                    ++it;
                }
            }
            auto& iter = m_MaterialStorage.begin();
            m_iNumMaterialStorage = static_cast<_uint>(m_MaterialStorage.size());
            m_iNumRows = m_iNumMaterialStorage / 8;
            m_iNumCurrentStorage = m_iNumMaterialStorage;
            for (auto& iter : m_MaterialStorage)
            {
                if (iter->Get_item_Qty() < 0)
                {
                    Safe_Release(iter);
                }
            }
            if (m_MaterialStorage.empty() != true)
            {
                std::advance(iter, m_iSelectedIndex);
                m_iSelectedGrade = (*iter)->Get_Item_Rarity();
                m_bDisplayButton = false;
            }
        }
        else if (m_eRenderInvenType == RIV_COOKING)
        {
            m_iNumCookingStorage = static_cast<_uint>(m_CookingStorage.size());
            m_iNumRows = m_iNumCookingStorage / 8;
            m_iNumCurrentStorage = m_iNumCookingStorage;
            for (auto it = m_CookingStorage.begin(); it != m_CookingStorage.end();)
            {
                if ((*it)->Get_item_Qty()<=0) {
                    Safe_Release(*it);
                    it = m_CookingStorage.erase(it);
                    m_iSelectedIndex = 0;
                }
                else {
                    ++it;
                }
            }
            auto& iter = m_CookingStorage.begin();
            if (m_CookingStorage.empty() != true)
            {
                std::advance(iter, m_iSelectedIndex);
                m_iSelectedGrade = (*iter)->Get_Item_Rarity();
                m_bDisplayButton = true;
            }
        }
        else
        {
            return E_FAIL;
        }


        //상단 아이콘 체크 

        POINT       ptMouse;
        RECT        rcDimentions;

        GetCursorPos(&ptMouse);
        ScreenToClient(g_hWnd, &ptMouse);

        ptMouse.x = ptMouse.x - g_iWinSizeX / 2;
        ptMouse.y = (-ptMouse.y + g_iWinSizeY / 2);

        _bool bOvered = { FALSE };
        for (_int iIndex = 0; iIndex < RIV_END -1; ++iIndex)
        {
            rcDimentions.left = static_cast<LONG>(m_IconMatrix[iIndex]._41 - m_IconMatrix[iIndex]._11 / 2.f);
            rcDimentions.top = static_cast<LONG>(m_IconMatrix[iIndex]._42 - m_IconMatrix[iIndex]._22 / 2.f);
            rcDimentions.right = static_cast<LONG>(m_IconMatrix[iIndex]._41 + m_IconMatrix[iIndex]._11 / 2.f);
            rcDimentions.bottom = static_cast<LONG>(m_IconMatrix[iIndex]._42 + m_IconMatrix[iIndex]._22 / 2.f);

            if (PtInRect(&rcDimentions, ptMouse))
            {
                bOvered = TRUE; 
                m_iMOverIndex = iIndex;


                if (m_pGameInstance->Key_Down(VK_LBUTTON))
                {
                    switch (m_iMOverIndex)
                    {
                    case 0:
                        m_eRenderInvenType = RIV_WEAPON; 
                        m_iSelectedIndex = 0;
                        m_fScrollView = 0.f;
                        m_iNumCurrentStorage = m_iNumWeaponStorage;
                        break;
                    case 1:
                        m_eRenderInvenType = RIV_ARTIFACT;
                        m_iSelectedIndex = 0;
                        m_fScrollView = 0.f;
                        m_iNumCurrentStorage = m_iNumArtifactStorage;
                        break;
                    case 2:
                        m_eRenderInvenType = RIV_MATERIAL;
                        m_iSelectedIndex = 0;
                        m_fScrollView = 0.f;
                        m_iNumCurrentStorage = m_iNumMaterialStorage;
                        break;
                    case 3:
                        m_eRenderInvenType = RIV_COOKING;
                        m_iSelectedIndex = 0;
                        m_fScrollView = 0.f;
                        m_iNumCurrentStorage = m_iNumCookingStorage;
                        break;
                    default:
                        break;
                    }

                }

            }
        }
        if (bOvered == FALSE)
            m_iMOverIndex = -1;

        bOvered = { FALSE };
        for (_uint iSlot = 0; iSlot < m_iNumCurrentStorage; ++iSlot)
        {
            rcDimentions.left = static_cast<LONG>(m_SlotMatrix[iSlot]._41 - m_SlotMatrix[iSlot]._11 / 2.f);
            rcDimentions.top = static_cast<LONG>(m_SlotMatrix[iSlot]._42+ m_fScrollView*10.f - m_SlotMatrix[iSlot]._22 / 2.f);
            rcDimentions.right = static_cast<LONG>(m_SlotMatrix[iSlot]._41 + m_SlotMatrix[iSlot]._11 / 2.f);
            rcDimentions.bottom = static_cast<LONG>(m_SlotMatrix[iSlot]._42 + m_fScrollView *10.f + m_SlotMatrix[iSlot]._22 / 2.f);

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



        //사용 버튼
        if (m_bDisplayButton == true)
        {

             if (bOvered == FALSE)
                 m_iMOverIndex = -1;

                 bOvered = { FALSE };
                 m_bOver = { FALSE };
             
                 rcDimentions.left = static_cast<LONG>(m_ButtonMat._41 - m_ButtonMat._11 / 2.f);
                 rcDimentions.top = static_cast<LONG>(m_ButtonMat._42 - m_ButtonMat._22 / 2.f);
                 rcDimentions.right = static_cast<LONG>(m_ButtonMat._41 + m_ButtonMat._11 / 2.f);
                 rcDimentions.bottom = static_cast<LONG>(m_ButtonMat._42 + m_ButtonMat._22 / 2.f);

                 if (PtInRect(&rcDimentions, ptMouse))
                 {
                     bOvered = TRUE;
                     m_bOver = TRUE;


                     if (m_pGameInstance->Key_Down(VK_LBUTTON))
                     {
                         _int iQty = {};
                         if (m_eRenderInvenType == RIV_COOKING)
                         {

                             auto& iter = m_CookingStorage.begin();
                             if (m_CookingStorage.empty() != true)
                             {
                                 std::advance(iter, m_iSelectedIndex);
                                 iQty = (*iter)->Get_item_Qty();

                             }
                         }

                         switch (m_eRenderInvenType)
                         {
                         case Client::CInventory::RIV_WEAPON:
                             m_bShowSelectBox = true;
                             m_pSelect_Box->Set_SelRectColor(0);
                             m_pSelect_Box->Set_SelRectType(CSelRect::SELRECT_EQUIP);
                             break;
                         case Client::CInventory::RIV_ARTIFACT:
                             m_bShowSelectBox = true;
                             m_pSelect_Box->Set_SelRectColor(0);
                             m_pSelect_Box->Set_SelRectType(CSelRect::SELRECT_EQUIP);
                             break;
                         case Client::CInventory::RIV_MATERIAL:
                             break;
                         case Client::CInventory::RIV_COOKING:
                             m_bShowSelectBox = true;
                             m_pSelect_Box->Set_SelRectColor(0);
                             m_pSelect_Box->Set_OwnedQty(iQty);
                             m_pSelect_Box->Set_SelRectType(CSelRect::SELRECT_CONSUMABLE);
                             break;
                         case Client::CInventory::RIV_END:
                             break;
                         default:
                             break;
                         }
    
                     }

                 }
        
        }

        //상세창 배경색 선택
        /*
            fUVPos = {0.f,0.39166f};
            fUVScale = {1.f,0.1875f};
        */
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
            m_vStripUVPos = {0.f,0.f};
            m_vStripUVScale = {1.f,0.1875f};
            m_vItemBgUVPos = { 0.5234375f , 0.75f};
            m_vItemBgUVScale = { .328125f,0.25f};
            break;
        default:
            break;
        }

        }
    }

    return S_OK;
}

HRESULT CInventory::LateTick(_float fTimeDelta)
{
    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {
        m_pRendererCom->Add_RenderGroup(CRenderer::RG_POPUPUI, this);

        if (m_bShowSelectBox == true)
            m_pSelect_Box->LateTick(fTimeDelta);
    }

    return S_OK;
}

HRESULT CInventory::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    /* Render Inventory */
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderCurrentTexture(m_pShaderCom, "g_Texture")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    /* Render Icon */
    _ubyte byIndex = { 1 };
    for (auto& IconMatrix : m_IconMatrix)
    {
        if (m_eRenderInvenType == byIndex)
        {
            _float4x4 IconBGMatrix = IconMatrix;
            IconBGMatrix._11 = 64.f;
            IconBGMatrix._22 = 64.f;
            if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", IconBGMatrix)))
                return E_FAIL;

            if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 5)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
                return E_FAIL;

            if (FAILED(m_pVIBufferCom->Render()))
                return E_FAIL;
        }

        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", IconMatrix)))
            return E_FAIL;

        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", byIndex++)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;
    }

    if (m_bDisplayButton == true)
    {


        _float4x4 CombinedMatrix = m_ButtonMat;
        if (m_bOver == true)

        {
            CombinedMatrix._11 *= 1.1f;
            CombinedMatrix._22 *= 1.1f;
        }

        if (FAILED(m_pButtonTex->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            return E_FAIL;

        _vector vUVPos = { 0.f,0.f };
        _vector vUVScale = { 1.f,77.f / 155.f };

        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &vUVScale, sizeof(_float2))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &vUVPos, sizeof(_float2))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;

        CombinedMatrix = m_ButtonIconMat;
        _float fFontSize = { 0.6f };
        if (m_bOver == true)
        {
            fFontSize *= 1.1f;
            CombinedMatrix._11 *= 1.1f;
            CombinedMatrix._22 *= 1.1f;

        }

        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 10)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            return E_FAIL;

        vUVPos = { 0.f,0.f };
        vUVScale = { 1.f,1.f };

        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &vUVScale, sizeof(_float2))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &vUVPos, sizeof(_float2))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;


        m_pGameInstance->Render_Font(L"Font_Default", TEXT("사용한다"), { 1330.f,785.f }, fFontSize, { 62.f / 255.f,69.f / 255.f,86.f / 255.f,1.f }, 0.f, { 0.f,0.f }, false);
    }

    /* Render Item */
    _uint iSlotIndex = { 0 };
    switch (m_eRenderInvenType)
    {
    case RIV_WEAPON:

        if (m_WeaponStorage.empty() != true)
        {
            //7 8 9 6
            _float2 fUVPos, fUVScale;
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

            fUVPos = { 0.f,0.21875f };
            fUVScale = { 0.3203f,0.19531f };
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
            for (auto& iter : m_WeaponStorage)
            {
                Slotmatrix = m_SlotMatrix[iSlotIndex];
                Slotmatrix._42 += m_fScrollView * 10.f;
                iter->Render(_float2(Slotmatrix._41, Slotmatrix._42));

                if (m_iSelectedIndex == iSlotIndex)
                iter->Render_Detail(detailBoxmat);

                iSlotIndex++;
            }

        }

        break;

    case RIV_ARTIFACT:
        if (m_ArtifactStorage.empty() != true)
        {
            //7 8 9 6
            _float2 fUVPos, fUVScale;
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

            fUVPos = { 0.f,0.21875f };
            fUVScale = { 0.3203f,0.19531f };
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
            for (auto& iter : m_ArtifactStorage)
            {
                Slotmatrix = m_SlotMatrix[iSlotIndex];
                Slotmatrix._42 += m_fScrollView * 10.f;
                iter->Render(_float2(Slotmatrix._41, Slotmatrix._42));

                if (m_iSelectedIndex == iSlotIndex)
                    iter->Render_Detail(detailBoxmat);

                iSlotIndex++;
            }

        }
        break;

    case RIV_MATERIAL:
        if (m_MaterialStorage.empty() != true)
        {
            //7 8 9 6
            _float2 fUVPos, fUVScale;
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

            fUVPos = { 0.f,0.21875f };
            fUVScale = { 0.3203f,0.19531f };
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
            for (auto& iter : m_MaterialStorage)
            {
                Slotmatrix = m_SlotMatrix[iSlotIndex];
                Slotmatrix._42 += m_fScrollView * 10.f;
                iter->Render(_float2(Slotmatrix._41, Slotmatrix._42));

                if (m_iSelectedIndex == iSlotIndex)
                    iter->Render_Detail(detailBoxmat);

                iSlotIndex++;
            }

        }
        break;

    case RIV_COOKING:
        if (m_CookingStorage.empty() != true)
        {
            //7 8 9 6
            _float2 fUVPos, fUVScale;
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

                fUVPos = { 0.f,0.21875f };
                fUVScale = { 0.3203f,0.19531f };
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
                for (auto& iter : m_CookingStorage)
                {
                    Slotmatrix = m_SlotMatrix[iSlotIndex];
                    Slotmatrix._42 += m_fScrollView * 10.f;
                    iter->Render(_float2(Slotmatrix._41, Slotmatrix._42));

                    if (m_iSelectedIndex == iSlotIndex)
                        iter->Render_Detail(detailBoxmat);

                    iSlotIndex++;
                }



        }
        break;

       
    }



    return S_OK;
}

void CInventory::Open_Inventory()
{
    CUI_Manager* pUI_Manager = (CUI_Manager*)m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_UIManager"));

    pUI_Manager->Show_Page(PAGE_INVEN);

    m_fScrollView = 0.f;
    m_iSelectedIndex = 0;
    m_eRenderInvenType = RIV_WEAPON;
    m_iNumCurrentStorage = m_iNumWeaponStorage;

    m_WeaponStorage.sort([](const CItem* Weapona, const CItem* Weaponb) {
        return Weapona->compareByItemVariable(Weaponb);
        });
}

_int CInventory::Search_Inventory(wstring strItemName)
{
    _int iQty = 0;

    for (auto& iter : m_MaterialStorage)
    {
        if (iter->Get_Item_Name() == strItemName)
        {
            iQty = iter->Get_item_Qty();
            return iQty;
        }
    }

    for (auto& iter : m_CookingStorage)
    {
        if (iter->Get_Item_Name() == strItemName)
        {
            iQty = iter->Get_item_Qty();
            return iQty;
        }
    }

    return iQty;
}

_int CInventory::Search_Inventory(_int iItemID)
{
    _int iQty = 0;

    for (auto& iter : m_MaterialStorage)
    {
        if (iter->Get_Item_ID() == iItemID)
        {
            iQty = iter->Get_item_Qty();
            return iQty;
        }
    }

    for (auto& iter : m_CookingStorage)
    {
        if (iter->Get_Item_ID() == iItemID)
        {
            iQty = iter->Get_item_Qty();
            return iQty;
        }
    }

    return iQty;
}

void CInventory::Add_Item(CItem* pItem)
{
    CItem::ITEM_TYPE eItemType;

    CUI_Manager* pUI_Manager = (CUI_Manager*)m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_UIManager"));
    CPage_Main* pPage = (CPage_Main*)pUI_Manager->Get_UI_Page(PAGE_MAIN);
    _int iQty = pItem->Get_item_Qty();

    pPage->Register_On_AcquiredItems(pItem);

    eItemType = pItem->Get_Item_Type();

    if (pItem->Get_Item_Name() == TEXT("원석"))
    {
        m_iNumGems += pItem->Get_item_Qty();
    }

    if (pItem->Get_Item_Name() == TEXT("모라"))
    {
        m_iNumMora += pItem->Get_item_Qty();
    }

    if (eItemType == CItem::ITEM_TYPE_WEAPON)
    {
        m_WeaponStorage.push_back(static_cast<CItem_Weapon*>(pItem));
    }
    else if (eItemType == CItem::ITEM_TYPE_ARTIFACT)
    {
        m_ArtifactStorage.push_back(static_cast<CItem_Artifact*>(pItem));
    }
    else if (eItemType == CItem::ITEM_TYPE_EXPBOOK)
    {
        for (auto& iter : m_MaterialStorage)
        {
            if (iter->Get_Item_Name() == pItem->Get_Item_Name())
            {
                iter->Stash_Item(iQty);
                Safe_Release(pItem);
                return;
            }
        }
        m_MaterialStorage.push_back(static_cast<CItem_Material*>(pItem));
    }
    else if (eItemType == CItem::ITEM_TYPE_COOKING)
    {
        for (auto& iter : m_CookingStorage)
        {
            if (iter->Get_Item_Name() == pItem->Get_Item_Name())
            {
                iter->Stash_Item(iQty);
                Safe_Release(pItem);
                return;
            }

        }

            m_CookingStorage.push_back(static_cast<CItem_Consumable*>(pItem));
    }


}

void CInventory::Use_Item(wstring strItemName, _int iQty)
{
    for (auto& iter : m_MaterialStorage)
    {
        if (iter->Get_Item_Name() == strItemName)
        {
            iter->Use_item(iQty);
            return;
        }
    }
}

void CInventory::Use_Item(_int iItemID, _int iQty)
{
    for (auto& iter : m_MaterialStorage)
    {
        if (iter->Get_Item_ID() == iItemID)
        {
            iter->Use_item(iQty);
            return;
        }
    }
    for (auto& iter : m_CookingStorage)
    {
        if (iter->Get_Item_ID() == iItemID)
        {
            iter->Use_item(iQty);
            return;
        }
    }
}

_int CInventory::Get_CurrentMora()
{
    for (auto& iter : m_MaterialStorage)
    {
        if (iter->Get_Item_Name() == L"모라")
        {
            return iter->Get_item_Qty();
        }
    }

    return 0;
}

CInventory* CInventory::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CInventory* pInstance = new CInventory(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CInventory"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CInventory::Clone(void* pArg)
{
    CInventory* pInstance = new CInventory(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CInventory"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CInventory::Free()
{
    __super::Free();

    if (m_bCloned)
    {
    for (auto& Weapon : m_WeaponStorage)
        Safe_Release(Weapon);
    m_WeaponStorage.clear();
    for (auto& Artifact : m_ArtifactStorage)
        Safe_Release(Artifact);
    m_ArtifactStorage.clear();
    for (auto& Material : m_MaterialStorage)
        Safe_Release(Material);
    m_MaterialStorage.clear();
    for (auto& Consumable : m_CookingStorage)
        Safe_Release(Consumable);
    m_CookingStorage.clear();

        Safe_Release(m_pButtonTex);
        Safe_Release(m_pSelect_Box);
    }
}
