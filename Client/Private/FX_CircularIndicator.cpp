#include "stdafx.h"
#include "FX_CircularIndicator.h"

#include "DeciderObject.h"
#include "GameInstance.h"

#define DEFAULT_MESH 0

CFX_CircularIndicator::CFX_CircularIndicator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CAssetFX(pDevice, pContext)
{
}

CFX_CircularIndicator::CFX_CircularIndicator(const CFX_CircularIndicator& rhs)
    : CAssetFX(rhs)
{
}

HRESULT CFX_CircularIndicator::Add_Component(void* pArg)
{
    /* For.Com_Model_Gage */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_CircularIndicator_Gage"),
        TEXT("Com_Model_Gage"), (CComponent**)&m_pModelCom[IND_GAGE])))
        return E_FAIL;

    /* For.Com_Model_Border */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_CircularIndicator_Border"),
        TEXT("Com_Model_Border"), (CComponent**)&m_pModelCom[IND_BORDER])))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_MeshFX"),
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

    return S_OK;
}

HRESULT CFX_CircularIndicator::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    _float4	vUVRate = { m_fFrame, 0.f, 0.f, 0.f };
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVRate", &vUVRate, sizeof(_float4))))
        return E_FAIL;

    return S_OK;
}

HRESULT CFX_CircularIndicator::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CFX_CircularIndicator::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component(pArg)))
        return E_FAIL;

    CIND_DESC* pIndicatorDesc = static_cast<CIND_DESC*>(pArg);

    m_pMeteoriteTransformCom = pIndicatorDesc->pTransform;
    _float fScale = pIndicatorDesc->fScale;
    Safe_AddRef(m_pMeteoriteTransformCom);

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, pIndicatorDesc->vInitPosition);

    if (fScale != 0.f)
        m_pTransformCom->Set_Scale({ 0.04f, 0.04f, 0.04f, 0.f });
    else
        m_pTransformCom->Set_Scale({ 0.02f * fScale, 0.02f * fScale, 0.02f * fScale, 0.f });

    m_fDistance = pIndicatorDesc->fDistance;

    return S_OK;
}

HRESULT CFX_CircularIndicator::PreviousTick(_float fTimeDelta)
{
    if (CDeciderObject::Get_RenderMode() != CDeciderObject::RMODE_UI)
    {
        if (FAILED(__super::PreviousTick(fTimeDelta)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CFX_CircularIndicator::Tick(_float fTimeDelta)
{
    return S_OK;
}

HRESULT CFX_CircularIndicator::LateTick(_float fTimeDelta)
{
    m_pRendererCom->Add_RenderGroup(CRenderer::RG_BLEND, this);

    return S_OK;
}

HRESULT CFX_CircularIndicator::Render()
{
    if (FAILED(CFX_CircularIndicator::Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", Colors::Red, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(MESHFX_CIRCULARINDIBORDER)))
        return E_FAIL;
    
    if (FAILED(m_pModelCom[IND_BORDER]->Render(DEFAULT_MESH)))
        return E_FAIL;

    _float fRate = 
        1.f - XMVectorGetX(XMVector4Length(
            m_pMeteoriteTransformCom->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION))) / m_fDistance;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_ColorRate", &fRate, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(MESHFX_CIRCULARINDIGAGE)))
        return E_FAIL;

    if (FAILED(m_pModelCom[IND_GAGE]->Render(DEFAULT_MESH)))
        return E_FAIL;

    return S_OK;
}

CFX_CircularIndicator* CFX_CircularIndicator::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CFX_CircularIndicator* pInstance = new CFX_CircularIndicator(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CFX_CircularIndicator"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CFX_CircularIndicator::Clone(void* pArg)
{
    CFX_CircularIndicator* pInstance = new CFX_CircularIndicator(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CFX_CircularIndicator"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFX_CircularIndicator::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom[IND_GAGE]);
    Safe_Release(m_pModelCom[IND_BORDER]);

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pRendererCom);
    Safe_Release(m_pTransformCom);
    Safe_Release(m_pMeteoriteTransformCom);
}