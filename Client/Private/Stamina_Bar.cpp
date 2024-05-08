#include "stdafx.h"
#include "Stamina_Bar.h"
// #include "CUI_Manager.h"

#include "GameInstance.h"

CStamina_Bar::CStamina_Bar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : COrtho(pDevice, pContext)
{
}

CStamina_Bar::CStamina_Bar(const CStamina_Bar& rhs)
    : COrtho(rhs)
{
}

HRESULT CStamina_Bar::Add_Component(void* pArg)
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
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Stamina_Bar"),
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CStamina_Bar::Set_Texture_Color(_vector vColor)
{
    m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(vColor));

    return S_OK;
}

HRESULT CStamina_Bar::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CStamina_Bar::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component(pArg)))
        return E_FAIL;

    COrtho::Initialize_Matrix();

    m_eRenderType = RTYPE_GAMEPLAY;

    return S_OK;
}

HRESULT CStamina_Bar::PreviousTick(_float fTimeDelta)
{
    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {
        if (FAILED(__super::PreviousTick(fTimeDelta)))
            return E_FAIL;
    }
    return S_OK;
}

HRESULT CStamina_Bar::Tick(_float fTimeDelta)
{
    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {
        if (FAILED(__super::Tick(fTimeDelta)))
            return E_FAIL;

        m_pStamina_Bar->Tick(fTimeDelta);
    }
    return S_OK;
}

HRESULT CStamina_Bar::LateTick(_float fTimeDelta)
{
    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {
        if (FAILED(__super::LateTick(fTimeDelta)))
            return E_FAIL;

        m_pStamina_Bar->LateTick(fTimeDelta);
    }
    return S_OK;
}

HRESULT CStamina_Bar::Render()
{

        if (FAILED(Bind_ShaderResources()))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &m_vUVScale, sizeof(_float2))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &m_vUVPosition, sizeof(_float2))))
            return E_FAIL;

        if (FAILED(Set_Texture_Color(m_vColor)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_STAMINA)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;


        // at 1.f -> 40px

    return S_OK;
}

CStamina_Bar* CStamina_Bar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CStamina_Bar* pInstance = new CStamina_Bar(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CStamina_Bar"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CStamina_Bar::Clone(void* pArg)
{
    CStamina_Bar* pInstance = new CStamina_Bar(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CStamina_Bar"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CStamina_Bar::Free()
{
    __super::Free();
}
