#include "stdafx.h"
#include "UI_DualTex.h"
// #include "CUI_Manager.h"

#include "GameInstance.h"

CUI_DualTex::CUI_DualTex(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : COrtho(pDevice, pContext)
{
}

CUI_DualTex::CUI_DualTex(const CUI_DualTex& rhs)
    : COrtho(rhs)
{
}

HRESULT CUI_DualTex::Add_Component(void* pArg)
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
    if (FAILED(CGameObject::Add_Component(m_tOrthoDesc.eTextureLevel, m_tOrthoDesc.strTextureTag,
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    /* For.Com_Texture_Diff */
    if (FAILED(CGameObject::Add_Component(m_tOrthoDesc.eTextureLevel, strDiffTexTag,
        TEXT("Com_Texture_Diff"), (CComponent**)&m_pDiffTextureCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_DualTex::Bind_ShaderResources()
{


    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
        return E_FAIL;

    //if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &m_vUVScale, sizeof(_float2))))
    //    return E_FAIL;

    //if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &m_vUVPosition, sizeof(_float2))))
    //    return E_FAIL;

    return S_OK;
}

HRESULT CUI_DualTex::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_DualTex::Initialize_Clone(void* pArg)
{
    if (pArg != nullptr)
    {
        DUALTEX_DESC* pDesc = static_cast<DUALTEX_DESC*>(pArg);

        m_tOrthoDesc.eTextureLevel = pDesc->eTextureLevel;
        m_tOrthoDesc.strTextureTag = pDesc->strTextureTag;
        m_tOrthoDesc.fPosition = pDesc->fPosition;
        m_tOrthoDesc.fSize = pDesc->fSize;
        m_tOrthoDesc.ePage = pDesc->ePage;
        m_tOrthoDesc.iDepth = pDesc->iDepth;

        m_vDimentionOrigin.x = pDesc->fPosition.x;
        m_vDimentionOrigin.y = pDesc->fPosition.y;
        m_vDimentionOrigin.z = pDesc->fSize.x;
        m_vDimentionOrigin.w = pDesc->fSize.y;

        strDiffTexTag = pDesc->strDiffTextureTag;

        if (m_tOrthoDesc.ePage != PAGE_END)
        {
            // CUI_Manager::Get_Instance()->Add_UI_Object(m_tOrthoDesc.ePage, this);
            // 
            // if (m_tOrthoDesc.ePage == PAGE_MAIN || m_tOrthoDesc.ePage == CUI_Manager::Get_Instance()->Get_Current_Page())
            // {
            //     m_bWindowTrue = true;
            // }

        }

    }

    if (FAILED(Add_Component(pArg)))
        return E_FAIL;

    COrtho::Initialize_Matrix();

    m_eRenderType = RTYPE_UI;

    return S_OK;
}

HRESULT CUI_DualTex::PreviousTick(_float fTimeDelta)
{
    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {
        if (FAILED(__super::PreviousTick(fTimeDelta)))
            return E_FAIL;
    }
    return S_OK;
}

HRESULT CUI_DualTex::Tick(_float fTimeDelta)
{
    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {
        if (FAILED(__super::Tick(fTimeDelta)))
            return E_FAIL;
    }
    return S_OK;
}

HRESULT CUI_DualTex::LateTick(_float fTimeDelta)
{
    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {
        if (FAILED(__super::LateTick(fTimeDelta)))
            return E_FAIL;
    }
    return S_OK;
}

HRESULT CUI_DualTex::Render()
{

        if (FAILED(Bind_ShaderResources()))
            return E_FAIL;


        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture",1)))
            return E_FAIL;

        _float4x4 vScaledMat = m_pTransformCom->Get_WorldFloat4x4();
        vScaledMat._11 *= 240.f/160.f;
        vScaledMat._22 *= 780.f/520.f;
        //250 785

        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", vScaledMat)))
            return E_FAIL;

        /*
    Fire
    {1.f,0.65882352941.f,0.43921568627.f,1.f}

    Frost
    { 0.80392156862f,1.f,1.f,1.f}

    Wind
    {0.65098039215f,95686274509.f,0.8f,1.f}

    Lightning
    {0.87058823529f,0.7294117647 ,1.f,1.f}

    Earth
    {244,214,96}

    Water
    {8, 288, 255}

    Nature
    {177,235,41}

    */

        if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vRadianceColor, sizeof(_vector))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_RED_REF)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;


        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
            return E_FAIL;

     
        if (FAILED(m_pDiffTextureCom->Bind_ShaderCurrentTexture(m_pShaderCom, "g_BrushTexture")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_DUALTEX)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;


    return S_OK;
}

void CUI_DualTex::SetTextureIndex(_uint iTextureIndex)
{
    m_pDiffTextureCom->Set_CurrentTexture(static_cast<_float>(iTextureIndex));

}

CUI_DualTex* CUI_DualTex::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_DualTex* pInstance = new CUI_DualTex(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CUI_DualTex"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_DualTex::Clone(void* pArg)
{
    CUI_DualTex* pInstance = new CUI_DualTex(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CUI_DualTex"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_DualTex::Free()
{
    __super::Free();

    Safe_Release(m_pDiffTextureCom);

}
