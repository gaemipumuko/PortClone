#include "stdafx.h"
#include "InteractionBox.h"

#include "GameInstance.h"
#include "AlphaRect.h"

#include "UI_Manager.h"

CInteraction_Box::CInteraction_Box(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : COrtho(pDevice, pContext)
{
}

CInteraction_Box::CInteraction_Box(const CInteraction_Box& rhs)
    : COrtho(rhs)
{
}

HRESULT CInteraction_Box::Add_Component(void* pArg)
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
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Interaction_Box"),
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Interaction_Icons"),
        TEXT("Com_Texture_Icon"), (CComponent**)&m_pInteractionIconCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CInteraction_Box::Bind_ShaderResources()
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

void CInteraction_Box::Clear_Boxes()
{
        m_BoxDescList.clear();
        m_iNumBoxes = 0;
        m_iSelectedIndex = 0;

}

void CInteraction_Box::Add_InteractionBox(wstring strInteractionBox, INTERACTION_ICON eIconType)
{
    if (m_iNumBoxes<6)
    {

    INTERACTION_DESC tInteractionDesc = {};
    tInteractionDesc.strBoxText = strInteractionBox;
    tInteractionDesc.eBoxIcon = eIconType;

    m_BoxDescList.push_back(tInteractionDesc);
    m_iNumBoxes++;
    }

}

_int CInteraction_Box::Get_SelectedIndex()
{
    if (m_iIndexOnScreen >= 0 && m_iNumBoxes > 0)
    {
        m_pGameInstance->PlaySoundW(L"InteractionBox.mp3", 0.7f);
        return m_iSelectedIndex;
    }
    
    return -1;
}

HRESULT CInteraction_Box::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CInteraction_Box::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component(pArg)))
        return E_FAIL;

    COrtho::Initialize_Matrix();


    _float4x4 ButtonMat = { };
    XMStoreFloat4x4(&ButtonMat, XMMatrixIdentity());


    //Button Origin
    ButtonMat._11 = 220.f; ButtonMat._22 = 45.f;
    ButtonMat._41 = static_cast<_float>(1075.f - g_iWinSizeX / 2);
    ButtonMat._42 = static_cast<_float>(-(450.f - g_iWinSizeY / 2));
    m_vecBoxMat.push_back(ButtonMat);

    ButtonMat._11 = 220.f; ButtonMat._22 = 45.f;
    ButtonMat._41 = static_cast<_float>(1075.f - g_iWinSizeX / 2);
    ButtonMat._42 = static_cast<_float>(-(495.f - g_iWinSizeY / 2));
    m_vecBoxMat.push_back(ButtonMat);

    ButtonMat._11 = 220.f; ButtonMat._22 = 45.f;
    ButtonMat._41 = static_cast<_float>(1075.f - g_iWinSizeX / 2);
    ButtonMat._42 = static_cast<_float>(-(540.f - g_iWinSizeY / 2));
    m_vecBoxMat.push_back(ButtonMat);

    ButtonMat._11 = 220.f; ButtonMat._22 = 45.f;
    ButtonMat._41 = static_cast<_float>(1075.f - g_iWinSizeX / 2);
    ButtonMat._42 = static_cast<_float>(-(585.f - g_iWinSizeY / 2));
    m_vecBoxMat.push_back(ButtonMat);

    ButtonMat._11 = 220.f; ButtonMat._22 = 45.f;

    ButtonMat._41 = static_cast<_float>(1075.f - g_iWinSizeX / 2);
    ButtonMat._42 = static_cast<_float>(-(630.f - g_iWinSizeY / 2));
    m_vecBoxMat.push_back(ButtonMat);

    ButtonMat._11 = 220.f; ButtonMat._22 = 45.f;
    ButtonMat._41 = static_cast<_float>(1075.f - g_iWinSizeX / 2);
    ButtonMat._42 = static_cast<_float>(-(675.f - g_iWinSizeY / 2));
    m_vecBoxMat.push_back(ButtonMat);

    m_eRenderType = RTYPE_GAMEPLAY;

    return S_OK;
}

HRESULT CInteraction_Box::PreviousTick(_float fTimeDelta)
{
    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {
        if (FAILED(__super::PreviousTick(fTimeDelta)))
            return E_FAIL;
    }
    return S_OK;
}

HRESULT CInteraction_Box::Tick(_float fTimeDelta)
{

    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {
        if (FAILED(__super::Tick(fTimeDelta)))
            return E_FAIL;

        m_pUI_Manager = (CUI_Manager*)m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_UIManager"));
        m_iIndexOnScreen = m_pUI_Manager->Register_InteractionBox(m_iNumBoxes) - m_iNumBoxes;
        m_iSelectedIndex = m_pUI_Manager->Get_SelectedBoxIdx()- m_iIndexOnScreen;

        if(m_iIndexOnScreen == 0 )
            m_fDisplayMargin = 0.f;

        if (m_fDisplayMargin > 0.f)
            m_fDisplayMargin -= fTimeDelta*30;
        else
            m_fDisplayMargin = 0.f;

#pragma region Click Related
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

        if (m_pGameInstance->Key_Down(VK_LBUTTON))
        {

        if (PtInRect(&rcbtn, ptMouse))
        {


        }

        }
#pragma endregion

        m_fTimeAcc += fTimeDelta;
        m_fSinRepeat = sin(m_fTimeAcc*4.5f);

#ifdef _DEBUG
        if (m_pGameInstance->Key_Down('N'))
        {
            m_BoxDescList.clear();
            m_iNumBoxes = 0;
            m_iSelectedIndex = 0;
        }
#endif

    }
    return S_OK;
}

HRESULT CInteraction_Box::LateTick(_float fTimeDelta)
{

    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {
        if (m_iNumBoxes > 0)
        {
            if (m_iIndexOnScreen >= 0)
            {

                if (FAILED(__super::LateTick(fTimeDelta)))
                    return E_FAIL;

                if(m_pUI_Manager !=nullptr)
                m_iNumBoxOnScreen = m_pUI_Manager->Get_NumBox_Registered();
            }
        }


    }
    return S_OK;
}

HRESULT CInteraction_Box::Render()
{

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _float  fSlide = 20.f;
    _float4x4 CombinedMatrix = {};
    _float2 vUVScale = { 1.f,1.f };
    _float2 vUVPos = { 0.f,0.f };
    list<INTERACTION_DESC>::iterator iter = m_BoxDescList.begin();

    for (_int iIndex = 0; iIndex < m_iNumBoxes; iIndex++)
    {

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    CombinedMatrix = m_vecBoxMat[m_iIndexOnScreen+iIndex];
    CombinedMatrix._42 += fSlide * m_iNumBoxOnScreen - m_fDisplayMargin;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;


    //Render Fonts
    _int iLength;
    iLength = static_cast<_int>((*iter).strBoxText.size());
    m_pGameInstance->Render_Font(L"Font_Default", (*iter).strBoxText, _float2(CombinedMatrix._41 + g_iWinSizeX/2 -60.f, -(CombinedMatrix._42 + 11.f -g_iWinSizeY/2)), 0.5f, { 1.f ,1.f,1.f,1.f },0.f, { 0.f,0.f }, false);
    ++iter;
    }


    if (m_iIndexOnScreen == 0)
    {

    if (FAILED(m_pInteractionIconCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    CombinedMatrix = m_vecBoxMat[m_iSelectedIndex];
    CombinedMatrix._11 = 15.f;
    CombinedMatrix._22 = 17.f;
    CombinedMatrix._41 -= m_vecBoxMat[m_iSelectedIndex]._11 / 2.f +10.f + m_fSinRepeat*3.f;
    CombinedMatrix._42 += fSlide * m_iNumBoxOnScreen - m_fDisplayMargin;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
        return E_FAIL;

    vUVPos = _float2(903.f/1024.f, 240.f/1024.f);
    vUVScale = _float2(30.f/1024.f , 34.f/1024.f);

    if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &vUVScale, sizeof(_float2))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &vUVPos, sizeof(_float2))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    }

    return S_OK;
}

CInteraction_Box* CInteraction_Box::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CInteraction_Box* pInstance = new CInteraction_Box(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CInteraction_Box"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CInteraction_Box::Clone(void* pArg)
{
    CInteraction_Box* pInstance = new CInteraction_Box(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CInteraction_Box"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CInteraction_Box::Free()
{
    __super::Free();

    if (m_bCloned)
    {
        Safe_Release(m_pInteractionIconCom);
    }

}
