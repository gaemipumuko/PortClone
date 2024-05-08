#include "stdafx.h"
#include "Mouse.h"

#include "GameInstance.h"

CMouse::CMouse(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : COrtho(pDevice, pContext)
{
}

CMouse::CMouse(const CMouse& rhs)
    : COrtho(rhs)
{
}

HRESULT CMouse::Add_Component(void* pArg)
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
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, L"Prototype_Component_Texture_Cursor",
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CMouse::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderCurrentTexture(m_pShaderCom, "g_Texture")))
        return E_FAIL;

    return S_OK;
}

HRESULT CMouse::Update_CursorPos()
{
    POINT		ptMouse;
    GetCursorPos(&ptMouse);
    ScreenToClient(g_hWnd, &ptMouse);

    COrtho::Update_Matrix(
        static_cast<_float>(ptMouse.x) - g_iWinSizeX * 0.5f + (g_iMouseSizeX * 0.5f),
        -static_cast<_float>(ptMouse.y) + g_iWinSizeY * 0.5f - (g_iMouseSizeY * 0.5f),
        g_iMouseSizeX, g_iMouseSizeY
    );

    return S_OK;
}

HRESULT CMouse::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMouse::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component(pArg)))
        return E_FAIL;

    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(static_cast<_float>(g_iWinSizeX), static_cast<_float>(g_iWinSizeY), 0.f, 1.f));

    ShowCursor(FALSE);
    m_pGameInstance->Fix_Mouse();

    return S_OK;
}

HRESULT CMouse::Tick(_float fTimeDelta)
{
    if (FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;

    if (FAILED(CMouse::Update_CursorPos()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMouse::LateTick(_float fTimeDelta)
{
    if (m_pGameInstance->Get_ClipCount() > 0)
        m_pRendererCom->Add_RenderGroup(CRenderer::RG_MOUSE, this);

    return S_OK;
}

HRESULT CMouse::Render()
{
    /* Shader에 값 전달 */
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    /* Shader */
    if (FAILED(m_pShaderCom->Begin(VTXTEX_ALPHA)))
        return E_FAIL;

    /* VIBuffer */
    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

CMouse* CMouse::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMouse* pInstance = new CMouse(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CMouse"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMouse::Clone(void* pArg)
{
    CMouse* pInstance = new CMouse(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CMouse"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMouse::Free()
{
    __super::Free();
}
