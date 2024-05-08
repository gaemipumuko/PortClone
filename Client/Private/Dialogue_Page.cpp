#include "stdafx.h"
#include "Dialogue_Page.h"

#include "GameInstance.h"

CDialogue_Page::CDialogue_Page(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : COrtho(pDevice, pContext)
{
}

CDialogue_Page::CDialogue_Page(const CDialogue_Page& rhs)
    : COrtho(rhs)
{
}

HRESULT CDialogue_Page::Add_Component(void* pArg)
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
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Dialogue_Objects"),
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CDialogue_Page::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
        return E_FAIL;

    return S_OK;
}

HRESULT CDialogue_Page::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CDialogue_Page::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component(pArg)))
        return E_FAIL;

    COrtho::Initialize_Matrix();

    _float4x4 DialoguePgMat = { };
    XMStoreFloat4x4(&DialoguePgMat, XMMatrixIdentity());


    //Kuro_Obi
    DialoguePgMat._11 = 1220.f; DialoguePgMat._22 = 250.f;
    DialoguePgMat._41 = static_cast<_float>(0.f);
    DialoguePgMat._42 = static_cast<_float>(-(815.f - g_iWinSizeY/2));
    m_DialogueBoxMat = DialoguePgMat;

    //Decor L
    DialoguePgMat._11 = 50.f; DialoguePgMat._22 = 20.f;
    DialoguePgMat._41 = static_cast<_float>(0.f);
    DialoguePgMat._42 = static_cast<_float>(-(750.f - g_iWinSizeY / 2));
    m_DecorLMat.push_back(DialoguePgMat);

    //Decor L Mid
    DialoguePgMat._11 = 75.f; DialoguePgMat._22 = 20.f;
    DialoguePgMat._41 = static_cast<_float>(0.f);
    DialoguePgMat._42 = static_cast<_float>(-(750.f - g_iWinSizeY / 2));
    m_DecorLMat.push_back(DialoguePgMat);

    //Decor L End
    DialoguePgMat._11 = 46.f; DialoguePgMat._22 = 20.f;
    DialoguePgMat._41 = static_cast<_float>(0.f);
    DialoguePgMat._42 = static_cast<_float>(-(750.f - g_iWinSizeY / 2));
    m_DecorLMat.push_back(DialoguePgMat);
    
    //Decor R
    DialoguePgMat._11 = 46.f; DialoguePgMat._22 = 20.f;
    DialoguePgMat._41 = static_cast<_float>(0.f);
    DialoguePgMat._42 = static_cast<_float>(-(750.f - g_iWinSizeY / 2));
    m_DecorRMat.push_back(DialoguePgMat);

    //Decor R Mid
    DialoguePgMat._11 = 75.f; DialoguePgMat._22 = 20.f;
    DialoguePgMat._41 = static_cast<_float>(0.f);
    DialoguePgMat._42 = static_cast<_float>(-(750.f - g_iWinSizeY / 2));
    m_DecorRMat.push_back(DialoguePgMat);

    //Decor R End
    DialoguePgMat._11 = 50.f; DialoguePgMat._22 = 20.f;
    DialoguePgMat._41 = static_cast<_float>(0.f);
    DialoguePgMat._42 = static_cast<_float>(-(750.f - g_iWinSizeY / 2));
    m_DecorRMat.push_back(DialoguePgMat);

    m_eRenderType = RTYPE_GAMEPLAY;

    return S_OK;
}

HRESULT CDialogue_Page::PreviousTick(_float fTimeDelta)
{
    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {
        if (FAILED(__super::PreviousTick(fTimeDelta)))
            return E_FAIL;
    }
    return S_OK;
}

HRESULT CDialogue_Page::Tick(_float fTimeDelta)
{


    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {
        if (FAILED(__super::Tick(fTimeDelta)))
            return E_FAIL;

        POINT       ptMouse;
        RECT        rcDimentions, rcbtn;

        if (m_pGameInstance->Key_Down(VK_LBUTTON))
        {
            m_pGameInstance->PlaySoundW(L"InteractionBox.mp3", 0.7f);
        }

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

        _int iLength;

        if (m_strNPC_Script.size() >= 30 && m_strNPC_Script.size() <= 100)
        {
            iLength = static_cast<_int>(m_strNPC_Script.size());
        }
        else if (m_strNPC_Script.size() <= 30)
        {
            iLength = 30;
        }
        else
        {
            iLength = 100;
        }

        m_fScriptLength = iLength * 15.f * 0.5f *2.f;
        m_fScriptStartX = 0.f - m_fScriptLength / 2.f;
        m_fScriptEndX = 0.f + m_fScriptLength / 2.f;
        //LTail X,Length
        if (m_strNPC_Occupation.empty() == true)
        {
            m_fLTailX = m_fScriptStartX - (m_DecorLMat[0]._11 / 2.f);
            m_fLTail_Len = m_DecorLMat[0]._11;

            m_fLMidX = 0.f;
            m_fLMid_Len = m_fScriptLength;

            m_fRTailX = m_fScriptEndX + (m_DecorRMat[2]._11 / 2.f);
            m_fRTail_Len = m_DecorRMat[2]._11;

        }
        else
        {

            iLength = static_cast<_int>(m_strNPC_Occupation.size());
            m_fOccupationLength = iLength * 15.f * 0.5f *2.f;
            m_fOccupationStartX = 0.f - m_fOccupationLength*0.5f;
            m_fOccupationXEnd = 0.f + m_fOccupationLength*0.5f;

            //LHead


            m_fLHeadX = m_fOccupationStartX - m_DecorLMat[2]._11*0.5f;
            m_fLHead_Len = m_DecorLMat[2]._11;

            //LMid
            m_fLMid_Len = abs(m_fScriptStartX - (m_fLHeadX - m_fLHead_Len * 0.5f));
            m_fLMidX = (m_fLHeadX - m_fLHead_Len * 0.5f) - m_fLMid_Len * 0.5f;

            //LTail
            m_fLTailX = m_fScriptStartX - (m_DecorLMat[0]._11 / 2.f);
            m_fLTail_Len = m_DecorLMat[0]._11;

            m_fRHeadX = m_fOccupationXEnd + m_DecorRMat[0]._11 / 2.f;
            m_fRHead_Len = m_DecorRMat[0]._11;

            m_fRMid_Len = m_fLMid_Len;
            m_fRMidX = (m_fRHeadX + m_fRHead_Len / 2.f) + (m_fRMid_Len / 2.f);

            m_fRTail_Len = m_DecorRMat[2]._11;
            m_fRTailX = m_fRMidX + m_fRMid_Len / 2.f + m_fRTail_Len/2.f;
        }

        

        iLength = static_cast<_int>(m_strNPC_Occupation.size());
    }
    return S_OK;
}

HRESULT CDialogue_Page::LateTick(_float fTimeDelta)
{

    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {
        if (FAILED(__super::LateTick(fTimeDelta)))
            return E_FAIL;


    }
    return S_OK;
}

HRESULT CDialogue_Page::Render()
{
    //_int iLength;
    //_float fScriptStartX, fScriptEndX, fOccupationX, fOccupationXEnd, fOccupationLength, fScriptLength;
    _float2 vUVScale = { 1.f,1.f };
    _float2 vUVPos = { 0.f,0.f};
    _float4x4 CombinedMatrix = {};

    //fScriptLength = iLength * 15.f * 0.5f;
    //fScriptStartX = 0.f - fScriptLength /2;
    //fScriptEndX = 0.f + fScriptLength / 2;
    //if (m_strNPC_Occupation.empty() == false)
    //{
    //    iLength = m_strNPC_Occupation.size();

    //    fOccupationLength = (iLength * 15.f * 0.4f);
    //    fOccupationX = 0.f - fOccupationLength /2.f;
    //    fOccupationXEnd = 0.f + fOccupationLength / 2.f;
    //}

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom,"g_Texture",0)))
        return E_FAIL;

    CombinedMatrix = m_DialogueBoxMat;
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



    if (m_strNPC_Occupation.empty())
    {
        //Decor L End -> Decor R End

        //Decor L End
        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 1)))
            return E_FAIL;

        CombinedMatrix = m_DecorLMat[0];
        CombinedMatrix._41 = m_fLTailX;
        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            return E_FAIL;

        vUVScale.x = 0.333f;
        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &vUVScale, sizeof(_float2))))
            return E_FAIL;

        vUVPos.x = 0.05f;
        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &vUVPos, sizeof(_float2))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;

        //Decor L Mid
        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 1)))
            return E_FAIL;

        CombinedMatrix = m_DecorLMat[1];
        CombinedMatrix._11 = m_fLMid_Len;
        CombinedMatrix._41 = m_fLMidX;
        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            return E_FAIL;

        vUVScale.x = 0.08f;
        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &vUVScale, sizeof(_float2))))
            return E_FAIL;

        vUVPos.x = 0.3f;
        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &vUVPos, sizeof(_float2))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;

        //Decor R END
        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 2)))
            return E_FAIL;

        CombinedMatrix = m_DecorRMat[2];
        CombinedMatrix._41 = m_fRTailX;
        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            return E_FAIL;

        vUVScale.x = 0.333f;
        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &vUVScale, sizeof(_float2))))
            return E_FAIL;

        vUVPos.x = 0.666f;
        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &vUVPos, sizeof(_float2))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;

    }
    else
    {
        //Decor L
         //Decor L End
        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 1)))
            return E_FAIL;

        CombinedMatrix = m_DecorLMat[0];
        CombinedMatrix._41 = m_fLTailX;
        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            return E_FAIL;

        vUVScale.x = 0.333f;
        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &vUVScale, sizeof(_float2))))
            return E_FAIL;

        vUVPos.x = 0.f;
        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &vUVPos, sizeof(_float2))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;

        //Decor L Mid
        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 1)))
            return E_FAIL;

        CombinedMatrix = m_DecorLMat[1];
        CombinedMatrix._11 = m_fLMid_Len;
        CombinedMatrix._41 = m_fLMidX;
        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            return E_FAIL;

        vUVScale.x = 0.08f;
        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &vUVScale, sizeof(_float2))))
            return E_FAIL;

        vUVPos.x = 0.3f;
        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &vUVPos, sizeof(_float2))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;

        //Decor L Head
        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 1)))
            return E_FAIL;

        CombinedMatrix = m_DecorLMat[2];
        CombinedMatrix._11 = m_fLHead_Len;
        CombinedMatrix._41 = m_fLHeadX;
        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            return E_FAIL;

        vUVScale.x = 0.6133f;
        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &vUVScale, sizeof(_float2))))
            return E_FAIL;

        vUVPos.x = 0.3866f;
        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &vUVPos, sizeof(_float2))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;


        //Decor R

        //Decor R Head
        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 2)))
            return E_FAIL;

        CombinedMatrix = m_DecorRMat[0];
        CombinedMatrix._11 = m_fRHead_Len;
        CombinedMatrix._41 = m_fRHeadX;
        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            return E_FAIL;

        vUVScale.x = 0.6133f;
        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &vUVScale, sizeof(_float2))))
            return E_FAIL;

        vUVPos.x = 0.f;
        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &vUVPos, sizeof(_float2))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;

        //Decor R Mid
        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 2)))
            return E_FAIL;

        CombinedMatrix = m_DecorRMat[1];
        CombinedMatrix._11 = m_fRMid_Len;
        CombinedMatrix._41 = m_fRMidX;
        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            return E_FAIL;

        vUVScale.x = 0.07f;
        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &vUVScale, sizeof(_float2))))
            return E_FAIL;

        vUVPos.x = 0.64f;
        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &vUVPos, sizeof(_float2))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;

        //Decor R Tail
        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 2)))
            return E_FAIL;

        CombinedMatrix = m_DecorRMat[2];
        CombinedMatrix._11 = m_fRTail_Len;
        CombinedMatrix._41 = m_fRTailX;
        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            return E_FAIL;

        vUVScale.x = 0.3333f;
        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &vUVScale, sizeof(_float2))))
            return E_FAIL;

        vUVPos.x = 0.6666f;
        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &vUVPos, sizeof(_float2))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;

    }




    _int iLength;

    iLength = static_cast<_int>(m_strNPC_Name.size());
    m_pGameInstance->Render_Font(L"Font_Default", m_strNPC_Name, _float2(800.f -(iLength* 40.f *0.6f)/2, 705.f), 0.6f, Colors::Gold, 0.f, { 0.f,0.f }, false);

    iLength = static_cast<_int>(m_strNPC_Occupation.size());
    {
        m_pGameInstance->Render_Font(L"Font_Default", m_strNPC_Occupation, _float2(800.f - (iLength * 38.f * 0.4f) / 2, 735.f), 0.4f, Colors::Gold, 0.f, { 0.f,0.f }, false);
    }


    if(m_strNPC_Script.empty() == true)
    m_strNPC_Script = TEXT("텍스트를 입력해 주세요");

    iLength = static_cast<_int>(m_strNPC_Script.size());
    m_pGameInstance->Render_Font(L"Font_Default", m_strNPC_Script, _float2(800.f - (iLength * 32.f * 0.5f)/2, 760.f), 0.5f, Colors::White, 0.f, { 0.f,0.f }, false);

    return S_OK;
}

CDialogue_Page* CDialogue_Page::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDialogue_Page* pInstance = new CDialogue_Page(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CDialogue_Page"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CDialogue_Page::Clone(void* pArg)
{
    CDialogue_Page* pInstance = new CDialogue_Page(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CDialogue_Page"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDialogue_Page::Free()
{
    __super::Free();


}
