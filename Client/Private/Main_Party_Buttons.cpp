#include "stdafx.h"
#include "Main_Party_Buttons.h"

#include "Part_Status.h"
#include "Player.h"
#include "GameInstance.h"


CParty_Buttons::CParty_Buttons(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : COrtho(pDevice, pContext)
{
}

CParty_Buttons::CParty_Buttons(const CParty_Buttons& rhs)
    : COrtho(rhs)
{
}

HRESULT CParty_Buttons::Add_Component(void* pArg)
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
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Main_Party_Buttons"),
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

	/* For.Com_Texture_Icon */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_AvatarIcon_Side"),
		TEXT("Com_Texture_Icon"), (CComponent**)&m_pIconTextureCom)))
		return E_FAIL;

    /* For.Com_Texture_Element */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Skill_Elemental"),
        TEXT("Com_Texture_Element"), (CComponent**)&m_pSkillTextureCom)))
        return E_FAIL;

    /* For.Com_Texture_Burst */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Skill_Burst"),
        TEXT("Com_Texture_Burst"), (CComponent**)&m_pBurstTextureCom)))
        return E_FAIL;

    /* For.Com_Texture_Stamina */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Stamina_Bar"),
        TEXT("Com_Texture_Stamina"), (CComponent**)&m_pStaminaTextureCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CParty_Buttons::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
        return E_FAIL;

    //if (FAILED(m_pShaderCom->Bind_RawValue("g_bInput", &m_bTagEnabled, sizeof(_bool))))
    //    return E_FAIL;

    return S_OK;
}

HRESULT CParty_Buttons::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CParty_Buttons::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component(pArg)))
        return E_FAIL;

    COrtho::Initialize_Matrix();

    _float4x4 IconMatrx = { };
    XMStoreFloat4x4(&IconMatrx, XMMatrixIdentity());

	// tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_CharacterBg");
// tOrthoDesc.fPosition = { static_cast<_float>(1504.f - g_iWinSizeX / 2), static_cast<_float>(-(230.f - g_iWinSizeY / 2)) };
// tOrthoDesc.fSize = { 200.f,64.f };
// tOrthoDesc.ePage = PAGE_MAIN;
// if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Button_Alpha"), strLayerTag, &tOrthoDesc)))
// 	return E_FAIL;

    IconMatrx._11 = 160.f; IconMatrx._22 = 64.f;
    IconMatrx._41 = static_cast<_float>(1464.f - g_iWinSizeX / 2); 
    IconMatrx._42 = static_cast<_float>(-(230.f - g_iWinSizeY / 2));
    m_ButtonBaseMat.push_back(IconMatrx);

	// tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_CharacterBg");
// tOrthoDesc.fPosition = { static_cast<_float>(1504.f - g_iWinSizeX / 2), static_cast<_float>(-(311.f - g_iWinSizeY / 2)) };
// tOrthoDesc.fSize = { 200.f,64.f };
// tOrthoDesc.ePage = PAGE_MAIN;
// if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Button_Alpha"), strLayerTag, &tOrthoDesc)))
// 	return E_FAIL;
	IconMatrx._11 = 160.f; IconMatrx._22 = 64.f;
	IconMatrx._41 = static_cast<_float>(1464.f - g_iWinSizeX / 2);
	IconMatrx._42 = static_cast<_float>(-(311.f - g_iWinSizeY / 2));
	m_ButtonBaseMat.push_back(IconMatrx);

	// tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_CharacterBg");
// tOrthoDesc.fPosition = { static_cast<_float>(1504.f - g_iWinSizeX / 2), static_cast<_float>(-(387.f - g_iWinSizeY / 2)) };
// tOrthoDesc.fSize = { 200.f,64.f };
// tOrthoDesc.ePage = PAGE_MAIN;
// if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Button_Alpha"), strLayerTag, &tOrthoDesc)))
// 	return E_FAIL;
	IconMatrx._11 = 160.f; IconMatrx._22 = 64.f;
	IconMatrx._41 = static_cast<_float>(1464.f - g_iWinSizeX / 2);
	IconMatrx._42 = static_cast<_float>(-(387.f - g_iWinSizeY / 2));
	m_ButtonBaseMat.push_back(IconMatrx);

	// tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_CharacterBg");
// tOrthoDesc.fPosition = { static_cast<_float>(1504.f - g_iWinSizeX / 2), static_cast<_float>(-(472.f - g_iWinSizeY / 2)) };
// tOrthoDesc.fSize = { 200.f,64.f };
// tOrthoDesc.ePage = PAGE_MAIN;
// if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Button_Alpha"), strLayerTag, &tOrthoDesc)))
// 	return E_FAIL;
	IconMatrx._11 = 160.f; IconMatrx._22 = 64.f;
	IconMatrx._41 = static_cast<_float>(1464.f - g_iWinSizeX / 2);
	IconMatrx._42 = static_cast<_float>(-(472.f - g_iWinSizeY / 2));
	m_ButtonBaseMat.push_back(IconMatrx);

	// tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_AvatarIcon_Side");
// tOrthoDesc.fPosition = { static_cast<_float>(1504.f - g_iWinSizeX / 2), static_cast<_float>(-(220.f - g_iWinSizeY / 2)) };
// tOrthoDesc.fSize = { 64.f,64.f };
// tOrthoDesc.ePage = PAGE_MAIN;
// if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Button_Alpha"), strLayerTag, &tOrthoDesc)))
// 	return E_FAIL;
	IconMatrx._11 = 64.f; IconMatrx._22 = 64.f;
	IconMatrx._41 = static_cast<_float>(1504.f - g_iWinSizeX / 2);
	IconMatrx._42 = static_cast<_float>(-(220.f - g_iWinSizeY / 2));
	m_IconMat.push_back(IconMatrx);

	// tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_AvatarIcon_Side");
// tOrthoDesc.fPosition = { static_cast<_float>(1504.f - g_iWinSizeX / 2), static_cast<_float>(-(301.f - g_iWinSizeY / 2)) };
// tOrthoDesc.fSize = { 64.f,64.f };
// tOrthoDesc.ePage = PAGE_MAIN;
// if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Button_Alpha"), strLayerTag, &tOrthoDesc)))
// 	return E_FAIL;
	IconMatrx._11 = 64.f; IconMatrx._22 = 64.f;
	IconMatrx._41 = static_cast<_float>(1504.f - g_iWinSizeX / 2);
	IconMatrx._42 = static_cast<_float>(-(301.f - g_iWinSizeY / 2));
	m_IconMat.push_back(IconMatrx);

	// tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_AvatarIcon_Side");
// tOrthoDesc.fPosition = { static_cast<_float>(1504.f - g_iWinSizeX / 2), static_cast<_float>(-(377.f - g_iWinSizeY / 2)) };
// tOrthoDesc.fSize = { 64.f,64.f };
// tOrthoDesc.ePage = PAGE_MAIN;
// if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Button_Alpha"), strLayerTag, &tOrthoDesc)))
// 	return E_FAIL;

	IconMatrx._11 = 64.f; IconMatrx._22 = 64.f;
	IconMatrx._41 = static_cast<_float>(1504.f - g_iWinSizeX / 2);
	IconMatrx._42 = static_cast<_float>(-(377.f - g_iWinSizeY / 2));
	m_IconMat.push_back(IconMatrx);

    // tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_AvatarIcon_Side");
// tOrthoDesc.fPosition = { static_cast<_float>(1504.f - g_iWinSizeX / 2), static_cast<_float>(-(462.f - g_iWinSizeY / 2)) };
// tOrthoDesc.fSize = { 64.f,64.f };
// tOrthoDesc.ePage = PAGE_MAIN;
// if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Button_Alpha"), strLayerTag, &tOrthoDesc)))
// 	return E_FAIL;

	IconMatrx._11 = 64.f; IconMatrx._22 = 64.f;
	IconMatrx._41 = static_cast<_float>(1504.f - g_iWinSizeX / 2);
	IconMatrx._42 = static_cast<_float>(-(462.f - g_iWinSizeY / 2));
	m_IconMat.push_back(IconMatrx);


    IconMatrx._11 = 64.f; IconMatrx._22 = 64.f;
    IconMatrx._41 = static_cast<_float>(1344.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(230.f - g_iWinSizeY / 2));
    m_BurstSCMat.push_back(IconMatrx);


    IconMatrx._11 = 64.f; IconMatrx._22 = 64.f;
    IconMatrx._41 = static_cast<_float>(1344.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(311.f - g_iWinSizeY / 2));
    m_BurstSCMat.push_back(IconMatrx);


    IconMatrx._11 = 64.f; IconMatrx._22 = 64.f;
    IconMatrx._41 = static_cast<_float>(1344.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(387.f - g_iWinSizeY / 2));
    m_BurstSCMat.push_back(IconMatrx);


    IconMatrx._11 = 64.f; IconMatrx._22 = 64.f;
    IconMatrx._41 = static_cast<_float>(1344.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(472.f - g_iWinSizeY / 2));
    m_BurstSCMat.push_back(IconMatrx);

    

	// tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_ButtonBase");
// tOrthoDesc.fPosition = { static_cast<_float>(1551.f - g_iWinSizeX / 2), static_cast<_float>(-(230.f - g_iWinSizeY / 2)) };
// tOrthoDesc.fSize = { 30.f,24.f };
// tOrthoDesc.ePage = PAGE_MAIN;
// if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Button_Alpha"), strLayerTag, &tOrthoDesc)))
// 	return E_FAIL;
	IconMatrx._11 = 30.f; IconMatrx._22 = 24.f;
	IconMatrx._41 = static_cast<_float>(1551.f - g_iWinSizeX / 2);
	IconMatrx._42 = static_cast<_float>(-(230.f - g_iWinSizeY / 2));
    m_ImmobileMat.push_back(IconMatrx);
	// tOrthoDesc.fPosition = { static_cast<_float>(1551.f - g_iWinSizeX / 2), static_cast<_float>(-(311.f - g_iWinSizeY / 2)) };
// tOrthoDesc.fSize = { 30.f,24.f };
// tOrthoDesc.ePage = PAGE_MAIN;
// if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Rect"), strLayerTag, &tOrthoDesc)))
// 	return E_FAIL;
	IconMatrx._11 = 30.f; IconMatrx._22 = 24.f;
	IconMatrx._41 = static_cast<_float>(1551.f - g_iWinSizeX / 2);
	IconMatrx._42 = static_cast<_float>(-(311.f - g_iWinSizeY / 2));
    m_ImmobileMat.push_back(IconMatrx);
	// tOrthoDesc.fPosition = { static_cast<_float>(1551.f - g_iWinSizeX / 2), static_cast<_float>(-(387.f - g_iWinSizeY / 2)) };
// tOrthoDesc.fSize = { 30.f,24.f };
// tOrthoDesc.ePage = PAGE_MAIN;
// if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Rect"), strLayerTag, &tOrthoDesc)))
// 	return E_FAIL;

	IconMatrx._11 = 30.f; IconMatrx._22 = 24.f;
	IconMatrx._41 = static_cast<_float>(1551.f - g_iWinSizeX / 2);
	IconMatrx._42 = static_cast<_float>(-(387.f - g_iWinSizeY / 2));
    m_ImmobileMat.push_back(IconMatrx);


	// tOrthoDesc.fPosition = { static_cast<_float>(1551.f - g_iWinSizeX / 2), static_cast<_float>(-(472.f - g_iWinSizeY / 2)) };
// tOrthoDesc.fSize = { 30.f,24.f };
// tOrthoDesc.ePage = PAGE_MAIN;
// if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Rect"), strLayerTag, &tOrthoDesc)))
// 	return E_FAIL;
// 
	IconMatrx._11 = 30.f; IconMatrx._22 = 24.f;
	IconMatrx._41 = static_cast<_float>(1551.f - g_iWinSizeX / 2);
	IconMatrx._42 = static_cast<_float>(-(472.f - g_iWinSizeY / 2));
    m_ImmobileMat.push_back(IconMatrx);


    //Skill Buttons
    IconMatrx._11 = 80.f; IconMatrx._22 = 80.f;
    IconMatrx._41 = static_cast<_float>(1501.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(810.f - g_iWinSizeY / 2));
    m_ImmobileMat.push_back(IconMatrx);

    IconMatrx._11 = 50.f; IconMatrx._22 = 50.f;
    IconMatrx._41 = static_cast<_float>(1396.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(825.f - g_iWinSizeY / 2));
    m_ImmobileMat.push_back(IconMatrx);
    
    // 
    // tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_Talent_Nahida_01");
    // tOrthoDesc.fPosition = { static_cast<_float>(1501.f - g_iWinSizeX / 2), static_cast<_float>(-(810.f - g_iWinSizeY / 2)) };
    // tOrthoDesc.fSize = { 80.f,80.f };
    // tOrthoDesc.ePage = PAGE_MAIN;
    // if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Button_Alpha"), strLayerTag, &tOrthoDesc)))
    // 	return E_FAIL;
    // 
    // tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_Talent_Nahida_02");
    // tOrthoDesc.fPosition = { static_cast<_float>(1396.f - g_iWinSizeX / 2), static_cast<_float>(-(825.f - g_iWinSizeY / 2)) };
    // tOrthoDesc.fSize = { 50.f,50.f };
    // tOrthoDesc.ePage = PAGE_MAIN;
    // if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Button_Alpha"), strLayerTag, &tOrthoDesc)))
    // 	return E_FAIL;
    // 



	// tOrthoDesc.fPosition = { static_cast<_float>(1501.f - g_iWinSizeX / 2), static_cast<_float>(-(856.f - g_iWinSizeY / 2)) };
	// tOrthoDesc.fSize = { 30.f,21.f };
	// tOrthoDesc.ePage = PAGE_MAIN;
	// if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Rect"), strLayerTag, &tOrthoDesc)))
	// 	return E_FAIL;
	 	IconMatrx._11 = 30.f; IconMatrx._22 = 21.f;
	IconMatrx._41 = static_cast<_float>(1501.f - g_iWinSizeX / 2);
	IconMatrx._42 = static_cast<_float>(-(856.f - g_iWinSizeY / 2));
    m_ImmobileMat.push_back(IconMatrx);
	// tOrthoDesc.fPosition = { static_cast<_float>(1396.f - g_iWinSizeX / 2), static_cast<_float>(-(856.f - g_iWinSizeY / 2)) };
	// tOrthoDesc.fSize = { 30.f,21.f };
	// tOrthoDesc.ePage = PAGE_MAIN;
	// if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Rect"), strLayerTag, &tOrthoDesc)))
	// 	return E_FAIL;
	IconMatrx._11 = 30.f; IconMatrx._22 = 21.f;
	IconMatrx._41 = static_cast<_float>(1396.f - g_iWinSizeX / 2);
	IconMatrx._42 = static_cast<_float>(-(472.f - g_iWinSizeY / 2));
    m_ImmobileMat.push_back(IconMatrx);


    IconMatrx._11 = 50.f; IconMatrx._22 = 5.f;
    IconMatrx._41 = static_cast<_float>(1454.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(245.f - g_iWinSizeY / 2));
    m_HealthMat.push_back(IconMatrx);

    IconMatrx._11 = 50.f; IconMatrx._22 = 5.f;
    IconMatrx._41 = static_cast<_float>(1454.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(326.f - g_iWinSizeY / 2));
    m_HealthMat.push_back(IconMatrx);

    IconMatrx._11 = 50.f; IconMatrx._22 = 5.f;
    IconMatrx._41 = static_cast<_float>(1454.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(402.f - g_iWinSizeY / 2));
    m_HealthMat.push_back(IconMatrx);

    IconMatrx._11 = 50.f; IconMatrx._22 = 5.f;
    IconMatrx._41 = static_cast<_float>(1454.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(487.f - g_iWinSizeY / 2));
    m_HealthMat.push_back(IconMatrx);

    //Stamina Matrices

    IconMatrx._11 = 50.f; IconMatrx._22 = 150.f;
    IconMatrx._41 = static_cast<_float>(1000.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(300.f - g_iWinSizeY / 2));
    m_StaminaMat.push_back(IconMatrx);

    IconMatrx._11 = 50.f; IconMatrx._22 = 150.f;
    IconMatrx._41 = static_cast<_float>(1000.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(300.f - g_iWinSizeY / 2));
    m_StaminaMat.push_back(IconMatrx);

    COrtho::ORTHO_DESC tOrthoDesc;

    tOrthoDesc.eTextureLevel = LEVEL_STATIC;
    tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_HPBar");
    tOrthoDesc.fPosition = { static_cast<_float>(1454.f - g_iWinSizeX / 2), static_cast<_float>(-(245.f - g_iWinSizeY / 2)) };
    tOrthoDesc.fSize = { 50.f,9.f };
    tOrthoDesc.ePage = PAGE_MAIN;
    m_pHp_Bar[0] = static_cast<CHp_Bar*>(m_pGameInstance->Find_Object_Prototype(L"Prototype_GameObject_Ui_Hp_Bar")->Clone(&tOrthoDesc));

    tOrthoDesc.eTextureLevel = LEVEL_STATIC;
    tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_HPBar");
    tOrthoDesc.fPosition = { static_cast<_float>(1454.f - g_iWinSizeX / 2), static_cast<_float>(-(326.f - g_iWinSizeY / 2)) };
    tOrthoDesc.fSize = { 50.f,5.f };
    tOrthoDesc.ePage = PAGE_MAIN;
    m_pHp_Bar[1] = (CHp_Bar*)m_pGameInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Ui_Hp_Bar"))->Clone(&tOrthoDesc);

    tOrthoDesc.eTextureLevel = LEVEL_STATIC;
    tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_HPBar");
    tOrthoDesc.fPosition = { static_cast<_float>(1454.f - g_iWinSizeX / 2), static_cast<_float>(-(402.f - g_iWinSizeY / 2)) };
    tOrthoDesc.fSize = { 50.f,5.f };
    tOrthoDesc.ePage = PAGE_MAIN;
    m_pHp_Bar[2] = (CHp_Bar*)m_pGameInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Ui_Hp_Bar"))->Clone(&tOrthoDesc);

    tOrthoDesc.eTextureLevel = LEVEL_STATIC;
    tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_HPBar");
    tOrthoDesc.fPosition = { static_cast<_float>(1454.f - g_iWinSizeX / 2), static_cast<_float>(-(487.f - g_iWinSizeY / 2)) };
    tOrthoDesc.fSize = { 50.f,5.f };
    tOrthoDesc.ePage = PAGE_MAIN;
    m_pHp_Bar[3] = (CHp_Bar*)m_pGameInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Ui_Hp_Bar"))->Clone(&tOrthoDesc);




    m_eRenderType = RTYPE_GAMEPLAY;

    m_pHp_Bar[m_iMemberIndex]->Update_Matrix(static_cast<_float>(771.f - g_iWinSizeX / 2), static_cast<_float>(-(835.f - g_iWinSizeY / 2)), 253.f, 9.f);

    m_vElementColor[0] = { 255.f / 255.f,89.f / 255.f,71.f / 255.f,0.8f };
    m_vElementColor[1] = { 207.f / 255.f,181.f / 255.f,91.f / 255.f,0.8f };
    m_vElementColor[2] = { 70.f / 255.f,180.f / 255.f,255.f / 255.f,0.8f };
    m_vElementColor[3] = { 161.f / 255.f,125.f / 255.f,1.f ,1.f };


    return S_OK;
}

HRESULT CParty_Buttons::PreviousTick(_float fTimeDelta)
{
    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {
        if (FAILED(__super::PreviousTick(fTimeDelta)))
            return E_FAIL;

    }

    return S_OK;
}

HRESULT CParty_Buttons::Tick(_float fTimeDelta)
{
    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {
        if (FAILED(__super::Tick(fTimeDelta)))
            return E_FAIL;

        m_fTimeAcc += fTimeDelta;

        if (m_fCoolDownRate > 0.f)
        {
            m_fCoolDownRate -= fTimeDelta;

        }
        else
            m_fCoolDownRate = 0.f;

            if (m_fBurstDownRate > 0.f)
            {

                m_fBurstDownRate -= fTimeDelta;
            }
            else

            m_fBurstDownRate = 0.f;

        if (m_pGameInstance->Key_Down('K'))
        {
            m_fCoolDownRate = 1.f;
            m_fBurstDownRate = 1.f;
        }

       // m_pStamina_Bar->Tick(fTimeDelta);
        for (int i = 0; i < 4; i++)
        {
            m_pHp_Bar[i]->Tick(fTimeDelta);
        }

        if ( m_iMemberIndex==0 && m_bTagEnabled == TRUE)
        {

            m_pHp_Bar[0]->Update_Hp_Matrix(static_cast<_float>(771.f - g_iWinSizeX / 2), static_cast<_float>(-(835.f - g_iWinSizeY / 2)), 253.f, 9.f);
            m_pHp_Bar[1]->Update_Hp_Matrix(static_cast<_float>(1454.f - g_iWinSizeX / 2), static_cast<_float>(-(326.f - g_iWinSizeY / 2)), 50.f, 5.f);
            m_pHp_Bar[2]->Update_Hp_Matrix(static_cast<_float>(1454.f - g_iWinSizeX / 2), static_cast<_float>(-(402.f - g_iWinSizeY / 2)), 50.f, 5.f);
            m_pHp_Bar[3]->Update_Hp_Matrix(static_cast<_float>(1454.f - g_iWinSizeX / 2), static_cast<_float>(-(487.f - g_iWinSizeY / 2)), 50.f, 5.f);

            
        }

        if (m_iMemberIndex == 1 && m_bTagEnabled ==TRUE)
        {
            m_pHp_Bar[0]->Update_Hp_Matrix(static_cast<_float>(1454.f - g_iWinSizeX / 2), static_cast<_float>(-(245.f - g_iWinSizeY / 2)), 50.f, 5.f);
            m_pHp_Bar[1]->Update_Hp_Matrix(static_cast<_float>(771.f - g_iWinSizeX / 2), static_cast<_float>(-(835.f - g_iWinSizeY / 2)), 253.f, 9.f);
            m_pHp_Bar[2]->Update_Hp_Matrix(static_cast<_float>(1454.f - g_iWinSizeX / 2), static_cast<_float>(-(402.f - g_iWinSizeY / 2)), 50.f, 5.f);
            m_pHp_Bar[3]->Update_Hp_Matrix(static_cast<_float>(1454.f - g_iWinSizeX / 2), static_cast<_float>(-(487.f - g_iWinSizeY / 2)), 50.f, 5.f);



        }

        if (m_iMemberIndex == 2 && m_bTagEnabled == TRUE)
        {

            m_pHp_Bar[0]->Update_Hp_Matrix(static_cast<_float>(1454.f - g_iWinSizeX / 2), static_cast<_float>(-(245.f - g_iWinSizeY / 2)), 50.f, 5.f);
            m_pHp_Bar[1]->Update_Hp_Matrix(static_cast<_float>(1454.f - g_iWinSizeX / 2), static_cast<_float>(-(326.f - g_iWinSizeY / 2)), 50.f, 5.f);
            m_pHp_Bar[2]->Update_Hp_Matrix(static_cast<_float>(771.f - g_iWinSizeX / 2), static_cast<_float>(-(835.f - g_iWinSizeY / 2)), 253.f, 9.f);
            m_pHp_Bar[3]->Update_Hp_Matrix(static_cast<_float>(1454.f - g_iWinSizeX / 2), static_cast<_float>(-(487.f - g_iWinSizeY / 2)), 50.f, 5.f);

           
        }

        if (m_iMemberIndex == 3 && m_bTagEnabled == TRUE)
        {

          

            m_pHp_Bar[0]->Update_Hp_Matrix(static_cast<_float>(1454.f - g_iWinSizeX / 2), static_cast<_float>(-(245.f - g_iWinSizeY / 2)), 50.f, 5.f);
            m_pHp_Bar[1]->Update_Hp_Matrix(static_cast<_float>(1454.f - g_iWinSizeX / 2), static_cast<_float>(-(326.f - g_iWinSizeY / 2)), 50.f, 5.f);
            m_pHp_Bar[2]->Update_Hp_Matrix(static_cast<_float>(1454.f - g_iWinSizeX / 2), static_cast<_float>(-(402.f - g_iWinSizeY / 2)), 50.f, 5.f);
            m_pHp_Bar[3]->Update_Hp_Matrix(static_cast<_float>(771.f - g_iWinSizeX / 2), static_cast<_float>(-(835.f - g_iWinSizeY / 2)), 253.f, 9.f);
         
           
        }

        //CD/ELEM Check

        if (m_pGameInstance->Key_Pressing('P'))
        {
            if(m_fTempCurrentStamina>=0.f)
            m_fTempCurrentStamina -= 1.f;
        }
        if (m_pGameInstance->Key_Pressing('K'))
        {
            if (m_fTempCurrentStamina <= 100.f)
            m_fTempCurrentStamina += 1.f;
        }


        if (m_fTempCurrentStamina / m_fTempTotalStamina <= 0.15f)
        {

            if (m_bFadeOut == true)
            {
                m_fStaminaAlpha -= fTimeDelta*0.7f;
                if (m_fStaminaAlpha <= -0.3f)
                {
                    m_bFadeOut = false;
                }
            }
            else
            {
                m_fStaminaAlpha += fTimeDelta*0.7f;
                if (m_fStaminaAlpha >= 0.3f)
                {
                    m_bFadeOut = true;
                }
            }
        }

    }
    return S_OK;
}

HRESULT CParty_Buttons::LateTick(_float fTimeDelta)
{
    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {

        if (FAILED(__super::LateTick(fTimeDelta)))
            return E_FAIL;

        for (int i = 0; i < m_iNumMembers; i++)
        {

            m_pHp_Bar[i]->LateTick(fTimeDelta);

        }
    }
    
    return S_OK;
}

HRESULT CParty_Buttons::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

	_float4x4 CombinedMatrix = {};
    _float2 vUVScale = { 1.f,1.f };
    _float2 vUVPos = { 0.f,0.f };
    _bool   bTrue = {};

    for (_int iIndex = 0; iIndex < m_iNumMembers; ++iIndex)
    {


        if (m_iMemberIndex == iIndex)
        {
            _vector vColor = { 1.f,1.f ,1.f,0.f };

            if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(vColor))))
                return E_FAIL;

           if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 1)))
               return E_FAIL;
        }
        else
        {
        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
            return E_FAIL;
        }
        CombinedMatrix = m_ButtonBaseMat[iIndex];
        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &m_vUVScale, sizeof(_float2))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &m_vUVPosition, sizeof(_float2))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;


        // Render Icons

        if (FAILED(m_pIconTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iMemberID[iIndex])))
            return E_FAIL;

        CombinedMatrix = m_IconMat[iIndex];

        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()
        ))
            return E_FAIL;

        if (m_iMemberIndex != iIndex)
        {
        // Render Key Buttons
        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 2)))
            return E_FAIL;

        CombinedMatrix = m_ImmobileMat[iIndex];
        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
            return E_FAIL;


        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;

        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 3)))
            return E_FAIL;

        CombinedMatrix = m_HealthMat[iIndex];
        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;


        }

  

    }

    //Render Skill Icons

    //m_BurstSCMat
    for (_int iIndex = 0; iIndex < m_iNumMembers; ++iIndex)
    {
        if (m_iMemberIndex != iIndex && m_bBurstReady[iIndex] == TRUE)
        {
            if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 5)))
                return E_FAIL;

            _float4x4 BaseMatrix = m_BurstSCMat[iIndex];
            _float    fElemRate = 1.f;

            if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", BaseMatrix)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_RawValue("g_fRate", &fElemRate, sizeof(_float))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vElementColor[iIndex], sizeof(_vector))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(UI_SKILL_ICON_BASE)))
                return E_FAIL;

            if (FAILED(m_pVIBufferCom->Render()))
                return E_FAIL;

            if (FAILED(m_pBurstTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iMemberID[iIndex])))
                return E_FAIL;

            CombinedMatrix = m_BurstSCMat[iIndex];
            CombinedMatrix._11 *= 0.8f;
            CombinedMatrix._22 *= 0.8f;

            if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
                return E_FAIL;

            if (FAILED(m_pVIBufferCom->Render()))
                return E_FAIL;

            if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 6 + iIndex)))
                return E_FAIL;

            _float2 vPos;

            BaseMatrix = m_BurstSCMat[iIndex];
            vPos.x = BaseMatrix._41;
            vPos.y = BaseMatrix._42;
            BaseMatrix._11 *= 1.5f;
            BaseMatrix._22 *= 1.5f;
            XMStoreFloat4x4(&BaseMatrix, (XMLoadFloat4x4(&BaseMatrix) * XMMatrixRotationZ(-m_fTimeAcc)));
            BaseMatrix._41 = vPos.x;
            BaseMatrix._42 = vPos.y;

            if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", BaseMatrix)))
                return E_FAIL;


            if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
                return E_FAIL;

            if (FAILED(m_pVIBufferCom->Render()))
                return E_FAIL;

        }
    }
    

    //if (FAILED(m_pShaderCom->Bind_RawValue("g_bInput", &bTrue, sizeof(_bool))))
    //    return E_FAIL;

    //base
    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 5)))
        return E_FAIL;

    _float4x4 BaseMatrix = m_ImmobileMat[4];
    BaseMatrix._11 *= 1.2f;
    BaseMatrix._22 *= 1.2f;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", BaseMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fRate", &m_fElementRate, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vElementColor[m_iMemberIndex], sizeof(_vector))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(UI_SKILL_ICON_BASE)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    //ICON
    if (FAILED(m_pBurstTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iMemberID[m_iMemberIndex])))
        return E_FAIL;

    if (m_fBurstDownRate > 0.f)
    {
        bTrue = false;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_bInput", &bTrue, sizeof(_bool))))
            return E_FAIL;

    }

    CombinedMatrix = m_ImmobileMat[4];
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
        return E_FAIL;
    

    if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    bTrue = true;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_bInput", &bTrue, sizeof(_bool))))
        return E_FAIL;


    if (m_fElementRate == 1.f)
    {
        _float fCool = {1.f};

        //Circle
        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 4)))
            return E_FAIL;

        CombinedMatrix = m_ImmobileMat[4];
        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vElementColor[m_iMemberIndex], sizeof(_vector))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_fCoolDown", &fCool, sizeof(_float))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_COOLDOWN_CIRCLE)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;
    }
    else
    {

        //Circle
        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 4)))
            return E_FAIL;

        CombinedMatrix = m_ImmobileMat[4];
        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            return E_FAIL;

        _vector vCooldownColor = { 1.f,1.f,1.f,1.f };

        if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vCooldownColor, sizeof(_vector))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_fCoolDown", &m_fBurstDownRate, sizeof(_float))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_COOLDOWN_CIRCLE)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;

    }

    if (m_fElementRate == 1.f)
    {

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 6+ m_iMemberIndex)))
        return E_FAIL;

    _float2 vPos;

    BaseMatrix = m_ImmobileMat[4];
    vPos.x = BaseMatrix._41;
    vPos.y = BaseMatrix._42;
    BaseMatrix._11 *= 1.8f;
    BaseMatrix._22 *= 1.8f;
    XMStoreFloat4x4(&BaseMatrix, (XMLoadFloat4x4(&BaseMatrix)* XMMatrixRotationZ(-m_fTimeAcc)));
    BaseMatrix._41 = vPos.x;
    BaseMatrix._42 = vPos.y;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", BaseMatrix)))
        return E_FAIL;


    if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;
    }

    //Burst Base

    BaseMatrix = m_ImmobileMat[5];
    BaseMatrix._11 *= 1.2f;
    BaseMatrix._22 *= 1.2f;
    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture",5)))
        return E_FAIL;
    CombinedMatrix = m_ImmobileMat[5];
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", BaseMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    //Burst
    if (FAILED(m_pSkillTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iMemberID[m_iMemberIndex])))
        return E_FAIL;

    if (m_fCoolDownRate > 0.f)
    {
        bTrue = false;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_bInput", &bTrue, sizeof(_bool))))
            return E_FAIL;

    }

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    bTrue = true;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_bInput", &bTrue, sizeof(_bool))))
        return E_FAIL;

    //Circle
    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 4)))
        return E_FAIL;

    CombinedMatrix = m_ImmobileMat[5];
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
        return E_FAIL;

    _vector vCooldownColor = { 1.f,1.f,1.f,1.f };

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vCooldownColor, sizeof(_vector))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fCoolDown", &m_fCoolDownRate, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(UI_COOLDOWN_CIRCLE)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    //Render Stamina Bar

    _vector vStaminaAlpha = { 1.f,1.f,1.f,0.f };

    if (m_fTempCurrentStamina / m_fTempTotalStamina < 1.f)
    {

        if (FAILED(m_pStaminaTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
            return E_FAIL;

        CombinedMatrix = m_StaminaMat[0];
        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;

        if (m_fTempCurrentStamina / m_fTempTotalStamina <= 0.5f && m_fTempCurrentStamina / m_fTempTotalStamina >= 0.15f)
        {
            if (FAILED(m_pStaminaTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 1)))
                return E_FAIL;

            CombinedMatrix = m_StaminaMat[0];
            if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
                return E_FAIL;

            if (FAILED(m_pVIBufferCom->Render()))
                return E_FAIL;
        }
        if (m_fTempCurrentStamina / m_fTempTotalStamina <= 0.15f)
        {

            if (FAILED(m_pStaminaTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 1)))
                return E_FAIL;

            CombinedMatrix = m_StaminaMat[0];
            if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
                return E_FAIL;


            if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &vUVScale, sizeof(_float2))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &vUVPos, sizeof(_float2))))
                return E_FAIL;

            vStaminaAlpha.m128_f32[3] = m_fStaminaAlpha;

            if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vStaminaAlpha, sizeof(_vector))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(UI_DEFINED)))
                return E_FAIL;

            if (FAILED(m_pVIBufferCom->Render()))
                return E_FAIL;

            if (FAILED(m_pStaminaTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 2)))
                return E_FAIL;

            CombinedMatrix = m_StaminaMat[0];
            CombinedMatrix._11 *= 1.1f;
            CombinedMatrix._22 *= 1.1f;
            CombinedMatrix._41 -= 2.f;

            if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &vUVScale, sizeof(_float2))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &vUVPos, sizeof(_float2))))
                return E_FAIL;

            vStaminaAlpha.m128_f32[1] = 0.f;
            vStaminaAlpha.m128_f32[2] = 0.f;

            if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vStaminaAlpha, sizeof(_vector))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(UI_DEFINED)))
                return E_FAIL;

            if (FAILED(m_pVIBufferCom->Render()))
                return E_FAIL;

        }


        if (FAILED(m_pStaminaTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 3)))
            return E_FAIL;

        CombinedMatrix = m_StaminaMat[1];
        _float fStaminaRatio = m_fTempCurrentStamina / m_fTempTotalStamina;
        CombinedMatrix._22 = m_StaminaMat[1]._22 * fStaminaRatio;
        CombinedMatrix._42 = m_StaminaMat[1]._42 - (m_StaminaMat[0]._22 - CombinedMatrix._22) / 2;

        vUVPos.y = 1 - fStaminaRatio;
        vUVScale.y = fStaminaRatio;
        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &vUVScale, sizeof(_float2))))


            return E_FAIL;
        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &vUVPos, sizeof(_float2))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_fStamina", &fStaminaRatio, sizeof(_float))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_STAMINA)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;
    }

    //Render Fonts
    wstring m_strOwnerName = { TEXT("다이루크") };
    _int iLength = static_cast<_int>(m_strOwnerName.size());
    _vector vColor = {0.f,0.f,0.f,0.f};
    //if (m_bTagEnabled == true)
    //else
    //    vColor = Colors::Gray;

        vColor = Colors::White;
    if (m_iMemberIndex != 0)
    {

        m_pGameInstance->Render_Font(L"Font_Default", TEXT("1"), _float2(1556.f, 223.f), 0.35f, Colors::Black);
        m_pGameInstance->Render_Font(L"Font_Default", m_strOwnerName, _float2(1454.f - iLength / 2 * 38 * 0.5f, 215.f), 0.4f, vColor, 0.f, { 0.f,0.f }, false);
    }
    if (m_iMemberIndex != 1)
    {
        m_strOwnerName = { TEXT("종려") };
        iLength = static_cast<_int>(m_strOwnerName.size());
        m_pGameInstance->Render_Font(L"Font_Default", m_strOwnerName, _float2(1454.f - iLength / 2 * 38 * 0.5f, 296.f), 0.4f, vColor, 0.f, { 0.f,0.f }, false);
        m_pGameInstance->Render_Font(L"Font_Default", TEXT("2"), _float2(1560.f, 303.f), 0.35f, Colors::Black);
    }
    if (m_iMemberIndex != 2)
    {
        m_strOwnerName = { TEXT("바바라") };
        iLength = static_cast<_int>(m_strOwnerName.size());
        m_pGameInstance->Render_Font(L"Font_Default", m_strOwnerName, _float2(1454.f - iLength / 2 * 38 * 0.5f, 372.f), 0.4f, Colors::White, 0.f, { 0.f,0.f }, false);
        m_pGameInstance->Render_Font(L"Font_Default", TEXT("3"), _float2(1560.f, 379.f), 0.35f, Colors::Black);
    }
    if (m_iMemberIndex != 3)
    {
        m_strOwnerName = { TEXT("라이덴 쇼군") };
        iLength = static_cast<_int>(m_strOwnerName.size());
        m_pGameInstance->Render_Font(L"Font_Default", m_strOwnerName, _float2(1454.f - iLength / 2 * 38 * 0.5f, 457.f), 0.4f, Colors::White, 0.f, { 0.f,0.f }, false);
        m_pGameInstance->Render_Font(L"Font_Default", TEXT("4"), _float2(1560.f, 464.f), 0.35f, Colors::Black);
    }

    _uint iLevel = 90;
    _tchar szLevel[MAX_PATH];
    if (iLevel != 0)
    {
        wsprintf(szLevel, TEXT("Lv.%d"), iLevel);

        m_pGameInstance->Render_Font(L"Font_Default", szLevel, _float2(651.f,826.f), 0.4f, Colors::White);
    }


    return S_OK;
}

CParty_Buttons* CParty_Buttons::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CParty_Buttons* pInstance = new CParty_Buttons(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CParty_Buttons"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CParty_Buttons::Clone(void* pArg)
{
    CParty_Buttons* pInstance = new CParty_Buttons(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CParty_Buttons"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CParty_Buttons::Free()
{
    __super::Free();

    if (m_bCloned == true)
    {
	Safe_Release(m_pIconTextureCom);
    Safe_Release(m_pBurstTextureCom);
    Safe_Release(m_pSkillTextureCom);
    Safe_Release(m_pStaminaTextureCom);

    for (int i = 0; i < 4; i++)
    {
        Safe_Release(m_pHp_Bar[i]);
    }
    }
}
