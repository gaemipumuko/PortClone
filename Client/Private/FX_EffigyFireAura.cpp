#include "stdafx.h"
#include "FX_EffigyFireAura.h"

#include "DeciderObject.h"
#include "GameInstance.h"

CFX_EffigyFireAura::CFX_EffigyFireAura(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CAssetFX(pDevice, pContext)
{
}

CFX_EffigyFireAura::CFX_EffigyFireAura(const CFX_EffigyFireAura& rhs)
    : CAssetFX(rhs)
{
}

HRESULT CFX_EffigyFireAura::Add_Component(void* pArg)
{
    /* For.Com_Model_EffigyFire_Torus_0 */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_EffigyFire_Torus_0"),
        TEXT("Com_Model_EffigyFire_Torus_0"), (CComponent**)&m_pModelCom[AURA_0])))
        return E_FAIL;

    /* For.Com_Model_EffigyFire_Torus_45 */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_EffigyFire_Torus_45"),
        TEXT("Com_Model_EffigyFire_Torus_45"), (CComponent**)&m_pModelCom[AURA_45])))
        return E_FAIL;

    /* For.Com_Model_EffigyFire_Torus_90 */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_EffigyFire_Torus_90"),
        TEXT("Com_Model_EffigyFire_Torus_90"), (CComponent**)&m_pModelCom[AURA_90])))
        return E_FAIL;

    /* For.Com_Model_EffigyFire_Torus_135 */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_EffigyFire_Torus_135"),
        TEXT("Com_Model_EffigyFire_Torus_135"), (CComponent**)&m_pModelCom[AURA_135])))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_MeshFX"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EffigyFire_Aura"),
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    /* For.Com_Renderer */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
        TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
        return E_FAIL;

    /* For.Com_Transform */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CFX_EffigyFireAura::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResources(m_pShaderCom, "g_EffectTextures")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVRate", &m_vFrame, sizeof(_float4))))
        return E_FAIL;

    return S_OK;
}

HRESULT CFX_EffigyFireAura::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CFX_EffigyFireAura::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component(pArg)))
        return E_FAIL;

    ASFX_DESC* pAsfxDesc = static_cast<ASFX_DESC*>(pArg);
    if (pAsfxDesc->ePosType == ASFX_DESC::POS_FIX)
        return E_FAIL;

    m_pTargetTransformCom = pAsfxDesc->pTransform;
    Safe_AddRef(m_pTargetTransformCom);

    return S_OK;
}
    
HRESULT CFX_EffigyFireAura::PreviousTick(_float fTimeDelta)
{
    if (CDeciderObject::Get_RenderMode() != CDeciderObject::RMODE_UI)
    {
        if (FAILED(__super::PreviousTick(fTimeDelta)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CFX_EffigyFireAura::Tick(_float fTimeDelta)
{
    if (CDeciderObject::Get_RenderMode() != CDeciderObject::RMODE_UI)
    {
        /* FX Frame */
        m_vFrame.x += fTimeDelta;
        m_vFrame.x = fmod(m_vFrame.x, 1.f);

        /* Invalidate Transform */
        m_pTransformCom->Set_WorldMatrix(m_pTargetTransformCom->Get_WorldMatrix());
        m_pTransformCom->Set_Scale({ 0.015f, 0.015f, 0.015f, 0.f });
    }

    return S_OK;
}

HRESULT CFX_EffigyFireAura::LateTick(_float fTimeDelta)
{
    m_pRendererCom->Add_RenderGroup(CRenderer::RG_BLEND, this);

    return S_OK;
}

HRESULT CFX_EffigyFireAura::Render()
{
    if (FAILED(CFX_EffigyFireAura::Bind_ShaderResources()))
        return E_FAIL;

    for (_int iIndex = 0; iIndex < static_cast<_int>(AURA_END); ++iIndex)
    {
        _uint iNumMeshes = m_pModelCom[iIndex]->Get_NumMeshes();
        for (_uint siMesh = 0; siMesh < iNumMeshes; siMesh++)
        {
            if (FAILED(m_pShaderCom->Begin(MESHFX_EFFIGYFIRE_AURA)))
                return E_FAIL;

            if (FAILED(m_pModelCom[iIndex]->Render(siMesh)))
                return E_FAIL;
        }
    }

    return S_OK;
}

CFX_EffigyFireAura* CFX_EffigyFireAura::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CFX_EffigyFireAura* pInstance = new CFX_EffigyFireAura(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CFX_EffigyFireAura"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CFX_EffigyFireAura::Clone(void* pArg)
{
    CFX_EffigyFireAura* pInstance = new CFX_EffigyFireAura(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CFX_EffigyFireAura"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFX_EffigyFireAura::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom[AURA_0]);
    Safe_Release(m_pModelCom[AURA_45]);
    Safe_Release(m_pModelCom[AURA_90]);
    Safe_Release(m_pModelCom[AURA_135]);

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pRendererCom);
    Safe_Release(m_pTransformCom);
    Safe_Release(m_pTargetTransformCom);
}