#include "stdafx.h"
#include "Gacha_Result_Page.h"

#include "GameInstance.h"
#include "UI_Manager.h"

#include "UI_DualTex.h"
#include "Inventory.h"

CGacha_Result_Page::CGacha_Result_Page(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : COrtho(pDevice, pContext)
{
}

CGacha_Result_Page::CGacha_Result_Page(const CGacha_Result_Page& rhs)
    : COrtho(rhs)
{
}

HRESULT CGacha_Result_Page::Add_Component(void* pArg)
{
    if (FAILED(__super::Add_Component(pArg)))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Gacha_Result_Bg"),
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CGacha_Result_Page::Bind_ShaderResources()
{

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_ViewMatrix)))
        return E_FAIL;
    
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
        return E_FAIL;
    
    return S_OK;

}

HRESULT CGacha_Result_Page::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CGacha_Result_Page::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component(pArg)))
        return E_FAIL;

    COrtho::Initialize_Matrix();

    CUI_DualTex* m_pDualTexRect;
    CUI_DualTex::DUALTEX_DESC tOrthoDesc = { };
    
    for (int i = 0; i < 10; i++)
    {
        tOrthoDesc.eTextureLevel = LEVEL_STATIC;
        tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_Gacha_Result_Base");
        tOrthoDesc.fPosition = { 0.f,0.f };
        tOrthoDesc.fSize = { 120.f, 460.f };
        tOrthoDesc.ePage = PAGE_GACHA_RESULT;
        tOrthoDesc.iDepth = 2;
        tOrthoDesc.strDiffTextureTag = TEXT("Prototype_Component_Texture_Gacha_Avatar_Bg");
        m_pDualTexRect = (CUI_DualTex*)m_pGameInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_UI_Dual_Tex"))->Clone(&tOrthoDesc);
        m_pTex_List.push_back(m_pDualTexRect);
    }

    m_eRenderType = RENDER_TYPE::RTYPE_UI;

    return S_OK;
}

HRESULT CGacha_Result_Page::PreviousTick(_float fTimeDelta)
{
    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {
        if (FAILED(__super::PreviousTick(fTimeDelta)))
            return E_FAIL;
    }
    return S_OK;
}

HRESULT CGacha_Result_Page::Tick(_float fTimeDelta)
{


    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {
        if (FAILED(__super::Tick(fTimeDelta)))
            return E_FAIL;

        for (auto iter : m_pTex_List)
        {
            iter->Tick(fTimeDelta);
        }

        if (m_fDelayTime <= 0.f || m_pGameInstance->Key_Down(VK_LBUTTON))
        {

            if (m_bPlayingVideo == true)
            {
                MCIWndClose(m_hVideo);
                SelectObject(m_BackDC, m_hOldBackBit); //DC에 원래 설정을 돌려줍니다.
                DeleteDC(m_BackDC);  // 메모리를 반환합니다.
                DeleteObject(m_hBackBit); // 메모리를 반환합니다.
                ReleaseDC(g_hWnd, m_DC); // 윈도우에 DC 해제를 요청합니다.
                m_pGameInstance->Set_PlayVideo(false);
                m_pGameInstance->StopSound(SOUND_CH0);
                m_bPlayingVideo = false;

            }

        }

        if (m_bPlayingVideo == true)
        {
            m_fDelayTime -= fTimeDelta;
        }
        else
        {
            _int iIndex = 0;

            if (m_pTex_List.empty() == false)
            {

                for (auto iter : m_pTex_List)
                {
                    iter->In_From_Right(static_cast<_float>(248.f + (120.f * iIndex) - g_iWinSizeX / 2), 15.f);
                    iIndex++;
                }
            }
        }




        POINT       ptMouse;
        RECT        rcDimentions, rcbtn;

        GetCursorPos(&ptMouse);

        ScreenToClient(g_hWnd, &ptMouse);

        //ptMouse.x = ptMouse.x - g_iWinSizeX / 2;
        //ptMouse.y = (-ptMouse.y + g_iWinSizeY / 2);

        rcDimentions.left = static_cast<LONG>(m_tOrthoDesc.fPosition.x - m_tOrthoDesc.fSize.x / 2.f);
        rcDimentions.top = static_cast<LONG>(m_tOrthoDesc.fPosition.y - m_tOrthoDesc.fSize.y / 2.f);
        rcDimentions.right = static_cast<LONG>(m_tOrthoDesc.fPosition.x + m_tOrthoDesc.fSize.x / 2.f);
        rcDimentions.bottom = static_cast<LONG>(m_tOrthoDesc.fPosition.y + m_tOrthoDesc.fSize.y / 2.f);

        rcbtn.left = static_cast<LONG>(1520.f);
        rcbtn.top = static_cast<LONG>(40.f);
        rcbtn.right = static_cast<LONG>(1560.f);
        rcbtn.bottom = static_cast<LONG>(80.f);

        if (m_pGameInstance->Key_Down(VK_LBUTTON) && m_bPlayingVideo == false)
        {

        if (PtInRect(&rcbtn, ptMouse))
        {
            CUI_Manager* UI_Manager = (CUI_Manager*)m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_UIManager"));
            UI_Manager->Show_Page(PAGE_GACHA);

            // CUI_Manager::Get_Instance()->Close_Page(PAGE_GACHA_RESULT);
            // CUI_Manager::Get_Instance()->Show_Page(PAGE_GACHA);

            m_pGameInstance->StopSound(SOUND_BGM);
            m_pGameInstance->PlayBGM(L"Gacha_Bgm.mp3");
        }

        }

        if (m_pGameInstance->Key_Down('I'))
        {

            ShowResults(m_pGameInstance->Get_Int_Random(0, 100));
        }
    }
    return S_OK;
}

HRESULT CGacha_Result_Page::LateTick(_float fTimeDelta)
{

    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {
        if (FAILED(__super::LateTick(fTimeDelta)))
            return E_FAIL;

        for (auto iter : m_pTex_List)
        {
            iter->LateTick(fTimeDelta);
        }
    }
    return S_OK;
}

HRESULT CGacha_Result_Page::Render()
{
    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {
        if (FAILED(__super::Bind_ShaderResources()))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;

    }
    return S_OK;
}

void CGacha_Result_Page::ShowResults(_uint iDice)
{
    RollDice(iDice);

    if (iDice >= 95)
    {
        m_fDelayTime = 6.5f;

        m_bPlayingVideo = true;

        m_pGameInstance->Set_PlayVideo(true);
    
        m_DC = GetDC(g_hWnd);
    
        m_BackDC = CreateCompatibleDC(m_DC);
    
        m_hBackBit = CreateCompatibleBitmap(m_DC, g_iWinSizeX, g_iWinSizeY);
    
        m_hOldBackBit = (HBITMAP)SelectObject(m_BackDC, m_hBackBit);
    
        m_hVideo = MCIWndCreate(g_hWnd, NULL, WS_CHILD | WS_VISIBLE | MCIWNDF_NOPLAYBAR
            , L"../../Reference/Resources/Video/Rare.wmv");
    
        MCIWndSetVolume(g_hWnd, 0.5f);

        m_pGameInstance->StopSound(SOUND_BGM);
        m_pGameInstance->PlaySoundW(L"Rare.mp3", 0.5f, Engine::SOUND_CH0);
    
        MoveWindow(m_hVideo, 0, 0, g_iWinSizeX, g_iWinSizeY, FALSE);
    
        MCIWndPlay(m_hVideo);
    }
    else
    {
        m_fDelayTime = 6.5f;

        m_bPlayingVideo = true;

        m_pGameInstance->Set_PlayVideo(true);

        m_DC = GetDC(g_hWnd);

        m_BackDC = CreateCompatibleDC(m_DC);

        m_hBackBit = CreateCompatibleBitmap(m_DC, g_iWinSizeX, g_iWinSizeY);

        m_hOldBackBit = (HBITMAP)SelectObject(m_BackDC, m_hBackBit);

        m_hVideo = MCIWndCreate(g_hWnd, NULL, WS_CHILD | WS_VISIBLE | MCIWNDF_NOPLAYBAR
            , L"../../Reference/Resources/Video/General.wmv");

        MCIWndSetVolume(g_hWnd, 0.5f);
        m_pGameInstance->StopSound(SOUND_BGM);
        m_pGameInstance->PlaySoundW(L"General.mp3", 0.5f, Engine::SOUND_CH0);

        MoveWindow(m_hVideo, 0, 0, g_iWinSizeX, g_iWinSizeY, FALSE);

        MCIWndPlay(m_hVideo);
    }
}

void CGacha_Result_Page::RollDice(_uint iDice)
{

    _uint iDice9, iIndex, iItemIndex;

    list<_uint>     Dice_List;

    // CUI_DualTex* m_pDualTexRect;

    if (iDice <= 80)
    {
        iDice = m_pGameInstance->Get_Int_Random(80, 94);
    }

    Dice_List.push_back(iDice);

    for (int i = 0; i < 9; i++)
    {
        iDice9 = m_pGameInstance->Get_Int_Random(0, 100);

        Dice_List.push_back(iDice9);
    }

    Dice_List.sort(greater<>());

    iIndex = 0;
    iItemIndex = 0;

    CInventory* pInven = (CInventory*)m_pGameInstance->Find_Object(LEVEL_STATIC,TEXT("Layer_Inventory"));

    std::list<_uint>::iterator iter = Dice_List.begin();
    std::list<CUI_DualTex*>::iterator Tex_iter = m_pTex_List.begin();

    for (int i = 0; i < 10; i++)
    {
        if ((*iter) >= 95)
        {
            (*Tex_iter)->SetRadianceColor(COLOR_LEGENDARY);
            (*Tex_iter)->SetTextureIndex(m_pGameInstance->Get_Int_Random(70, 75));

        }
        else if ((*iter) >= 90&& (*iter)<95)
        {
            (*Tex_iter)->SetRadianceColor(COLOR_LEGENDARY);
            iIndex = m_pGameInstance->Get_Int_Random(0, 16);
            switch (iIndex)
            {
            case 0:
                iItemIndex = 9;
                break;
            case 1:
                iItemIndex = 10;
                break;
            case 2:
                iItemIndex = 21;
                break;
            case 3:
                iItemIndex = 22;
                break;
            case 4:
                iItemIndex = 23;
                break;
            case 5:
                iItemIndex = 33;
                break;
            case 6:
                iItemIndex = 34;
                break;
            case 7:
                iItemIndex = 35;
                break;
            case 8:
                iItemIndex = 36;
                break;
            case 9:
                iItemIndex = 46;
                break;
            case 10:
                iItemIndex = 47;
                break;
            case 11:
                iItemIndex = 48;
                break;
            case 12:
                iItemIndex = 49;
                break;
            case 13:
                iItemIndex = 57;
                break;
            case 14:
                iItemIndex = 58;
                break;
            case 15:
                iItemIndex = 59;
                break;
            default:
                break;
            }

           (*Tex_iter)->SetTextureIndex(iItemIndex);
            pInven->Add_Item(CItem::Create_Item(iItemIndex));  
        }
        else if ((*iter) < 90 && (*iter)>=80)
        {
            (*Tex_iter)->SetRadianceColor(COLOR_EPIC);
            iIndex = m_pGameInstance->Get_Int_Random(60, 69);
            (*Tex_iter)->SetTextureIndex(iIndex);
        }
        else if ((*iter) < 80 && (*iter) >= 60)
        {
            //From Here
            (*Tex_iter)->SetRadianceColor(COLOR_EPIC);
            iIndex = m_pGameInstance->Get_Int_Random(0, 15);
            switch (iIndex)
            {
            case 0:
                iItemIndex = 5;
                break;
            case 1:
                iItemIndex = 6;
                break;
            case 2:
                iItemIndex = 7;
                break;
            case 3:
                iItemIndex = 8;
                break;
            case 4:
                iItemIndex = 18;
                break;
            case 5:
                iItemIndex = 19;
                break;
            case 6:
                iItemIndex = 20;
                break;
            case 7:
                iItemIndex = 29;
                break;
            case 8:
                iItemIndex = 30;
                break;
            case 9:
                iItemIndex = 31;
                break;
            case 10:
                iItemIndex = 32;
                break;
            case 11:
                iItemIndex = 43;
                break;
            case 12:
                iItemIndex = 44;
                break;
            case 13:
                iItemIndex = 45;
                break;
            case 14:
                iItemIndex = 55;
                break;
            case 15:
                iItemIndex = 56;
                break;
            default:
                break;
            }

            (*Tex_iter)->SetTextureIndex(iItemIndex);
            pInven->Add_Item(CItem::Create_Item(iItemIndex));
        }
        else if ((*iter) < 60 && (*iter) >= 40)
        {
            (*Tex_iter)->SetRadianceColor(COLOR_RARE);
            iIndex = m_pGameInstance->Get_Int_Random(0, 15);
            switch (iIndex)
            {
            case 0:
                iItemIndex = 2;
                break;
            case 1:
                iItemIndex = 3;
                break;
            case 2:
                iItemIndex = 4;
                break;
            case 3:
                iItemIndex = 13;
                break;
            case 4:
                iItemIndex = 14;
                break;
            case 5:
                iItemIndex = 15;
                break;
            case 6:
                iItemIndex = 16;
                break;
            case 7:
                iItemIndex = 17;
                break;
            case 8:
                iItemIndex = 26;
                break;
            case 9:
                iItemIndex = 27;
                break;
            case 10:
                iItemIndex = 28;
                break;
            case 11:
                iItemIndex = 39;
                break;
            case 12:
                iItemIndex = 42;
                break;
            case 13:
                iItemIndex = 52;
                break;
            case 14:
                iItemIndex = 53;
                break;
            case 15:
                iItemIndex = 54;
                break;
            default:
                break;
            }

            (*Tex_iter)->SetTextureIndex(iItemIndex);
            pInven->Add_Item(CItem::Create_Item(iItemIndex));

            //2,3,4,13-17 26-28 39-42  52-54
        }
        else if ((*iter) < 40 && (*iter) >= 20)
        {
            (*Tex_iter)->SetRadianceColor(COLOR_UNCOMMON);
            iIndex = m_pGameInstance->Get_Int_Random(0, 4);
            switch (iIndex)
            {
            case 0:
                iItemIndex = 1;
                break;
            case 1:
                iItemIndex = 12;
                break;
            case 2:
                iItemIndex = 25;
                break;
            case 3:
                iItemIndex = 38;
                break;
            case 4:
                iItemIndex = 51;
                break;
            default:
                break;
            }
            (*Tex_iter)->SetTextureIndex(iItemIndex);
            pInven->Add_Item(CItem::Create_Item(iItemIndex));
        }
        else if ((*iter) < 20 && (*iter) >= 0)
        {
            (*Tex_iter)->SetRadianceColor(COLOR_COMMON);
            iIndex = m_pGameInstance->Get_Int_Random(0, 4);
            switch (iIndex)
            {
            case 0:
                iItemIndex = 0;
                break;
            case 1:
                iItemIndex = 11;
                break;
            case 2:
                iItemIndex = 24;
                break;
            case 3:
                iItemIndex = 37;
                break;
            case 4:
                iItemIndex = 50;
                break;
            default:
                break;
            }
            (*Tex_iter)->SetTextureIndex(iItemIndex);
            pInven->Add_Item(CItem::Create_Item(iItemIndex));
        }
        else 
        {
            (*Tex_iter)->SetRadianceColor(COLOR_COMMON);
            (*Tex_iter)->SetTextureIndex(0);
            pInven->Add_Item(CItem::Create_Item(0));

        }
    
        //static_cast<CItem_Weapon*>
        (*Tex_iter)->Update_Matrix(static_cast<_float>(368.f + (150.f * i) - g_iWinSizeX / 2), 15.f);

        Tex_iter++;
        iter++;

    }

}

CGacha_Result_Page* CGacha_Result_Page::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CGacha_Result_Page* pInstance = new CGacha_Result_Page(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CGacha_Result_Page"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CGacha_Result_Page::Clone(void* pArg)
{
    CGacha_Result_Page* pInstance = new CGacha_Result_Page(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CGacha_Result_Page"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CGacha_Result_Page::Free()
{
    __super::Free();


    for (auto iter : m_pTex_List)
    {
        Safe_Release(iter);
    }
    m_pTex_List.clear();

}
