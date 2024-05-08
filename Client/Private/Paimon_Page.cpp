#include "stdafx.h"
#include "Paimon_Page.h"

#include "GameInstance.h"
#include "UI_Manager.h"
#include "Inventory.h"

CPaimon_Page::CPaimon_Page(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : COrtho(pDevice, pContext)
{
}

CPaimon_Page::CPaimon_Page(const CPaimon_Page& rhs)
    : COrtho(rhs)
{
}

HRESULT CPaimon_Page::Add_Component(void* pArg)
{
    if (FAILED(__super::Add_Component(pArg)))
        return E_FAIL;

    /* For.Com_Texture */

    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Paimon_Page"),
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Paimon_Icon"),
        TEXT("Com_Texture_Icon"), (CComponent**)&m_pIconTextureCom)))
        return E_FAIL;

    /* For.Com_Texture_Main_Icon */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Main_Icons"),
        TEXT("Com_Texture_Main_Icon"), (CComponent**)&m_pMainIconTextureCom)))
        return E_FAIL;

    /* For.Com_Texture_Portraits */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Paimon_Portraits"),
        TEXT("Com_Texture_Portraits"), (CComponent**)&m_pPortraitTextureCom)))
        return E_FAIL;

    /* For.Com_Texture_Card */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Paimon_Name_Card"),
        TEXT("Com_Texture_Card"), (CComponent**)&m_pNameCardTextureCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CPaimon_Page::Bind_ShaderResources()
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

HRESULT CPaimon_Page::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CPaimon_Page::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component(pArg)))
        return E_FAIL;

    COrtho::Initialize_Matrix();

    if (m_pGameInstance->Key_Down(VK_LBUTTON))
    {
        m_pGameInstance->PlaySoundW(L"InteractionBox.mp3", 0.7f);
    }

    _float4x4 IconMatrx = { };
    XMStoreFloat4x4(&IconMatrx, XMMatrixIdentity());

    // //Pages
// COrtho::ORTHO_DESC tOrthoDesc = { };
// tOrthoDesc.eTextureLevel = LEVEL_WORLD;
// tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_Paimon_Base");
// tOrthoDesc.fPosition = { static_cast<_float>(350.f - g_iWinSizeX / 2), static_cast<_float>(-(450.f - g_iWinSizeY / 2)) };
// tOrthoDesc.fSize = { 560.f,900.f };
// tOrthoDesc.ePage = PAGE_PAIMON;
// tOrthoDesc.iDepth = 0;
// if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, TEXT("Prototype_GameObject_Paimon_Page"), TEXT("Layer_Icon"), &tOrthoDesc)))
// 	return E_FAIL;
// 

    IconMatrx._11 = 560.f; IconMatrx._22 = 900.f;
    IconMatrx._41 = static_cast<_float>(350.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(450.f - g_iWinSizeY / 2));
    m_PaimonBaseMat.push_back(IconMatrx);

    //tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_Blue_Strip");
//tOrthoDesc.fPosition = { static_cast<_float>(38.f - g_iWinSizeX / 2), static_cast<_float>(-(450.f - g_iWinSizeY / 2)) };
//tOrthoDesc.fSize = { 85.f,900.f };
//tOrthoDesc.ePage = PAGE_PAIMON;
//if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, TEXT("Prototype_GameObject_AlphaRect"), TEXT("Layer_Icon"), &tOrthoDesc)))
//    return E_FAIL;

    IconMatrx._11 = 85.f; IconMatrx._22 = 900.f;
    IconMatrx._41 = static_cast<_float>(38.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(450.f - g_iWinSizeY / 2));
    m_PaimonBaseMat.push_back(IconMatrx);

    //tOrthoDesc.eTextureLevel = LEVEL_WORLD;
    //tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_Paimon_Button");
    //tOrthoDesc.fPosition = { static_cast<_float>(156.f - g_iWinSizeX / 2), static_cast<_float>(-(325.f - g_iWinSizeY / 2)) };
    //tOrthoDesc.fSize = { 120.f, 98.f };
    //tOrthoDesc.ePage = PAGE_PAIMON;
    //if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, TEXT("Prototype_GameObject_Button"), TEXT("Layer_Icon"), &tOrthoDesc)))
    //    return E_FAIL;

    IconMatrx._11 = 120.f; IconMatrx._22 = 98.f;
    IconMatrx._41 = static_cast<_float>(156.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(325.f - g_iWinSizeY / 2));
    m_PaimonBaseMat.push_back(IconMatrx);

    //tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_Paimon_Button");
    //tOrthoDesc.fPosition = { static_cast<_float>(286.f - g_iWinSizeX / 2), static_cast<_float>(-(325.f - g_iWinSizeY / 2)) };
    //tOrthoDesc.fSize = { 120.f, 98.f };
    //tOrthoDesc.ePage = PAGE_PAIMON;
    //if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, TEXT("Prototype_GameObject_Button"), TEXT("Layer_Icon"), &tOrthoDesc)))
    //    return E_FAIL;
    // 

    IconMatrx._11 = 120.f; IconMatrx._22 = 98.f;
    IconMatrx._41 = static_cast<_float>(286.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(325.f - g_iWinSizeY / 2));
    m_PaimonBaseMat.push_back(IconMatrx);

    //tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_Paimon_Button");
    //tOrthoDesc.fPosition = { static_cast<_float>(416.f - g_iWinSizeX / 2), static_cast<_float>(-(325.f - g_iWinSizeY / 2)) };
    //tOrthoDesc.fSize = { 120.f, 98.f };
    //tOrthoDesc.ePage = PAGE_PAIMON;
    //if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, TEXT("Prototype_GameObject_Button"), TEXT("Layer_Icon"), &tOrthoDesc)))
    //    return E_FAIL;
    // 
    IconMatrx._11 = 120.f; IconMatrx._22 = 98.f;
    IconMatrx._41 = static_cast<_float>(416.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(325.f - g_iWinSizeY / 2));
    m_PaimonBaseMat.push_back(IconMatrx);
    //tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_Paimon_Button");
    //tOrthoDesc.fPosition = { static_cast<_float>(546.f - g_iWinSizeX / 2), static_cast<_float>(-(325.f - g_iWinSizeY / 2)) };
    //tOrthoDesc.fSize = { 120.f, 98.f };
    //tOrthoDesc.ePage = PAGE_PAIMON;
    //if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, TEXT("Prototype_GameObject_Button"), TEXT("Layer_Icon"), &tOrthoDesc)))
    //    return E_FAIL;
    IconMatrx._11 = 120.f; IconMatrx._22 = 98.f;
    IconMatrx._41 = static_cast<_float>(546.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(325.f - g_iWinSizeY / 2));
    m_PaimonBaseMat.push_back(IconMatrx);
    ////R2

    //tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_Paimon_Button");
    //tOrthoDesc.fPosition = { static_cast<_float>(156.f - g_iWinSizeX / 2), static_cast<_float>(-(435.f - g_iWinSizeY / 2)) };
    //tOrthoDesc.fSize = { 120.f, 98.f };
    //tOrthoDesc.ePage = PAGE_PAIMON;
    //if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, TEXT("Prototype_GameObject_Button"), TEXT("Layer_Icon"), &tOrthoDesc)))
    //    return E_FAIL;
    
    IconMatrx._11 = 120.f; IconMatrx._22 = 98.f;
    IconMatrx._41 = static_cast<_float>(156.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(435.f - g_iWinSizeY / 2));
    m_PaimonBaseMat.push_back(IconMatrx);
    IconMatrx._11 = 120.f; IconMatrx._22 = 98.f;
    IconMatrx._41 = static_cast<_float>(286.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(435.f - g_iWinSizeY / 2));
    m_PaimonBaseMat.push_back(IconMatrx);
    IconMatrx._11 = 120.f; IconMatrx._22 = 98.f;
    IconMatrx._41 = static_cast<_float>(416.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(435.f - g_iWinSizeY / 2));
    m_PaimonBaseMat.push_back(IconMatrx);
        IconMatrx._11 = 120.f; IconMatrx._22 = 98.f;
    IconMatrx._41 = static_cast<_float>(546.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(435.f - g_iWinSizeY / 2));
    m_PaimonBaseMat.push_back(IconMatrx);

    //tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_Paimon_Button");
    //tOrthoDesc.fPosition = { static_cast<_float>(286.f - g_iWinSizeX / 2), static_cast<_float>(-(435.f - g_iWinSizeY / 2)) };
    //tOrthoDesc.fSize = { 120.f, 98.f };
    //tOrthoDesc.ePage = PAGE_PAIMON;
    //if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, TEXT("Prototype_GameObject_Button"), TEXT("Layer_Icon"), &tOrthoDesc)))
    //    return E_FAIL;
    //tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_Paimon_Button");
    //tOrthoDesc.fPosition = { static_cast<_float>(416.f - g_iWinSizeX / 2), static_cast<_float>(-(435.f - g_iWinSizeY / 2)) };
    //tOrthoDesc.fSize = { 120.f, 98.f };
    //tOrthoDesc.ePage = PAGE_PAIMON;
    //if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, TEXT("Prototype_GameObject_Button"), TEXT("Layer_Icon"), &tOrthoDesc)))
    //    return E_FAIL;
    //tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_Paimon_Button");
    //tOrthoDesc.fPosition = { static_cast<_float>(546.f - g_iWinSizeX / 2), static_cast<_float>(-(435.f - g_iWinSizeY / 2)) };
    //tOrthoDesc.fSize = { 120.f, 98.f };
    //tOrthoDesc.ePage = PAGE_PAIMON;
    //if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, TEXT("Prototype_GameObject_Button"), TEXT("Layer_Icon"), &tOrthoDesc)))
    //    return E_FAIL;

    ////R3


    IconMatrx._11 = 120.f; IconMatrx._22 = 98.f;
    IconMatrx._41 = static_cast<_float>(156.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(545.f - g_iWinSizeY / 2));
    m_PaimonBaseMat.push_back(IconMatrx);
    IconMatrx._11 = 120.f; IconMatrx._22 = 98.f;
    IconMatrx._41 = static_cast<_float>(286.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(545.f - g_iWinSizeY / 2));
    m_PaimonBaseMat.push_back(IconMatrx);
    IconMatrx._11 = 120.f; IconMatrx._22 = 98.f;
    IconMatrx._41 = static_cast<_float>(416.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(545.f - g_iWinSizeY / 2));
    m_PaimonBaseMat.push_back(IconMatrx);
    IconMatrx._11 = 120.f; IconMatrx._22 = 98.f;
    IconMatrx._41 = static_cast<_float>(546.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(545.f - g_iWinSizeY / 2));
    m_PaimonBaseMat.push_back(IconMatrx);
    //tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_Paimon_Button");
    //tOrthoDesc.fPosition = { static_cast<_float>(156.f - g_iWinSizeX / 2), static_cast<_float>(-(545.f - g_iWinSizeY / 2)) };
    //tOrthoDesc.fSize = { 120.f, 98.f };
    //tOrthoDesc.ePage = PAGE_PAIMON;
    //if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, TEXT("Prototype_GameObject_Button"), TEXT("Layer_Icon"), &tOrthoDesc)))
    //    return E_FAIL;
    //tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_Paimon_Button");
    //tOrthoDesc.fPosition = { static_cast<_float>(286.f - g_iWinSizeX / 2), static_cast<_float>(-(545.f - g_iWinSizeY / 2)) };
    //tOrthoDesc.fSize = { 120.f, 98.f };
    //tOrthoDesc.ePage = PAGE_PAIMON;
    //if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, TEXT("Prototype_GameObject_Button"), TEXT("Layer_Icon"), &tOrthoDesc)))
    //    return E_FAIL;
    //tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_Paimon_Button");
    //tOrthoDesc.fPosition = { static_cast<_float>(416.f - g_iWinSizeX / 2), static_cast<_float>(-(545.f - g_iWinSizeY / 2)) };
    //tOrthoDesc.fSize = { 120.f, 98.f };
    //tOrthoDesc.ePage = PAGE_PAIMON;
    //if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, TEXT("Prototype_GameObject_Button"), TEXT("Layer_Icon"), &tOrthoDesc)))
    //    return E_FAIL;
    //tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_Paimon_Button");
    //tOrthoDesc.fPosition = { static_cast<_float>(546.f - g_iWinSizeX / 2), static_cast<_float>(-(545.f - g_iWinSizeY / 2)) };
    //tOrthoDesc.fSize = { 120.f, 98.f };
    //tOrthoDesc.ePage = PAGE_PAIMON;
    //if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, TEXT("Prototype_GameObject_Button"), TEXT("Layer_Icon"), &tOrthoDesc)))
    //    return E_FAIL;

    ////R4

    IconMatrx._11 = 120.f; IconMatrx._22 = 98.f;
    IconMatrx._41 = static_cast<_float>(156.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(655.f - g_iWinSizeY / 2));
    m_PaimonBaseMat.push_back(IconMatrx);
    IconMatrx._11 = 120.f; IconMatrx._22 = 98.f;
    IconMatrx._41 = static_cast<_float>(286.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(655.f - g_iWinSizeY / 2));
    m_PaimonBaseMat.push_back(IconMatrx);
    IconMatrx._11 = 120.f; IconMatrx._22 = 98.f;
    IconMatrx._41 = static_cast<_float>(416.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(655.f - g_iWinSizeY / 2));
    m_PaimonBaseMat.push_back(IconMatrx);
    IconMatrx._11 = 120.f; IconMatrx._22 = 98.f;
    IconMatrx._41 = static_cast<_float>(546.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(655.f - g_iWinSizeY / 2));
    m_PaimonBaseMat.push_back(IconMatrx);
    //tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_Paimon_Button");
    //tOrthoDesc.fPosition = { static_cast<_float>(156.f - g_iWinSizeX / 2), static_cast<_float>(-(655.f - g_iWinSizeY / 2)) };
    //tOrthoDesc.fSize = { 120.f, 98.f };
    //tOrthoDesc.ePage = PAGE_PAIMON;
    //if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, TEXT("Prototype_GameObject_Button"), TEXT("Layer_Icon"), &tOrthoDesc)))
    //    return E_FAIL;
    //tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_Paimon_Button");
    //tOrthoDesc.fPosition = { static_cast<_float>(286.f - g_iWinSizeX / 2), static_cast<_float>(-(655.f - g_iWinSizeY / 2)) };
    //tOrthoDesc.fSize = { 120.f, 98.f };
    //tOrthoDesc.ePage = PAGE_PAIMON;
    //if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, TEXT("Prototype_GameObject_Button"), TEXT("Layer_Icon"), &tOrthoDesc)))
    //    return E_FAIL;
    //tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_Paimon_Button");
    //tOrthoDesc.fPosition = { static_cast<_float>(416.f - g_iWinSizeX / 2), static_cast<_float>(-(655.f - g_iWinSizeY / 2)) };
    //tOrthoDesc.fSize = { 120.f, 98.f };
    //tOrthoDesc.ePage = PAGE_PAIMON;
    //if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, TEXT("Prototype_GameObject_Button"), TEXT("Layer_Icon"), &tOrthoDesc)))
    //    return E_FAIL;
    //tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_Paimon_Button");
    //tOrthoDesc.fPosition = { static_cast<_float>(546.f - g_iWinSizeX / 2), static_cast<_float>(-(655.f - g_iWinSizeY / 2)) };
    //tOrthoDesc.fSize = { 120.f, 98.f };
    //tOrthoDesc.ePage = PAGE_PAIMON;
    //if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, TEXT("Prototype_GameObject_Button"), TEXT("Layer_Icon"), &tOrthoDesc)))
    //    return E_FAIL;

    ////R5

    IconMatrx._11 = 120.f; IconMatrx._22 = 98.f;
    IconMatrx._41 = static_cast<_float>(156.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(765.f - g_iWinSizeY / 2));
    m_PaimonBaseMat.push_back(IconMatrx);
    IconMatrx._11 = 120.f; IconMatrx._22 = 98.f;
    IconMatrx._41 = static_cast<_float>(286.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(765.f - g_iWinSizeY / 2));
    m_PaimonBaseMat.push_back(IconMatrx);
    IconMatrx._11 = 120.f; IconMatrx._22 = 98.f;
    IconMatrx._41 = static_cast<_float>(416.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(765.f - g_iWinSizeY / 2));
    m_PaimonBaseMat.push_back(IconMatrx);
    IconMatrx._11 = 120.f; IconMatrx._22 = 98.f;
    IconMatrx._41 = static_cast<_float>(546.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(765.f - g_iWinSizeY / 2));
    m_PaimonBaseMat.push_back(IconMatrx);

    //tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_Paimon_Button");
    //tOrthoDesc.fPosition = { static_cast<_float>(156.f - g_iWinSizeX / 2), static_cast<_float>(-(765.f - g_iWinSizeY / 2)) };
    //tOrthoDesc.fSize = { 120.f, 98.f };
    //tOrthoDesc.ePage = PAGE_PAIMON;
    //if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, TEXT("Prototype_GameObject_Button"), TEXT("Layer_Icon"), &tOrthoDesc)))
    //    return E_FAIL;
    //tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_Paimon_Button");
    //tOrthoDesc.fPosition = { static_cast<_float>(286.f - g_iWinSizeX / 2), static_cast<_float>(-(765.f - g_iWinSizeY / 2)) };
    //tOrthoDesc.fSize = { 120.f, 98.f };
    //tOrthoDesc.ePage = PAGE_PAIMON;
    //if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, TEXT("Prototype_GameObject_Button"), TEXT("Layer_Icon"), &tOrthoDesc)))
    //    return E_FAIL;
    //tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_Paimon_Button");
    //tOrthoDesc.fPosition = { static_cast<_float>(416.f - g_iWinSizeX / 2), static_cast<_float>(-(765.f - g_iWinSizeY / 2)) };
    //tOrthoDesc.fSize = { 120.f, 98.f };
    //tOrthoDesc.ePage = PAGE_PAIMON;
    //if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, TEXT("Prototype_GameObject_Button"), TEXT("Layer_Icon"), &tOrthoDesc)))
    //    return E_FAIL;
    //tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_Paimon_Button");
    //tOrthoDesc.fPosition = { static_cast<_float>(546.f - g_iWinSizeX / 2), static_cast<_float>(-(765.f - g_iWinSizeY / 2)) };
    //tOrthoDesc.fSize = { 120.f, 98.f };
    //tOrthoDesc.ePage = PAGE_PAIMON;
    //if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, TEXT("Prototype_GameObject_Button"), TEXT("Layer_Icon"), &tOrthoDesc)))
    //    return E_FAIL;

    IconMatrx._11 = 547.f; IconMatrx._22 = 254.f;
    IconMatrx._41 = static_cast<_float>(352.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(126.f - g_iWinSizeY / 2));
    m_NameCardMat.push_back(IconMatrx);


    //tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_Paimon_Name_Card");
    //tOrthoDesc.fPosition = { static_cast<_float>(352.f - g_iWinSizeX / 2), static_cast<_float>(-(126.f - g_iWinSizeY / 2)) };
    //tOrthoDesc.fSize = { 547.f, 254.f };
    //tOrthoDesc.ePage = PAGE_PAIMON;
    //if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, TEXT("Prototype_GameObject_Decor"), TEXT("Layer_Icon"), &tOrthoDesc)))
    //    return E_FAIL;

    IconMatrx._11 = 470.f; IconMatrx._22 = 228.f;
    IconMatrx._41 = static_cast<_float>(323.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(139.f - g_iWinSizeY / 2));
    m_NameCardMat.push_back(IconMatrx);

    //tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_Text_layer");
    //tOrthoDesc.fPosition = { static_cast<_float>(323.f - g_iWinSizeX / 2), static_cast<_float>(-(139.f - g_iWinSizeY / 2)) };
    //tOrthoDesc.fSize = { 470.f, 228.f };
    //tOrthoDesc.ePage = PAGE_PAIMON;
    //if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, TEXT("Prototype_GameObject_Decor"), TEXT("Layer_Icon"), &tOrthoDesc)))
    //    return E_FAIL;


    IconMatrx._11 = 78.f; IconMatrx._22 = 78.f;
    IconMatrx._41 = static_cast<_float>(166.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(94.f - g_iWinSizeY / 2));
    m_NameCardMat.push_back(IconMatrx);

    //tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_Paimon_Portraits");
    //tOrthoDesc.fPosition = { static_cast<_float>(166.f - g_iWinSizeX / 2), static_cast<_float>(-(94.f - g_iWinSizeY / 2)) };
    //tOrthoDesc.fSize = { 78.f, 78.f };
    //tOrthoDesc.ePage = PAGE_PAIMON;
    //if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, TEXT("Prototype_GameObject_Decor"), TEXT("Layer_Icon"), &tOrthoDesc)))
    //    return E_FAIL;

    ////Icons


    IconMatrx._11 = 50.f; IconMatrx._22 = 50.f;
    IconMatrx._41 = static_cast<_float>(156.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(315.f - g_iWinSizeY / 2));
    m_PaimonIconsMat.push_back(IconMatrx);
    IconMatrx._11 = 50.f; IconMatrx._22 = 50.f;
    IconMatrx._41 = static_cast<_float>(286.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(315.f - g_iWinSizeY / 2));
    m_PaimonIconsMat.push_back(IconMatrx);
    IconMatrx._11 = 50.f; IconMatrx._22 = 50.f;
    IconMatrx._41 = static_cast<_float>(416.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(315.f - g_iWinSizeY / 2));
    m_PaimonIconsMat.push_back(IconMatrx);
    IconMatrx._11 = 50.f; IconMatrx._22 = 50.f;
    IconMatrx._41 = static_cast<_float>(546.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(315.f - g_iWinSizeY / 2));
    m_PaimonIconsMat.push_back(IconMatrx);

    //tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_Icon_Shop");
    //tOrthoDesc.fPosition = { static_cast<_float>(156.f - g_iWinSizeX / 2), static_cast<_float>(-(315.f - g_iWinSizeY / 2)) };
    //tOrthoDesc.fSize = { 50.f, 50.f };
    //tOrthoDesc.ePage = PAGE_PAIMON;
    //if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, TEXT("Prototype_GameObject_Decor"), TEXT("Layer_Icon"), &tOrthoDesc)))
    //    return E_FAIL;
    //tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_Icon_Party_Setup");
    //tOrthoDesc.fPosition = { static_cast<_float>(286.f - g_iWinSizeX / 2), static_cast<_float>(-(315.f - g_iWinSizeY / 2)) };
    //tOrthoDesc.fSize = { 50.f, 50.f };
    //tOrthoDesc.ePage = PAGE_PAIMON;
    //if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, TEXT("Prototype_GameObject_Decor"), TEXT("Layer_Icon"), &tOrthoDesc)))
    //    return E_FAIL;
    //tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_Icon_Friends");
    //tOrthoDesc.fPosition = { static_cast<_float>(416.f - g_iWinSizeX / 2), static_cast<_float>(-(315.f - g_iWinSizeY / 2)) };
    //tOrthoDesc.fSize = { 50.f, 50.f };
    //tOrthoDesc.ePage = PAGE_PAIMON;
    //if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, TEXT("Prototype_GameObject_Decor"), TEXT("Layer_Icon"), &tOrthoDesc)))
    //    return E_FAIL;
    //tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_Icon_Achievements");
    //tOrthoDesc.fPosition = { static_cast<_float>(546.f - g_iWinSizeX / 2), static_cast<_float>(-(315.f - g_iWinSizeY / 2)) };
    //tOrthoDesc.fSize = { 50.f, 50.f };
    //tOrthoDesc.ePage = PAGE_PAIMON;
    //if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, TEXT("Prototype_GameObject_Decor"), TEXT("Layer_Icon"), &tOrthoDesc)))
    //    return E_FAIL;

    ////R2
    // 
    IconMatrx._11 = 50.f; IconMatrx._22 = 50.f;
    IconMatrx._41 = static_cast<_float>(156.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(425.f - g_iWinSizeY / 2));
    m_PaimonIconsMat.push_back(IconMatrx);
    IconMatrx._11 = 50.f; IconMatrx._22 = 50.f;
    IconMatrx._41 = static_cast<_float>(286.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(425.f - g_iWinSizeY / 2));
    m_PaimonIconsMat.push_back(IconMatrx);
    IconMatrx._11 = 50.f; IconMatrx._22 = 50.f;
    IconMatrx._41 = static_cast<_float>(416.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(425.f - g_iWinSizeY / 2));
    m_PaimonIconsMat.push_back(IconMatrx);
    IconMatrx._11 = 50.f; IconMatrx._22 = 50.f;
    IconMatrx._41 = static_cast<_float>(546.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(425.f - g_iWinSizeY / 2));
    m_PaimonIconsMat.push_back(IconMatrx);

    IconMatrx._11 = 50.f; IconMatrx._22 = 50.f;
    IconMatrx._41 = static_cast<_float>(156.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(535.f - g_iWinSizeY / 2));
    m_PaimonIconsMat.push_back(IconMatrx);
    IconMatrx._11 = 50.f; IconMatrx._22 = 50.f;
    IconMatrx._41 = static_cast<_float>(286.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(535.f - g_iWinSizeY / 2));
    m_PaimonIconsMat.push_back(IconMatrx);
    IconMatrx._11 = 50.f; IconMatrx._22 = 50.f;
    IconMatrx._41 = static_cast<_float>(416.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(535.f - g_iWinSizeY / 2));
    m_PaimonIconsMat.push_back(IconMatrx);
    IconMatrx._11 = 50.f; IconMatrx._22 = 50.f;
    IconMatrx._41 = static_cast<_float>(546.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(535.f - g_iWinSizeY / 2));
    m_PaimonIconsMat.push_back(IconMatrx);

    IconMatrx._11 = 50.f; IconMatrx._22 = 50.f;
    IconMatrx._41 = static_cast<_float>(156.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(645.f - g_iWinSizeY / 2));
    m_PaimonIconsMat.push_back(IconMatrx);
    IconMatrx._11 = 50.f; IconMatrx._22 = 50.f;
    IconMatrx._41 = static_cast<_float>(286.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(645.f - g_iWinSizeY / 2));
    m_PaimonIconsMat.push_back(IconMatrx);
    IconMatrx._11 = 50.f; IconMatrx._22 = 50.f;
    IconMatrx._41 = static_cast<_float>(416.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(645.f - g_iWinSizeY / 2));
    m_PaimonIconsMat.push_back(IconMatrx);
    IconMatrx._11 = 50.f; IconMatrx._22 = 50.f;
    IconMatrx._41 = static_cast<_float>(546.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(645.f - g_iWinSizeY / 2));
    m_PaimonIconsMat.push_back(IconMatrx);

    IconMatrx._11 = 50.f; IconMatrx._22 = 50.f;
    IconMatrx._41 = static_cast<_float>(156.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(755.f - g_iWinSizeY / 2));
    m_PaimonIconsMat.push_back(IconMatrx);
    IconMatrx._11 = 50.f; IconMatrx._22 = 50.f;
    IconMatrx._41 = static_cast<_float>(286.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(755.f - g_iWinSizeY / 2));
    m_PaimonIconsMat.push_back(IconMatrx);
    IconMatrx._11 = 50.f; IconMatrx._22 = 50.f;
    IconMatrx._41 = static_cast<_float>(416.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(755.f - g_iWinSizeY / 2));
    m_PaimonIconsMat.push_back(IconMatrx);
    IconMatrx._11 = 50.f; IconMatrx._22 = 50.f;
    IconMatrx._41 = static_cast<_float>(546.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(755.f - g_iWinSizeY / 2));
    m_PaimonIconsMat.push_back(IconMatrx);

    IconMatrx._11 = 45.f; IconMatrx._22 = 38.f;
    IconMatrx._41 = static_cast<_float>(40.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(305.f - g_iWinSizeY / 2));
    m_PaimonIconsMat.push_back(IconMatrx);
    IconMatrx._11 = 45.f; IconMatrx._22 = 45.f;
    IconMatrx._41 = static_cast<_float>(40.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(385.f - g_iWinSizeY / 2));
    m_PaimonIconsMat.push_back(IconMatrx);
    IconMatrx._11 = 45.f; IconMatrx._22 = 40.f;
    IconMatrx._41 = static_cast<_float>(40.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(465.f - g_iWinSizeY / 2));
    m_PaimonIconsMat.push_back(IconMatrx);
    IconMatrx._11 = 45.f; IconMatrx._22 = 45.f;
    IconMatrx._41 = static_cast<_float>(40.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(545.f - g_iWinSizeY / 2));
    m_PaimonIconsMat.push_back(IconMatrx);
    IconMatrx._11 = 45.f; IconMatrx._22 = 45.f;
    IconMatrx._41 = static_cast<_float>(40.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(625.f - g_iWinSizeY / 2));
    m_PaimonIconsMat.push_back(IconMatrx);
    IconMatrx._11 = 45.f; IconMatrx._22 = 45.f;
    IconMatrx._41 = static_cast<_float>(40.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(855.f - g_iWinSizeY / 2));
    m_PaimonIconsMat.push_back(IconMatrx);


    m_eRenderType = RENDER_TYPE::RTYPE_UI;

    return S_OK;
}

HRESULT CPaimon_Page::PreviousTick(_float fTimeDelta)
{
    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {

            if (FAILED(__super::PreviousTick(fTimeDelta)))
                return E_FAIL;

    }
    return S_OK;
}

HRESULT CPaimon_Page::Tick(_float fTimeDelta)
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
            for (_int iIndex = 0; iIndex < 20; ++iIndex)
            {
                rcDimentions.left = static_cast<LONG>(m_PaimonBaseMat[iIndex+2]._41 - m_PaimonBaseMat[iIndex + 2]._11 / 2.f);
                rcDimentions.top = static_cast<LONG>(m_PaimonBaseMat[iIndex + 2]._42 - m_PaimonBaseMat[iIndex + 2]._22 / 2.f);
                rcDimentions.right = static_cast<LONG>(m_PaimonBaseMat[iIndex + 2]._41 + m_PaimonBaseMat[iIndex + 2]._11 / 2.f);
                rcDimentions.bottom = static_cast<LONG>(m_PaimonBaseMat[iIndex + 2]._42 + m_PaimonBaseMat[iIndex + 2]._22 / 2.f);

                if (PtInRect(&rcDimentions, ptMouse))
                {
                    bOvered = TRUE;
                    m_iMOverIndex = iIndex;
                    break;
                }
            }
            for (_int iIndex = 20; iIndex < 26; ++iIndex)
            {
                rcDimentions.left = static_cast<LONG>(m_PaimonIconsMat[iIndex]._41 - m_PaimonIconsMat[iIndex]._11 / 2.f);
                rcDimentions.top = static_cast<LONG>(m_PaimonIconsMat[iIndex]._42 - m_PaimonIconsMat[iIndex]._22 / 2.f);
                rcDimentions.right = static_cast<LONG>(m_PaimonIconsMat[iIndex]._41 + m_PaimonIconsMat[iIndex]._11 / 2.f);
                rcDimentions.bottom = static_cast<LONG>(m_PaimonIconsMat[iIndex]._42 + m_PaimonIconsMat[iIndex]._22 / 2.f);

                if (PtInRect(&rcDimentions, ptMouse))
                {
                    bOvered = TRUE;
                    m_iMOverIndex = iIndex;
                    break;
                }
            }

            if (bOvered == FALSE)
                m_iMOverIndex = -1;
            if (m_pGameInstance->Key_Down(VK_LBUTTON) && (m_iMOverIndex != -1))
            {
                CUI_Manager* UI_Manager = (CUI_Manager*)m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_UIManager"));
                CInventory* pInven = (CInventory*)m_pGameInstance->Find_Object(LEVEL_WORLD, TEXT("Layer_Inventory"));
                switch (m_iMOverIndex)
                {
                case 0:
                    break;
                case 1:
                    break;
                case 2:
                    break;
                case 3:
                    break;
                case 4:
                    break;
                case 5:
                    break;
                case 6:
                    break;
                case 7:
                    pInven->Open_Inventory();
                    break;
                case 12:
                    UI_Manager->Show_Page(PAGE_GACHA);
                    break;         
                case 8:
                    break;
                case 9:
                    break;
                case 25:
                    UI_Manager->Show_Page(PAGE_MAIN);
                    break;
                default:
                    break;
                }
            }

    }
    return S_OK;
}

HRESULT CPaimon_Page::LateTick(_float fTimeDelta)
{
    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {

            if (FAILED(__super::LateTick(fTimeDelta)))
                return E_FAIL;

    }
        return S_OK;
}

HRESULT CPaimon_Page::Render()
{

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _float4x4 CombinedMatrix = {};

        _uint     iPaimonIconIndex = 0;
        _uint     iMainIconIndex = 0 ;
        //Base

        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 1)))
            return E_FAIL;

        CombinedMatrix = m_PaimonBaseMat[0];

        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;

        //Blue_Strip

        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
            return E_FAIL;

        CombinedMatrix = m_PaimonBaseMat[1];


        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;

        //Buttons
        for (_int iIndex = 0; iIndex < 20; ++iIndex)
        {
            if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 2)))
                return E_FAIL;

            CombinedMatrix = m_PaimonBaseMat[iIndex + 2];
            if (m_iMOverIndex == iIndex)
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

            //Icons
            if (iIndex != 6 && iIndex != 8 && iIndex != 10 && iIndex != 11 && iIndex != 12 && iIndex != 13 && iIndex != 14)
            { 
                if (FAILED(m_pIconTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", iPaimonIconIndex)))
                    return E_FAIL;
                iPaimonIconIndex++;
            }
            else
            {
                switch (iIndex)
                {
                case 6:
                    iMainIconIndex = 9;
                    break;
                case 8:
                    iMainIconIndex = 3;
                    break;
                case 10:
                    iMainIconIndex = 4;
                    break;
                case 11:
                    iMainIconIndex = 7;
                    break;
                case 12:
                    iMainIconIndex = 6;
                    break;
                case 13:
                    iMainIconIndex = 5;
                    break;
                case 14:
                    iMainIconIndex = 2;
                    break;
                default:
                    break;
                }

                if (FAILED(m_pMainIconTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", iMainIconIndex)))
                    return E_FAIL;
            }

            CombinedMatrix = m_PaimonIconsMat[iIndex];
            if (m_iMOverIndex == iIndex)
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


            //if (m_iMOverIndex == iIndex)
            //{
            //    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 4)))
            //        return E_FAIL;

            //    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            //        return E_FAIL;

            //    if (FAILED(m_pShaderCom->Begin(VTXTEX_ALPHA)))
            //        return E_FAIL;

            //    if (FAILED(m_pVIBufferCom->Render()))
            //        return E_FAIL;
            //}
        }


        for (int i = 0; i < 6; i++)
        {

            if (FAILED(m_pIconTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", iPaimonIconIndex)))
                return E_FAIL;
            iPaimonIconIndex++;

            CombinedMatrix = m_PaimonIconsMat[i+20];
            if (m_iMOverIndex == i+20)
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
          
        //NameCard
        if (FAILED(m_pNameCardTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 3)))
            return E_FAIL;

        CombinedMatrix = m_NameCardMat[0];

        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;

        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 3)))
            return E_FAIL;


        //TextLayer
        CombinedMatrix = m_NameCardMat[1];

        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;

        if (FAILED(m_pPortraitTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
            return E_FAIL;

        //Portraits
        CombinedMatrix = m_NameCardMat[2];

        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;



        //Render Fonts
        //m_pGameInstance->Render_Font(TEXT("Font_Default"),)


    return S_OK;
}

CPaimon_Page* CPaimon_Page::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPaimon_Page* pInstance = new CPaimon_Page(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CPaimon_Page"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPaimon_Page::Clone(void* pArg)
{
    CPaimon_Page* pInstance = new CPaimon_Page(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CPaimon_Page"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPaimon_Page::Free()
{
    __super::Free();

    if (m_bCloned)
    {
        Safe_Release(m_pIconTextureCom);
        Safe_Release(m_pMainIconTextureCom);
        Safe_Release(m_pPortraitTextureCom);
        Safe_Release(m_pNameCardTextureCom);
        
    }
}
