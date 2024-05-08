#include "stdafx.h"
#include "LoadingRect.h"

#include "GameInstance.h"

CLoadingRect::CLoadingRect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : COrtho(pDevice, pContext)
{
}

CLoadingRect::CLoadingRect(const CLoadingRect& rhs)
    : COrtho(rhs)
    , m_BackMatrix(rhs.m_BackMatrix)
    , m_IconMatrix(rhs.m_IconMatrix)
    , m_LBarMatrix(rhs.m_LBarMatrix)
{
}

HRESULT CLoadingRect::Add_Component(void* pArg)
{
    /* For.Com_Shader */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
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
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, L"Prototype_Component_Texture_Loading",
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLoadingRect::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLoadingRect::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    XMStoreFloat4x4(&m_BackMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_IconMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_LBarMatrix, XMMatrixIdentity());

    /* BackGround */
    m_BackMatrix._11 = static_cast<_float>(g_iWinSizeX); m_BackMatrix._22 = static_cast<_float>(g_iWinSizeY);
    m_BackMatrix._41 = 0.f; m_BackMatrix._42 = 0.f;

    m_IconMatrix._11 = 200.f; m_IconMatrix._22 = 200.f;
    m_IconMatrix._41 = 0.f; m_IconMatrix._42 = 0.f;

    m_LBarMatrix._11 = 400.f; m_LBarMatrix._22 = 54.f;
    m_LBarMatrix._41 = static_cast<_float>(820.f - g_iWinSizeX / 2); m_LBarMatrix._42 = static_cast<_float>(-(800.f - g_iWinSizeY / 2));

    return S_OK;
}

HRESULT CLoadingRect::Initialize_Clone(void* pArg)
{
    if (FAILED(Add_Component(pArg)))
        return E_FAIL;

    m_vCurrLoadRatio = { 0.f, 100.f };
    m_fRealLoadRatio = 0.f;

    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(static_cast<_float>(g_iWinSizeX), static_cast<_float>(g_iWinSizeY), 0.f, 1.f));

    return S_OK;
}

HRESULT CLoadingRect::PreviousTick(_float fTimeDelta)
{
    if (FAILED(__super::PreviousTick(fTimeDelta)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLoadingRect::Tick(_float fTimeDelta)
{
    if (FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLoadingRect::LateTick(_float fTimeDelta)
{
    if (m_fPrevRealRatio.x != m_fRealLoadRatio)
    {
        m_fLerpRatio = 0.f;
        m_fPrevRealRatio.x = m_fRealLoadRatio;
        m_fPrevRealRatio.y = m_vCurrLoadRatio.x;
    }

    m_fLerpRatio = min(m_fLerpRatio + fTimeDelta * 0.5f, 1.f);
    m_vCurrLoadRatio.x = Get_fLerp(m_fPrevRealRatio.y, m_fRealLoadRatio, m_fLerpRatio);

    if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RG_FONT, this)))
        return E_FAIL;
    
    return S_OK;
}

HRESULT CLoadingRect::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    /* BackGround */
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_BackMatrix)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(VTXTEX_ALPHA)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    /* Icon */
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_IconMatrix)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 1)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(VTXTEX_ALPHA)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    /* Loading Bar */
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor1", &m_vColor1, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor2", &m_vColor2, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVRate2", &m_vCurrLoadRatio, sizeof(_float2))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_LBarMatrix)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 3)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(VTXTEX_UVCOLORHORIZONTAL)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

CLoadingRect* CLoadingRect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLoadingRect* pInstance = new CLoadingRect(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CLoadingRect"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLoadingRect::Clone(void* pArg)
{
    CLoadingRect* pInstance = new CLoadingRect(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CLoadingRect"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLoadingRect::Free()
{
    __super::Free();
}
