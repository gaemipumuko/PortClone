#include "stdafx.h"
#include "Page_Main.h"

#include "UI_Manager.h"
#include "Inventory.h"
#include "GameInstance.h"


CPage_Main::CPage_Main(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : COrtho(pDevice, pContext)
{
}

CPage_Main::CPage_Main(const CPage_Main& rhs)
    : COrtho(rhs)
{
}

HRESULT CPage_Main::Add_Component(void* pArg)
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
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Main_Icons"),
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

	/* For.Com_Texture_Base */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Main_No_Responce"),
		TEXT("Com_Texture_Base"), (CComponent**)&m_pBaseTextureCom)))
		return E_FAIL;

    /* For.Com_Texture_Quest */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_QuestIcon"),
        TEXT("Com_Texture_Quest"), (CComponent**)&m_pQuestTextureCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CPage_Main::Bind_ShaderResources()
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

void CPage_Main::Display_Acquired(_float fTimedelta)
{
    m_fItem_DisplayTime += fTimedelta;

    for (auto it = m_Acquired_Items.begin(); it != m_Acquired_Items.end();)
    {
        if ((*it)->Get_item_Qty() <= 0) {
            Safe_Release(*it);
            it = m_Acquired_Items.erase(it);
            m_iNumDraw--;
        }
        else {
            ++it;
        }
    }

    if (m_fItem_DisplayTime >= 5.f)
    {

        for (auto iter : m_Acquired_Items)
        {
            Safe_Release(iter);
        }
        m_Acquired_Items.clear();

        m_iNumDraw = 0;
        m_fDisplayMargin = 100.f;
    }
    else
    {
        if (m_iNumDraw < m_Acquired_Items.size()|| m_iNumDraw == 0)
        {
            m_fDisplayMargin -= fTimedelta*200.f;

            if (m_fDisplayMargin <= 0.f)
            {
            m_iNumDraw++;
            m_fDisplayMargin = 100.f;
            m_fItem_DisplayTime = 0.f;
            }

        }
        else if (m_iNumDraw == m_Acquired_Items.size())
        {
            m_fDisplayMargin -= fTimedelta * 180.f;
            if (m_fDisplayMargin <= 0.f)
            {
                m_fDisplayMargin = 0.f;
            }

        }
    }



}


void CPage_Main::Update_Acquired_Rings(_int iRings)
{
    if(m_iAcquiredRings<iRings)
     m_bBoxBlink = true; 

     m_iAcquiredRings = iRings;

}

void CPage_Main::Register_On_QuestHUD(CQuest* pQuest)
{
    m_vecQuestDisplayed.push_back(pQuest);
    m_bQuestUpdated = true;
}

HRESULT CPage_Main::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CPage_Main::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component(pArg)))
        return E_FAIL;

    COrtho::Initialize_Matrix();

	/* Clone Main Page */
	m_pParty_Buttons = static_cast<CParty_Buttons*>(m_pGameInstance->Find_Object_Prototype(L"Prototype_GameObject_Party_Buttons")->Clone(nullptr));

    m_pMiniMap = CMini_Map::Create(m_pDevice, m_pContext);

#pragma region Icon Matrix Setup
    _float4x4 IconMatrx = { };
    XMStoreFloat4x4(&IconMatrx, XMMatrixIdentity());

    //Paimon Icon

    IconMatrx._11 = 58.f; IconMatrx._22 = 65.f;
    IconMatrx._41 = static_cast<_float>(45.f - g_iWinSizeX / 2); 
    IconMatrx._42 = static_cast<_float>(-(40.f - g_iWinSizeY / 2));
    m_RTIconsMatrix.push_back(IconMatrx);

    //Elemental Sight

    IconMatrx._11 = 42.f; IconMatrx._22 = 42.f;
    IconMatrx._41 = static_cast<_float>(230.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(30.f - g_iWinSizeY / 2));
    m_RTIconsMatrix.push_back(IconMatrx);

    //Co-Op

    IconMatrx._11 = 41.f; IconMatrx._22 = 41.f;
    IconMatrx._41 = static_cast<_float>(305.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(30.f - g_iWinSizeY / 2));
    m_RTIconsMatrix.push_back(IconMatrx);

    //Quest Icon
    IconMatrx._11 = 50.f; IconMatrx._22 = 50.f;
    IconMatrx._41 = static_cast<_float>(45.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(180.f - g_iWinSizeY / 2));
    m_RTIconsMatrix.push_back(IconMatrx);
	

    IconMatrx._11 = 42.f; IconMatrx._22 = 46.f;
    IconMatrx._41 = static_cast<_float>(1138.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(20.f - g_iWinSizeY / 2));
    m_RTIconsMatrix.push_back(IconMatrx);

    IconMatrx._11 = 43.f; IconMatrx._22 = 48.f;
    IconMatrx._41 = static_cast<_float>(1214.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(20.f - g_iWinSizeY / 2));
    m_RTIconsMatrix.push_back(IconMatrx);

	IconMatrx._11 = 44.f; IconMatrx._22 = 45.f;
	IconMatrx._41 = static_cast<_float>(1289.f - g_iWinSizeX / 2);
	IconMatrx._42 = static_cast<_float>(-(20.f - g_iWinSizeY / 2));
	m_RTIconsMatrix.push_back(IconMatrx);

	IconMatrx._11 = 50.f; IconMatrx._22 = 41.f;
	IconMatrx._41 = static_cast<_float>(1359.f - g_iWinSizeX / 2);
	IconMatrx._42 = static_cast<_float>(-(20.f - g_iWinSizeY / 2));
	m_RTIconsMatrix.push_back(IconMatrx);

	IconMatrx._11 = 43.f; IconMatrx._22 = 46.f;
	IconMatrx._41 = static_cast<_float>(1442.f - g_iWinSizeX / 2);
	IconMatrx._42 = static_cast<_float>(-(20.f - g_iWinSizeY / 2));
	m_RTIconsMatrix.push_back(IconMatrx);

	IconMatrx._11 = 42.f; IconMatrx._22 = 48.f;
	IconMatrx._41 = static_cast<_float>(1520.f - g_iWinSizeX / 2);
	IconMatrx._42 = static_cast<_float>(-(20.f - g_iWinSizeY / 2));
	m_RTIconsMatrix.push_back(IconMatrx);


    //Skill Buttons

	IconMatrx._11 = 75.f; IconMatrx._22 = 75.f;
	IconMatrx._41 = static_cast<_float>(1501.f - g_iWinSizeX / 2);
	IconMatrx._42 = static_cast<_float>(-(703.f - g_iWinSizeY / 2));
	m_vec_ImmobileMatrix.push_back(IconMatrx);

	IconMatrx._11 = 95.f; IconMatrx._22 = 95.f;
	IconMatrx._41 = static_cast<_float>(1501.f - g_iWinSizeX / 2);
	IconMatrx._42 = static_cast<_float>(-(810.f - g_iWinSizeY / 2));
	m_vec_ImmobileMatrix.push_back(IconMatrx);

	IconMatrx._11 = 65.f; IconMatrx._22 = 65.f;
	IconMatrx._41 = static_cast<_float>(1396.f - g_iWinSizeX / 2);
	IconMatrx._42 = static_cast<_float>(-(825.f - g_iWinSizeY / 2));
	m_vec_ImmobileMatrix.push_back(IconMatrx);


    //HP Bar
	IconMatrx._11 = 253.f; IconMatrx._22 = 9.f;
	IconMatrx._41 = static_cast<_float>(771.f - g_iWinSizeX / 2);
	IconMatrx._42 = static_cast<_float>(-(835.f - g_iWinSizeY / 2));
	m_vec_ImmobileMatrix.push_back(IconMatrx);

    
    // Top Overlay 
    IconMatrx._11 = 1100.f; IconMatrx._22 = 35.f;
    IconMatrx._41 = static_cast<_float>(800.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(155.f - g_iWinSizeY / 2));
    m_vec_ImmobileMatrix.push_back(IconMatrx);

    // Left Overlay 
    IconMatrx._11 = 240.f; IconMatrx._22 = 25.f;
    IconMatrx._41 = static_cast<_float>(170.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(315.f - g_iWinSizeY / 2));
    m_vecLOverlaySlot.push_back(IconMatrx);

    for (_int i = 0; i < 10; i++)
    {

        IconMatrx._11 = 330; IconMatrx._22 = 35.f;
        IconMatrx._41 = static_cast<_float>(170.f - g_iWinSizeX / 2);
        IconMatrx._42 = static_cast<_float>(-(310.f + (i+1) * 37.f - g_iWinSizeY / 2));
    m_vecLOverlaySlot.push_back(IconMatrx);
    }

    //Flight Test BGs
    //Time
    IconMatrx._11 = 220.f; IconMatrx._22 = 55.f;
    IconMatrx._41 = static_cast<_float>(790.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(110.f - g_iWinSizeY / 2));
    m_vecFlightTestMat.push_back(IconMatrx);

    //Acquired Rings
    IconMatrx._11 = 120.f; IconMatrx._22 = 40.f;
    IconMatrx._41 = static_cast<_float>(790.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(180.f - g_iWinSizeY / 2));
    m_vecFlightTestMat.push_back(IconMatrx);

    IconMatrx._11 = 30.f; IconMatrx._22 = 30.f;
    IconMatrx._41 = static_cast<_float>(765.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(180.f - g_iWinSizeY / 2));
    m_vecFlightTestMat.push_back(IconMatrx);

    m_vBoxColor = {0.2f,0.2f, 0.2f, -0.5f};

    IconMatrx._11 = 36.f; IconMatrx._22 = 36.f;
    IconMatrx._41 = static_cast<_float>(45.f - g_iWinSizeX / 2);
    IconMatrx._42 = static_cast<_float>(-(235.f - g_iWinSizeY / 2));
    m_QuestHUDMat = IconMatrx;

    m_eRenderType = RTYPE_GAMEPLAY;

    return S_OK;
}

HRESULT CPage_Main::PreviousTick(_float fTimeDelta)
{
    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {
        if (FAILED(__super::PreviousTick(fTimeDelta)))
            return E_FAIL;

		m_pParty_Buttons->PreviousTick(fTimeDelta);
        m_pMiniMap->PreviousTick(fTimeDelta);
    }
    
    return S_OK;
}

HRESULT CPage_Main::Tick(_float fTimeDelta)
{
    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {
        if (FAILED(__super::Tick(fTimeDelta)))
            return E_FAIL;

        if (m_bPresent_Top_Overlay == true)
        {
            m_fOverlay_DisplayTime += fTimeDelta;

            if (m_fOverlay_DisplayTime >= 3.f)
            {

                m_bPresent_Top_Overlay = false;
                m_fOverlay_DisplayTime = 0.f;
            }
        }

        if (m_bQuestUpdated == true)
        {
            m_fQuestBlinkTimer += fTimeDelta*6.f;

            if (m_fQuestBlinkTimer >= 1.f)
            {
                m_bQuestUpdated = false;
                m_fQuestBlinkTimer = 0.f;
            }
        }

        if (m_pGameInstance->Key_Down('P'))
        {
            m_bBoxBlink = true;
            m_bQuestUpdated = true;
        }

        if (m_bBoxBlink == true)
        {
            m_vBoxColor.m128_f32[0] += fTimeDelta * 2.5f;
            m_vBoxColor.m128_f32[1] += fTimeDelta * 2.5f;
            m_vBoxColor.m128_f32[2] += fTimeDelta * 2.5f *0.7f;
            m_vBoxColor.m128_f32[3] += fTimeDelta * 2.5f *0.6f;
            
            if (m_vBoxColor.m128_f32[0] > 1.f)
            {
                m_bBoxBlink = false;
                m_vBoxColor = { 0.2f,0.2f, 0.2f, -0.5f };
            }
        }

        if (m_bRegionNameDisplayed == true)
        {
            if (fTimeDelta < 1.f)
            {
                m_fTime_Remain += fTimeDelta;
                m_fTimeAcc += fTimeDelta;
                m_fFontAlpha = sin(m_fTimeAcc) * 3.f;

                if (m_fFontAlpha < 0.f)
                    m_bRegionNameDisplayed = false;
            }
        }

        if (m_Acquired_Items.empty() != true)
        {
            Display_Acquired(fTimeDelta);
        }

		m_pParty_Buttons->Tick(fTimeDelta);
        m_pMiniMap->Tick(fTimeDelta);

        POINT       ptMouse;
        RECT        rcDimentions;

        GetCursorPos(&ptMouse);
        ScreenToClient(g_hWnd, &ptMouse);

        ptMouse.x = ptMouse.x - g_iWinSizeX / 2;
        ptMouse.y = (-ptMouse.y + g_iWinSizeY / 2);

        _bool bOvered = { FALSE };
        for (_int iIndex = 0; iIndex < m_RTIconsMatrix.size(); ++iIndex)
        {
            rcDimentions.left = static_cast<LONG>(m_RTIconsMatrix[iIndex]._41 - m_RTIconsMatrix[iIndex]._11 / 2.f);
            rcDimentions.top = static_cast<LONG>(m_RTIconsMatrix[iIndex]._42 - m_RTIconsMatrix[iIndex]._22 / 2.f);
            rcDimentions.right = static_cast<LONG>(m_RTIconsMatrix[iIndex]._41 + m_RTIconsMatrix[iIndex]._11 / 2.f);
            rcDimentions.bottom = static_cast<LONG>(m_RTIconsMatrix[iIndex]._42 + m_RTIconsMatrix[iIndex]._22 / 2.f);

            if (PtInRect(&rcDimentions, ptMouse))
            {
                bOvered = TRUE;
                m_iMOverIndex = iIndex;
                break;
            }
        }

        if (bOvered == FALSE)
            m_iMOverIndex = -1;
    }

	if(m_pGameInstance->Key_Down(VK_LBUTTON) && (m_iMOverIndex != -1))
		{
        CUI_Manager* UI_Manager = (CUI_Manager*)m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_UIManager"));
        CInventory* pInven = (CInventory*)m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Inventory"));
		switch (m_iMOverIndex)
		{
		case 0:
            UI_Manager->Show_Page(PAGE_PAIMON);
			//Paimon_Page Open
			break;
		case 1:
			//Elemental Sight
			break;
		case 2:
			//Co_op 
			break;
		case 3:
			//Quest Icon
			break;
		case 4:
			//Event Icon
			break;
		case 5:
			//BattlePass
			break;
		case 6:
            UI_Manager->Show_Page(PAGE_GACHA);
            m_pGameInstance->PlayBGM(L"Gacha_Bgm.mp3", 0.3f);
			//Wish Gacha
			break;
		case 7:
			//Adventure Note
			break;
		case 8:
			//Inventory
            m_pGameInstance->PlaySoundW(L"Click.mp3", 0.8f);
            pInven->Open_Inventory();
			break;
		case 9:
			//Character Icon
			break;
		default:
			break;
		}
	}

    for (_int i = 0; i < m_vecQuestDisplayed.size(); i++)
    {
        if (m_vecQuestDisplayed[i]->Get_QuestUpdated())
        {
            m_bQuestUpdated = true;
            m_vecQuestDisplayed[i]->Set_QuestUpdated(false);
        }

        if (m_vecQuestDisplayed[i]->Get_QuestState() == CQuest::QUEST_COMPLETED)
        {
            m_vecQuestDisplayed.erase(m_vecQuestDisplayed.begin() + i);
        }
    }

    return S_OK;
}

HRESULT CPage_Main::LateTick(_float fTimeDelta)
{
    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {
        if (FAILED(__super::LateTick(fTimeDelta)))
            return E_FAIL;

		m_pParty_Buttons->LateTick(fTimeDelta);
        m_pMiniMap->LateTick(fTimeDelta);
    }
    
    return S_OK;
}

HRESULT CPage_Main::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _float4x4 CombinedMatrix = {};
    for (_uint iIndex = 0; iIndex < m_pTextureCom->Get_NumTexture(); ++iIndex)
    {
        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", iIndex)))
            return E_FAIL;

        CombinedMatrix = m_RTIconsMatrix[iIndex];
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
    }


        if (FAILED(m_pBaseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
            return E_FAIL;

        CombinedMatrix = m_vec_ImmobileMatrix[0];

        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;


    if (FAILED(m_pBaseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 1)))
        return E_FAIL;

    CombinedMatrix = m_vec_ImmobileMatrix[3];

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;


    //Page_Main Top Overlay 

#pragma region Top Overlay
    if (m_bPresent_Top_Overlay == true)
    {

    //Icon Behind
    if (FAILED(m_pQuestTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    CombinedMatrix = m_vec_ImmobileMatrix[4];
    CombinedMatrix._11 = 150.f;
    CombinedMatrix._22 = 150.f;


    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
        return E_FAIL;

    _vector vColor = { 0.87058823529f,0.7294117647f ,1.f, -0.7f };

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vOverlayColor, sizeof(vColor))))
        return E_FAIL;

    _float2 UVScale = { 70.f / 1500.f , 70.f / 1000.f };
    _float2 UVPos {315.f / 1500.f, 20.f / 1000.f};

    if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &UVScale, sizeof(_float2))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &UVPos, sizeof(_float2))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(UI_DEFINED)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    // OBI

    if (FAILED(m_pBaseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 2)))
        return E_FAIL;

    CombinedMatrix = m_vec_ImmobileMatrix[4];

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
        return E_FAIL;

    vColor = { 0.87058823529f,0.7294117647f ,1.f, -0.5f };

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vOverlayColor, sizeof(vColor))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &m_vUVScale, sizeof(_float2))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &m_vUVPosition, sizeof(_float2))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(UI_DEFINED)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    //Top Overlay Fonts

    m_pGameInstance->Render_Font(L"Font_Default", m_strOverlay, { 750.f,142.f }, 0.6f, { 0.9f,0.9f,0.9f,1.f }, 0.f, { 0.f,0.f }, false);

    }
#pragma endregion


#pragma region Left Overlay
    if (m_Acquired_Items.empty() != true)
    {
        // OBI

        if (FAILED(m_pBaseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 2)))
            return E_FAIL;

        //_float4x4 IconMatrx = { };
        //XMStoreFloat4x4(&IconMatrx, XMMatrixIdentity());
        //IconMatrx._11 = 240.f; IconMatrx._22 = 25.f;
        //IconMatrx._41 = static_cast<_float>(170.f - g_iWinSizeX / 2);
        //IconMatrx._42 = static_cast<_float>(-(315.f - g_iWinSizeY / 2));
        //m_vecLOverlaySlot.push_back(IconMatrx);

        CombinedMatrix = m_vecLOverlaySlot[0];

        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            return E_FAIL;

        _vector vColor = { 1.f,1.f ,1.f, -0.8f };

        if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(vColor))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &m_vUVScale, sizeof(_float2))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &m_vUVPosition, sizeof(_float2))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_DEFINED)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;

        //Left Overlay Fonts

        m_pGameInstance->Render_Font(L"Font_Default", TEXT("»πµÊ æ∆¿Ã≈€"), {130.f,305.f}, 0.4f, {1.f,1.f,1.f,1.f}, 0.f, {0.f,0.f}, false);

        //Left Overlay Item Obi & Items
        
        for (_int i = 0; i < m_iNumDraw; i++)
        {
            if (FAILED(m_pBaseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 2)))
                return E_FAIL;
            
            CombinedMatrix = m_vecLOverlaySlot[i + 1];
            if (i + 1 == m_iNumDraw)
            {
                CombinedMatrix._42 -= m_fDisplayMargin;
            }


            if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
                return E_FAIL;
            vColor = { 0.2f,0.2f ,0.2f, -0.5f };

            if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(vColor))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(UI_DEFINED)))
                return E_FAIL;

            if (FAILED(m_pVIBufferCom->Render()))
                return E_FAIL;

            //CombinedMatrix = m_vecLOverlaySlot[i+1];
            m_Acquired_Items[i]->Render_Icon(_float2(CombinedMatrix._41 - CombinedMatrix._11/4.f, CombinedMatrix._42),_float2(40.f, 35.f));


            //Left Overlay Item Fonts
            wstring	strL_Overlay = m_Acquired_Items[i]->Get_Item_Name();
            _int    iQty = m_Acquired_Items[i]->Get_item_Qty();
            _tchar szQty[MAX_PATH];
            _float2 vScreenPos = {CombinedMatrix._41 + g_iWinSizeX/2 - 50.f ,-(CombinedMatrix._42 - g_iWinSizeY/2)-10.f };
            wsprintf(szQty, TEXT("x %d"), iQty);
            strL_Overlay.append(szQty);
            m_pGameInstance->Render_Font(L"Font_Default", strL_Overlay,vScreenPos , 0.4f, { 1.f,1.f,1.f,1.f }, 0.f, { 0.f,0.f }, false);


        }
    }

#pragma endregion
    //Region Name Display
    if (m_bRegionNameDisplayed == true)
    {
        _vector vColor = { 0.9f* (m_fFontAlpha / 3.f),0.9f* (m_fFontAlpha / 3.f),0.9f* (m_fFontAlpha / 3.f), 0.f };
        
        m_pGameInstance->Render_Font(L"Font_Default", m_strRegion, { 800.f,142.f }, 1.f, vColor, 0.f, { 0.f,0.f }, true);

    }

    //Flight Test Display
    if (m_bFlightTestDisplay == true)
    {
        if (FAILED(m_pBaseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 3)))
            return E_FAIL;

        CombinedMatrix = m_vecFlightTestMat[0];

        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            return E_FAIL;

        _vector vColor = { 0.2f,0.2f ,0.2f, -0.5f };

        if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(vColor))))
            return E_FAIL;
        if (FAILED(m_pShaderCom->Begin(UI_DEFINED)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;


        if (m_bRingFTDisplay == TRUE)
        {
            if (FAILED(m_pBaseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 3)))
                return E_FAIL;

            CombinedMatrix = m_vecFlightTestMat[1];

            if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vBoxColor, sizeof(vColor))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(UI_DEFINED)))
                return E_FAIL;

            if (FAILED(m_pVIBufferCom->Render()))
                return E_FAIL;

            if (FAILED(m_pBaseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 4)))
                return E_FAIL;

            CombinedMatrix = m_vecFlightTestMat[2];

            if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
                return E_FAIL;

            if (FAILED(m_pVIBufferCom->Render()))
                return E_FAIL;
        }

        _tchar szTime[MAX_PATH];

        swprintf_s(szTime, sizeof(szTime) / sizeof(wchar_t), L"%.2f", m_fTime_Remain);
        m_pGameInstance->Render_Font(L"Font_Default", szTime, { 792.f,89.f }, 0.9f, Colors::White, 0.f, { 0.f,0.f }, true);


        if (m_bRingFTDisplay == TRUE)
        {
            if (m_iAcquiredRings < 10)
                wsprintf(szTime, TEXT("x 0%d"), m_iAcquiredRings);
            else
                wsprintf(szTime, TEXT("x %d"), m_iAcquiredRings);

            m_pGameInstance->Render_Font(L"Font_Default", szTime, { 782.f,165.f }, 0.6f, Colors::White, 0.f, { 0.f,0.f }, false);
        }

        //Quest HUD
    }
        for (_int i = 0; i < m_vecQuestDisplayed.size(); i++)
        {
            wstring strName = m_vecQuestDisplayed[i]->Get_QuestName();
            wstring strDetail = m_vecQuestDisplayed[i]->Get_QuestDetail();
            CQuest::QUEST_TYPES eQuestType = m_vecQuestDisplayed[i]->Get_QuestType();
            _float2 fIconUVPos, fIconUvScale;
            _vector vLineColor = { 1.f,1.f,0.65f,0.f };
            switch (eQuestType)
            {
            case Client::CQuest::QUEST_NORMAL:
                if (FAILED(m_pQuestTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
                    return E_FAIL;
                fIconUVPos = { 143.f / 1500.f,733.f / 1000.f };
                fIconUvScale = { 70.f / 1500.f,80.f / 1000.f };
                vLineColor = { 1.f,1.f,0.75f,0.f };
                break;
            case Client::CQuest::QUEST_LEGACY:
                if (FAILED(m_pQuestTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
                    return E_FAIL;
                fIconUVPos = { 145.f / 1500.f,611.f / 1000.f };
                fIconUvScale = { 68.f / 1500.f,80.f / 1000.f };
                vLineColor = {0.43921568627f, 0.79215686274f, 1.f, 0.f};
                    break;
            case Client::CQuest::QUEST_LEGENDARY:
                if (FAILED(m_pQuestTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
                    return E_FAIL;
                fIconUVPos = { 368.f / 1500.f,632.f / 1000.f };
                fIconUvScale = { 77.f / 1500.f,76.f / 1000.f };
                vLineColor = { 0.73333333333f, 0.50980392156f, 0.82745098039f, 0.f };
                break;
            case Client::CQuest::QUEST_END:
                fIconUVPos = { 0.f,0.f };
                fIconUvScale = { 0.f,0.f };
                break;
            default:
                break;
            }

            if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &fIconUvScale, sizeof(_float2))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &fIconUVPos, sizeof(_float2))))
                return E_FAIL;

            CombinedMatrix = m_QuestHUDMat;
            CombinedMatrix._42 -= 50.f * i;

            if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
                return E_FAIL;

            if (FAILED(m_pVIBufferCom->Render()))
                return E_FAIL;

            //Trail


            if (FAILED(m_pBaseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture",6)))
                return E_FAIL;
            fIconUVPos = { 0.f,0.f };
            fIconUvScale = { 1.f,1.f };
            if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &fIconUvScale, sizeof(_float2))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &fIconUVPos, sizeof(_float2))))
                return E_FAIL;

            _vector vHighlightColor = { 1.f,1.f,0.65f+ (m_fQuestBlinkTimer*0.3f),( - 1.f+m_fQuestBlinkTimer)};
            if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vHighlightColor, sizeof(_vector))))

            CombinedMatrix = m_QuestHUDMat;
            CombinedMatrix._11 = 190.f;
            CombinedMatrix._22 = 110.f;

            CombinedMatrix._41 += 110.f;
            CombinedMatrix._42 -= 14.f;

            if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(UI_DEFINED)))
                return E_FAIL;

            if (FAILED(m_pVIBufferCom->Render()))
                return E_FAIL;

            //UnderLine
            if (FAILED(m_pBaseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture",5)))
                return E_FAIL;
            fIconUVPos = { 0.f,0.f };
            fIconUvScale = { 1.f,1.f };
            if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &fIconUvScale, sizeof(_float2))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &fIconUVPos, sizeof(_float2))))
                return E_FAIL;

            CombinedMatrix = m_QuestHUDMat;
            CombinedMatrix._11 = 265.f;
            CombinedMatrix._22 = 5.f;

            CombinedMatrix._41 += 150.f;
            CombinedMatrix._42 -= 10.f;

            if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vLineColor, sizeof(_vector))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(UI_DEFINED)))
                return E_FAIL;

            if (FAILED(m_pVIBufferCom->Render()))
                return E_FAIL;

            m_pGameInstance->Render_Font(L"Font_Default", strName, { 60.f,225.f + 50.f * i }, 0.4f, Colors::White, 0.f, { 0.f,0.f }, false);
            m_pGameInstance->Render_Font(L"Font_Default", strDetail, { 70.f,255.f+ 50.f * i }, 0.35f, Colors::White, 0.f, { 0.f,0.f }, false);

        }

    


    return S_OK;
}


void CPage_Main::Register_On_MiniMap(CTransform* pTargetTransform, _int iIconIndex)
{
    m_pMiniMap->Register_OnMap(pTargetTransform, (CMini_Map::MAP_ICON)iIconIndex);
}

CPage_Main* CPage_Main::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPage_Main* pInstance = new CPage_Main(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CPage_Main"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPage_Main::Clone(void* pArg)
{
    CPage_Main* pInstance = new CPage_Main(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CPage_Main"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPage_Main::Request_Top_Overlay(_vector vColor, wstring strOverlayText)
{
    if (m_bPresent_Top_Overlay == false)
    {
        m_bPresent_Top_Overlay = true;
        m_vOverlayColor = vColor;
        m_strOverlay = strOverlayText;
    }
}

_bool CPage_Main::Register_On_AcquiredItems(CItem* pItem)
{
    if (m_Acquired_Items.size() < 10)
    {
        if (m_Acquired_Items.empty())
        {
            m_Acquired_Items.push_back(pItem);
            Safe_AddRef(pItem);
            m_fItem_DisplayTime = 0.f;
        }
        else
        {

            for (auto& iter : m_Acquired_Items)
            {
                if (iter->Get_Item_Name() == pItem->Get_Item_Name())
                {
                    iter->Stash_Item(pItem->Get_item_Qty());
                    m_fItem_DisplayTime = 0.f;
                    return FALSE;
                }
            }

            m_Acquired_Items.push_back(pItem);
            Safe_AddRef(pItem);
            m_fItem_DisplayTime = 0.f;
        }


        
    return TRUE;
    }

    return FALSE;
}

void CPage_Main::Display_Region_Name(wstring strRegion)
{
    m_fTimeAcc = 0.f;
    m_strRegion = strRegion;
    m_bRegionNameDisplayed = true;

}

void CPage_Main::Free()
{
    __super::Free();

    if (m_bCloned)
    {
	Safe_Release(m_pBaseTextureCom);
    Safe_Release(m_pParty_Buttons);
    Safe_Release(m_pMiniMap);
    Safe_Release(m_pQuestTextureCom);

    if (m_Acquired_Items.empty() == false)
    {
        for (auto iter : m_Acquired_Items)
        {
            Safe_Release(iter);
        }
        m_Acquired_Items.clear();
    }
    }
}

