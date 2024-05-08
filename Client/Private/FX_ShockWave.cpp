#include "stdafx.h"
#include "FX_ShockWave.h"

#include "DeciderObject.h"
#include "GameInstance.h"

CFX_ShockWave::CFX_ShockWave(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CAssetFX(pDevice, pContext)
{
}

CFX_ShockWave::CFX_ShockWave(const CFX_ShockWave& rhs)
    : CAssetFX(rhs)
{
}

HRESULT CFX_ShockWave::Add_Component(void* pArg)
{
    /* For.Com_Model_CyCone */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_ShockWave_Cylinder_1"),
        TEXT("Com_Model_CyCone"), (CComponent**)&m_pModelCom[CY_CONE])))
        return E_FAIL;

    /* For.Com_Model_CyCylinder */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_ShockWave_Cylinder_2"),
        TEXT("Com_Model_CyCylinder"), (CComponent**)&m_pModelCom[CY_CYLINDER])))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_MeshFX"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ShockWave"),
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    /* For.Com_Renderer */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
        TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
        return E_FAIL;

    /* For.Com_Transform_CyCone */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform_CyCone"), (CComponent**)&m_pTransformCom[CY_CONE])))
        return E_FAIL;

    /* For.Com_Transform_CyCylinder */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform_CyCylinder"), (CComponent**)&m_pTransformCom[CY_CYLINDER])))
        return E_FAIL;

    return S_OK;
}

HRESULT CFX_ShockWave::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResources(m_pShaderCom, "g_EffectTextures")))
        return E_FAIL;

    _float4	vUVRate = { m_fFrame, 0.f, 0.f, 0.f };
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVRate", &vUVRate, sizeof(_float4))))
        return E_FAIL;

    return S_OK;
}

HRESULT CFX_ShockWave::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CFX_ShockWave::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component(pArg)))
        return E_FAIL;

    SW_DESC* pSWDesc = static_cast<SW_DESC*>(pArg);

    m_fScaleFactor = pSWDesc->fScale;

    m_pTransformCom[CY_CONE]->Set_State(CTransform::STATE_POSITION, pSWDesc->vInitPosition);
    m_pTransformCom[CY_CONE]->Set_Scale(XMLoadFloat4(&m_vConeScale) * m_fScale);

    m_pTransformCom[CY_CYLINDER]->Set_State(CTransform::STATE_POSITION, pSWDesc->vInitPosition);
    m_pTransformCom[CY_CYLINDER]->Set_Scale(XMLoadFloat4(&m_vCylinderScale) * m_fScale);

    return S_OK;
}

HRESULT CFX_ShockWave::PreviousTick(_float fTimeDelta)
{
    if (CDeciderObject::Get_RenderMode() != CDeciderObject::RMODE_UI)
    {
        if (FAILED(__super::PreviousTick(fTimeDelta)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CFX_ShockWave::Tick(_float fTimeDelta)
{
    if (CDeciderObject::Get_RenderMode() != CDeciderObject::RMODE_UI)
    {
        /* FX Frame */
        m_fFrame += fTimeDelta * 2.f;
        m_fScale += fTimeDelta * 2.f;
        m_fFrame = fmod(m_fFrame, 1.f);

        /* FX Scale */
        m_pTransformCom[CY_CONE]->Set_Scale(XMLoadFloat4(&m_vConeScale) * m_fScale * m_fScaleFactor);
        m_pTransformCom[CY_CYLINDER]->Set_Scale(XMLoadFloat4(&m_vCylinderScale) * m_fScale * m_fScaleFactor);
    }

    return S_OK;
}

HRESULT CFX_ShockWave::LateTick(_float fTimeDelta)
{
    m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);

    return S_OK;
}

HRESULT CFX_ShockWave::Render()
{
    if (FAILED(CFX_ShockWave::Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = { };

    /* CyCone */
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom[CY_CONE]->Get_WorldFloat4x4())))
        return E_FAIL;

    iNumMeshes = m_pModelCom[CY_CONE]->Get_NumMeshes();
    for (_uint siMesh = 0; siMesh < iNumMeshes; siMesh++)
    {
        if (FAILED(m_pShaderCom->Begin(MESHFX_SHOCKWAVE)))
            return E_FAIL;

        if (FAILED(m_pModelCom[CY_CONE]->Render(siMesh)))
            return E_FAIL;
    }

    /* CyCylinder */
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom[CY_CYLINDER]->Get_WorldFloat4x4())))
        return E_FAIL;

    iNumMeshes = m_pModelCom[CY_CYLINDER]->Get_NumMeshes();
    for (_uint siMesh = 0; siMesh < iNumMeshes; siMesh++)
    {
        if (FAILED(m_pShaderCom->Begin(MESHFX_SHOCKWAVE)))
            return E_FAIL;

        if (FAILED(m_pModelCom[CY_CYLINDER]->Render(siMesh)))
            return E_FAIL;
    }

    return S_OK;
}

CFX_ShockWave* CFX_ShockWave::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CFX_ShockWave* pInstance = new CFX_ShockWave(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CFX_ShockWave"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CFX_ShockWave::Clone(void* pArg)
{
    CFX_ShockWave* pInstance = new CFX_ShockWave(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CFX_ShockWave"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFX_ShockWave::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom[CY_CONE]);
    Safe_Release(m_pModelCom[CY_CYLINDER]);
    
    Safe_Release(m_pTransformCom[CY_CONE]);
    Safe_Release(m_pTransformCom[CY_CYLINDER]);

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pRendererCom);
}