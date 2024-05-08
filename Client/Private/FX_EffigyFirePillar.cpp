#include "stdafx.h"
#include "FX_EffigyFirePillar.h"

#include "DeciderObject.h"
#include "GameInstance.h"

CFX_EffigyFirePillar::CFX_EffigyFirePillar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CAssetFX(pDevice, pContext)
{
}

CFX_EffigyFirePillar::CFX_EffigyFirePillar(const CFX_EffigyFirePillar& rhs)
    : CAssetFX(rhs)
{
}

HRESULT CFX_EffigyFirePillar::Add_Component(void* pArg)
{
    /* For.Com_Model_Cone */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_EffigyFire_Cone"),
        TEXT("Com_Model_Cone"), (CComponent**)&m_pModelCom[FP_CONE])))
        return E_FAIL;

    /* For.Com_Model_Cylinder */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_EffigyFire_Clinder"),
        TEXT("Com_Model_Cylinder"), (CComponent**)&m_pModelCom[FP_CYLINDER])))
        return E_FAIL;

    /* For.Com_Model_Circle */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_EffigyFire_Circle"),
        TEXT("Com_Model_Circle"), (CComponent**)&m_pModelCom[FP_CRACK])))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_MeshFX"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EffigyFire_Pillar"),
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    /* For.Com_Renderer */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
        TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
        return E_FAIL;

    /* For.Com_Transform_Cone */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform_Cone"), (CComponent**)&m_pTransformCom[FP_CONE])))
        return E_FAIL;

    /* For.Com_Transform_Cylinder */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform_Cylinder"), (CComponent**)&m_pTransformCom[FP_CYLINDER])))
        return E_FAIL;

    /* For.Com_Transform_Crack */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform_Crack"), (CComponent**)&m_pTransformCom[FP_CRACK])))
        return E_FAIL;

    return S_OK;
}

HRESULT CFX_EffigyFirePillar::Bind_ShaderResources()
{
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

HRESULT CFX_EffigyFirePillar::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CFX_EffigyFirePillar::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component(pArg)))
        return E_FAIL;

    ASFX_DESC* pAsfxDesc = static_cast<ASFX_DESC*>(pArg);
    if (pAsfxDesc->ePosType == ASFX_DESC::POS_RELATIVE)
        return E_FAIL;

    _vector vPosition = pAsfxDesc->vInitPosition + _vector{ 0.f, 0.1f, 0.f, 0.f };

    m_pTransformCom[FP_CONE]->Set_State(CTransform::STATE_POSITION, vPosition);
    m_pTransformCom[FP_CONE]->Set_Scale(XMLoadFloat4(&m_vConeScale));

    m_pTransformCom[FP_CYLINDER]->Set_State(CTransform::STATE_POSITION, vPosition);
    m_pTransformCom[FP_CYLINDER]->Set_Scale(XMLoadFloat4(&m_vCylinderScale));

    m_pTransformCom[FP_CRACK]->Set_State(CTransform::STATE_POSITION, vPosition);
    m_pTransformCom[FP_CRACK]->Set_Scale(XMLoadFloat4(&m_vCircleScale));

    m_eStep = STEP_1;

    return S_OK;
}

HRESULT CFX_EffigyFirePillar::PreviousTick(_float fTimeDelta)
{
    if (CDeciderObject::Get_RenderMode() != CDeciderObject::RMODE_UI)
    {
        if (FAILED(__super::PreviousTick(fTimeDelta)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CFX_EffigyFirePillar::Tick(_float fTimeDelta)
{
    if (CDeciderObject::Get_RenderMode() != CDeciderObject::RMODE_UI)
    {
        /* FX Frame */
        m_vFrame.x += fTimeDelta; /* Acc Frame */
        m_vFrame.y += fTimeDelta * 2.f; /* Scale */
        m_vFrame.z = fmod(m_vFrame.x, 1.f);

        if (m_vFrame.x < 1.5f)
        {
            m_vFrame.w = m_vFrame.x / 1.5f;
        }
        else
        {
            m_vFrame.w = 1.f - (m_vFrame.x - 1.5f) / 0.5f;
        }

        /* Invalidate */
        if (m_eStep == STEP_1)
        {
            if (m_vFrame.x > 0.5f)
            {
                m_eStep = STEP_2;
                m_vFrame.y = 0.f;
                m_pTransformCom[FP_CRACK]->Set_Scale(XMLoadFloat4(&m_vCircleScale) * 2.f);
            }
            else
            {
            }
        }
        else if (m_eStep == STEP_2)
        {
            if (m_vFrame.x > 1.f)
            {
                m_eStep = STEP_3;
                m_vFrame.y = 0.f;
            }
            else
            {

            }
        }
        else if (m_eStep == STEP_3)
        {
            if (m_vFrame.x > 1.5f)
            {
                m_eStep = STEP_4;
                m_vFrame.y = 0.f;
            }
            else
            {
                /* FX Scale */
                m_pTransformCom[FP_CYLINDER]->Set_Scale(XMLoadFloat4(&m_vCylinderScale) * m_vFrame.y);
            }
        }
        else if (m_eStep == STEP_4)
        {
            if (m_vFrame.x > 2.f)
            {
                m_bDuration = TRUE;
                m_fDuration = -1.f;
            }
            else
            {

            }
        }
    }

    return S_OK;
}

HRESULT CFX_EffigyFirePillar::LateTick(_float fTimeDelta)
{
    m_pRendererCom->Add_RenderGroup(CRenderer::RG_BLEND, this);

    return S_OK;
}

HRESULT CFX_EffigyFirePillar::Render()
{
    static _int iIndex_1 = 0;
    static _int iIndex_2 = 1;
    static _int iIndex_3 = 2;
    static _int iIndex_4 = 3;

    if (FAILED(CFX_EffigyFirePillar::Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = { };

    switch (m_eStep)
    {
    case STEP_1:
        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom[FP_CONE]->Get_WorldFloat4x4())))
            return E_FAIL;

        iNumMeshes = m_pModelCom[FP_CONE]->Get_NumMeshes();
        for (_uint siMesh = 0; siMesh < iNumMeshes; siMesh++)
        {
            if (FAILED(m_pShaderCom->Begin(MESHFX_EFFIGYFIRE_EXTENT)))
                return E_FAIL;

            if (FAILED(m_pModelCom[FP_CONE]->Render(siMesh)))
                return E_FAIL;
        }

        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom[FP_CRACK]->Get_WorldFloat4x4())))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_iIndex", &iIndex_1, sizeof(_int))))
            return E_FAIL;

        iNumMeshes = m_pModelCom[FP_CRACK]->Get_NumMeshes();
        for (_uint siMesh = 0; siMesh < iNumMeshes; siMesh++)
        {
            if (FAILED(m_pShaderCom->Begin(MESHFX_EFFIGYFIRE_PILLAR_CRACK)))
                return E_FAIL;

            if (FAILED(m_pModelCom[FP_CRACK]->Render(siMesh)))
                return E_FAIL;
        }
        break;

    case STEP_2:
        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom[FP_CONE]->Get_WorldFloat4x4())))
            return E_FAIL;

        iNumMeshes = m_pModelCom[FP_CONE]->Get_NumMeshes();
        for (_uint siMesh = 0; siMesh < iNumMeshes; siMesh++)
        {
            if (FAILED(m_pShaderCom->Begin(MESHFX_EFFIGYFIRE_EXTENT)))
                return E_FAIL;

            if (FAILED(m_pModelCom[FP_CONE]->Render(siMesh)))
                return E_FAIL;
        }

        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom[FP_CRACK]->Get_WorldFloat4x4())))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_iIndex", &iIndex_2, sizeof(_int))))
            return E_FAIL;

        iNumMeshes = m_pModelCom[FP_CRACK]->Get_NumMeshes();
        for (_uint siMesh = 0; siMesh < iNumMeshes; siMesh++)
        {
            if (FAILED(m_pShaderCom->Begin(MESHFX_EFFIGYFIRE_PILLAR_CRACK)))
                return E_FAIL;

            if (FAILED(m_pModelCom[FP_CRACK]->Render(siMesh)))
                return E_FAIL;
        }
        break;

    case STEP_3:
        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom[FP_CONE]->Get_WorldFloat4x4())))
            return E_FAIL;

        iNumMeshes = m_pModelCom[FP_CONE]->Get_NumMeshes();
        for (_uint siMesh = 0; siMesh < iNumMeshes; siMesh++)
        {
            if (FAILED(m_pShaderCom->Begin(MESHFX_EFFIGYFIRE_EXTENT)))
                return E_FAIL;

            if (FAILED(m_pModelCom[FP_CONE]->Render(siMesh)))
                return E_FAIL;
        }

        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom[FP_CRACK]->Get_WorldFloat4x4())))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_iIndex", &iIndex_3, sizeof(_int))))
            return E_FAIL;

        iNumMeshes = m_pModelCom[FP_CRACK]->Get_NumMeshes();
        for (_uint siMesh = 0; siMesh < iNumMeshes; siMesh++)
        {
            if (FAILED(m_pShaderCom->Begin(MESHFX_EFFIGYFIRE_PILLAR_CRACK)))
                return E_FAIL;
        
            if (FAILED(m_pModelCom[FP_CRACK]->Render(siMesh)))
                return E_FAIL;
        }

        if (m_vFrame.y > 0.f)
        {
            if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom[FP_CYLINDER]->Get_WorldFloat4x4())))
                return E_FAIL;

            iNumMeshes = m_pModelCom[FP_CYLINDER]->Get_NumMeshes();
            for (_uint siMesh = 0; siMesh < iNumMeshes; siMesh++)
            {
                if (FAILED(m_pShaderCom->Begin(MESHFX_EFFIGYFIRE_EXTENT)))
                    return E_FAIL;

                if (FAILED(m_pModelCom[FP_CYLINDER]->Render(siMesh)))
                    return E_FAIL;
            }
        }
        break;

    case STEP_4:
        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom[FP_CRACK]->Get_WorldFloat4x4())))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_iIndex", &iIndex_3, sizeof(_int))))
            return E_FAIL;

        iNumMeshes = m_pModelCom[FP_CRACK]->Get_NumMeshes();
        for (_uint siMesh = 0; siMesh < iNumMeshes; siMesh++)
        {
            if (FAILED(m_pShaderCom->Begin(MESHFX_EFFIGYFIRE_PILLAR_CRACK)))
                return E_FAIL;

            if (FAILED(m_pModelCom[FP_CRACK]->Render(siMesh)))
                return E_FAIL;
        }
        break;
    }

    return S_OK;
}

CFX_EffigyFirePillar* CFX_EffigyFirePillar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CFX_EffigyFirePillar* pInstance = new CFX_EffigyFirePillar(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CFX_EffigyFirePillar"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CFX_EffigyFirePillar::Clone(void* pArg)
{
    CFX_EffigyFirePillar* pInstance = new CFX_EffigyFirePillar(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CFX_EffigyFirePillar"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFX_EffigyFirePillar::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom[FP_CONE]);
    Safe_Release(m_pModelCom[FP_CYLINDER]);
    Safe_Release(m_pModelCom[FP_CRACK]);
    
    Safe_Release(m_pTransformCom[FP_CONE]);
    Safe_Release(m_pTransformCom[FP_CYLINDER]);
    Safe_Release(m_pTransformCom[FP_CRACK]);

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pRendererCom);
}