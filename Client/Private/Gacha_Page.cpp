#include "stdafx.h"
#include "Gacha_Page.h"
#include "UI_Manager.h"
#include "Inventory.h"

#include "GameInstance.h"

CGacha_Page::CGacha_Page(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : COrtho(pDevice, pContext)
{
}

CGacha_Page::CGacha_Page(const CGacha_Page& rhs)
    : COrtho(rhs)
{
}

HRESULT CGacha_Page::Add_Component(void* pArg)
{
    if (FAILED(__super::Add_Component(pArg)))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Gacha_Bg"),
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    /* For.Com_Texture_Banner */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Gacha_Banner"),
        TEXT("Com_Texture_Banner"), (CComponent**)&m_pBannerTexture)))
        return E_FAIL;

    return S_OK;
}

HRESULT CGacha_Page::Bind_ShaderResources()
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

HRESULT CGacha_Page::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CGacha_Page::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component(pArg)))
        return E_FAIL;

    COrtho::Initialize_Matrix();

    COrtho::ORTHO_DESC tOrthoDesc = { };
    
    _float4x4 IconMatrx = { };
    XMStoreFloat4x4(&IconMatrx, XMMatrixIdentity());

    IconMatrx._11 = g_iWinSizeX; IconMatrx._22 = g_iWinSizeY;
    IconMatrx._41 = static_cast<_float>(0.f);
    IconMatrx._42 = static_cast<_float>(-(0.f));
    m_GachaMat.push_back(IconMatrx);

    IconMatrx._11 = 180.f; IconMatrx._22 = 80.f;
    IconMatrx._41 = static_cast<_float>(650.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(80.f - g_iWinSizeY / 2));
    m_GachaMat.push_back(IconMatrx);

    IconMatrx._11 = 180.f; IconMatrx._22 = 80.f;
    IconMatrx._41 = static_cast<_float>(880.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(80.f - g_iWinSizeY / 2));
    m_GachaMat.push_back(IconMatrx);

    IconMatrx._11 = 1080.f; IconMatrx._22 = 500.f;
    IconMatrx._41 = static_cast<_float>(800.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(450.f - g_iWinSizeY / 2));
    m_BannerMat.push_back(IconMatrx);

    //RESULT PAGE

    //tOrthoDesc.eTextureLevel = LEVEL_WORLD;
    //tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_Gacha_Result_Bg");
    //tOrthoDesc.fPosition = { 0.f,0.f };
    //tOrthoDesc.fSize = _float2(g_iWinSizeX, g_iWinSizeY);
    //tOrthoDesc.ePage = PAGE_GACHA_RESULT;
    //tOrthoDesc.iDepth = 0;
    //if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, TEXT("Prototype_GameObject_Gacha_Result_Page"), TEXT("Layer_Icon"), &tOrthoDesc)))
    //    return E_FAIL;

    //m_pResultPg = (CGacha_Result_Page*)m_pGameInstance->Find_Object(LEVEL_WORLD, TEXT("Layer_Icon"), 
    //    static_cast<_uint>(m_pGameInstance->Find_Objects(LEVEL_WORLD, TEXT("Layer_Icon"))->size()) - 1);

    m_pSelect_Box = (CSelRect*)m_pGameInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Select_Box"))->Clone(nullptr);


    m_eRenderType = RENDER_TYPE::RTYPE_UI;

    return S_OK;
}

HRESULT CGacha_Page::PreviousTick(_float fTimeDelta)
{
    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {
        if (FAILED(__super::PreviousTick(fTimeDelta)))
            return E_FAIL;

        if (m_bShowSelectBox == true)
            m_pSelect_Box->PreviousTick(fTimeDelta);
    }
    return S_OK;
}

HRESULT CGacha_Page::Tick(_float fTimeDelta)
{

    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {

        CInventory* pInven = (CInventory*)m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Inventory"));
        CUI_Manager* UI_Manager = (CUI_Manager*)m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_UIManager"));
        m_iNumGems = pInven->Get_NumGems();

        _int        iSelect = -1;
        POINT       ptMouse, ptMouseView;
        RECT        rcDimentions, rcbtn, rcbtn2;

        GetCursorPos(&ptMouse);

        ScreenToClient(g_hWnd, &ptMouse);

        ptMouseView.x = ptMouse.x - g_iWinSizeX / 2;
        ptMouseView.y = (-ptMouse.y + g_iWinSizeY / 2);

        rcbtn.left = static_cast<LONG>(1515.f);
        rcbtn.top = static_cast<LONG>(45.f);
        rcbtn.right = static_cast<LONG>(1555.f);
        rcbtn.bottom = static_cast<LONG>(85.f);

        rcbtn2.left = static_cast<LONG>(1270.f);
        rcbtn2.top = static_cast<LONG>(770.f);
        rcbtn2.right = static_cast<LONG>(1550.f);
        rcbtn2.bottom = static_cast<LONG>(845.f);

        _bool bOvered = { FALSE };
        for (_uint iIndex = 0; iIndex < m_pBannerTexture->Get_NumTexture(); ++iIndex)
        {
            rcDimentions.left = static_cast<LONG>(m_GachaMat[iIndex+1]._41 - m_GachaMat[iIndex+1]._11 / 2.f);
            rcDimentions.top = static_cast<LONG>(m_GachaMat[iIndex+1]._42 - m_GachaMat[iIndex+1]._22 / 2.f);
            rcDimentions.right = static_cast<LONG>(m_GachaMat[iIndex+1]._41 + m_GachaMat[iIndex+1]._11 / 2.f);
            rcDimentions.bottom = static_cast<LONG>(m_GachaMat[iIndex+1]._42 + m_GachaMat[iIndex+1]._22 / 2.f);

            if (PtInRect(&rcDimentions, ptMouseView))
            {
                bOvered = TRUE;
                m_iMOverIndex = iIndex;
                break;
            }
        }

        if (bOvered == FALSE)
            m_iMOverIndex = -1;


        if (m_bShowSelectBox == true)
        {
            m_pSelect_Box->Tick(fTimeDelta);
            iSelect = m_pSelect_Box->Get_SelectedIdx();

            if (iSelect == 1)
            {
                m_bShowSelectBox = false;
                m_pSelect_Box->Reset_Box();
            }
            else if (m_iNumGems >= 1600 && m_pSelect_Box->Get_SelectedIdx() == 0)
            {
            //원석 1600개를 사용하여
            m_pResultPg = (CGacha_Result_Page*)UI_Manager->Get_UI_Page(PAGE_GACHA_RESULT);
            _uint iRandom;
            iRandom = m_pGameInstance->Get_Int_Random(0, 100);
            UI_Manager->Show_Page(PAGE_GACHA_RESULT);
            m_pGameInstance->StopAll();
            m_pResultPg->ShowResults(iRandom);
            m_pSelect_Box->Reset_Box();
            m_bShowSelectBox = false;

            }
            else
            {
            //원석이 부족합니다.
            }

        }

        

        if (m_pGameInstance->Key_Down(VK_LBUTTON))
        {
            m_pGameInstance->PlaySoundW(L"Click.mp3", 0.5f, Engine::SOUND_CH0);
            if (PtInRect(&rcbtn, ptMouse))
            {
                //닫기

                UI_Manager->Show_Page(PAGE_MAIN);
                m_pGameInstance->StopSound(SOUND_BGM);
                m_pGameInstance->PlayBGM(L"MondstadtBGM.mp3", 0.6f);

            }


            if (PtInRect(&rcbtn2, ptMouse))
            {

                m_bShowSelectBox = true;
                m_pSelect_Box->Set_BoxText(TEXT(" 원석 1600개를 사용하여 10연속 뽑기를 \n 진행하시겠습니까?"));
                m_pSelect_Box->Set_SelRectColor(0);
                m_pSelect_Box->Set_SelRectType(CSelRect::SELRECT_PLAIN);


            }

            if (m_iMOverIndex != -1 && m_iMOverIndex != m_iSelectedIndex)
            {
                m_iSelectedIndex = m_iMOverIndex;
                if (m_iSelectedIndex == 0)
                {

                }
                if (m_iSelectedIndex == 1)
                {

                }
                m_BannerMat[0]._41 = 100.f;
                m_BannerMat[0]._42 = 0.f;
            }
            
        }




        if (m_BannerMat[0]._41 >= 0.f)
        {
            m_BannerMat[0]._41 -= 5.f;
        }

        if (FAILED(__super::Tick(fTimeDelta)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CGacha_Page::LateTick(_float fTimeDelta)
{
    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {
        if (FAILED(__super::LateTick(fTimeDelta)))
            return E_FAIL;

        if (m_bShowSelectBox == true)
            m_pSelect_Box->LateTick(fTimeDelta);
    }

    return S_OK;
}

HRESULT CGacha_Page::Render()
{

        if (FAILED(Bind_ShaderResources()))
            return E_FAIL;


        _float4x4 CombinedMatrix = {};

        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
            return E_FAIL;

        CombinedMatrix = m_GachaMat[0];

        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;

        for (_uint iIndex = 0; iIndex < m_pBannerTexture->Get_NumTexture(); ++iIndex)
            {
               if (FAILED(m_pBannerTexture->Bind_ShaderResource(m_pShaderCom, "g_Texture", iIndex)))
                   return E_FAIL;

        		CombinedMatrix = m_GachaMat[iIndex+1];
               if (m_iMOverIndex == iIndex || m_iSelectedIndex == iIndex)
               {
                   CombinedMatrix._11 *= 1.1f;
                   CombinedMatrix._22 *= 1.1f;
               }

               if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
                   return E_FAIL;

               if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
                   return E_FAIL;

               if (FAILED(m_pVIBufferCom->Render()))
                   return E_FAIL; 
         }

        if (FAILED(m_pBannerTexture->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iSelectedIndex)))
            return E_FAIL;

        CombinedMatrix = m_BannerMat[0];

        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;

        //Render Gems
        _tchar szDamage[MAX_PATH];
        wsprintf(szDamage, TEXT("%d"), m_iNumGems);
        m_pGameInstance->Render_Font(L"Font_Number", szDamage, { 1178.f,50.f }, 0.5f, Colors::White, 0.f, { 0.f,0.f }, false);

    return S_OK;
}
    
CGacha_Page* CGacha_Page::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CGacha_Page* pInstance = new CGacha_Page(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CGacha_Page"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CGacha_Page::Clone(void* pArg)
{
    CGacha_Page* pInstance = new CGacha_Page(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CGacha_Page"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CGacha_Page::Free()
{
    __super::Free();

    if (m_bCloned)
    {
        Safe_Release(m_pBannerTexture);
        Safe_Release(m_pSelect_Box);
    }

}
