#include "stdafx.h"
#include "FX_RaidenES.h"
#include "FX_EffigyFireExplode.h"

#include "DeciderObject.h"
#include "GameInstance.h"

CFX_RaidenES::CFX_RaidenES(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CAssetFX(pDevice, pContext)
{
}

CFX_RaidenES::CFX_RaidenES(const CFX_RaidenES& rhs)
    : CAssetFX(rhs)
{
}

HRESULT CFX_RaidenES::Add_Component(void* pArg)
{
    /* For.Com_Model */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Raiden_ES"),
        TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
        return E_FAIL;

    /* For.Com_Shader_Line */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
        TEXT("Com_Shader_Line"), (CComponent**)&m_pShaderCom[RES_LINE])))
        return E_FAIL;

    /* For.Com_Shader_Eye */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader_Eye"), (CComponent**)&m_pShaderCom[RES_EYE])))
        return E_FAIL;

    /* For.Com_Renderer */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
        TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
        return E_FAIL;

    /* For.Com_Transform_Line */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform_Line"), (CComponent**)&m_pTransformCom[RES_LINE])))
        return E_FAIL;

    /* For.Com_Transform_Eye */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform_Eye"), (CComponent**)&m_pTransformCom[RES_EYE])))
        return E_FAIL;

    /* For.Com_VIBuffer */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CFX_RaidenES::Bind_ShaderResources()
{
    /* Line */
    if (FAILED(m_pShaderCom[RES_LINE]->Bind_Matrix("g_WorldMatrix", m_pTransformCom[RES_LINE]->Get_WorldFloat4x4())))
        return E_FAIL;

    if (FAILED(m_pShaderCom[RES_LINE]->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom[RES_LINE]->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pShaderCom[RES_LINE]->Bind_RawValue("g_vUVRate", &m_vFrame, sizeof(_float4))))
        return E_FAIL;

    /* Eye */
    if (FAILED(m_pShaderCom[RES_EYE]->Bind_Matrix("g_WorldMatrix", m_pTransformCom[RES_EYE]->Get_WorldFloat4x4())))
        return E_FAIL;

    if (FAILED(m_pShaderCom[RES_EYE]->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom[RES_EYE]->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pShaderCom[RES_EYE]->Bind_RawValue("g_vUVRate", &m_vFrame, sizeof(_float4))))
        return E_FAIL;

    return S_OK;
}

HRESULT CFX_RaidenES::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CFX_RaidenES::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component(pArg)))
        return E_FAIL;

    RAIDENES_DESC* pRaidenESDesc = static_cast<RAIDENES_DESC*>(pArg);

    _vector vPosition   = pRaidenESDesc->vInitPosition;
    vPosition           = vPosition + XMVector3Normalize(XMLoadFloat4(&pRaidenESDesc->vLookVector)) * 2.f;

    m_pTransformCom[RES_LINE]->Set_Scale({ 10.f, 0.5f, 1.f, 0.f });
    m_pTransformCom[RES_LINE]->Set_State(CTransform::STATE_POSITION, vPosition);
    m_pTransformCom[RES_LINE]->Look_At_Position(pRaidenESDesc->vInitPosition);

    m_pTransformCom[RES_LINE]->Set_State(CTransform::STATE_POSITION, vPosition + _vector{0.f, 1.25f, 0.f, 0.f});

    m_pTransformCom[RES_EYE]->Set_Scale({ 0.1f, 0.1f, 0.1f, 0.f });
    m_pTransformCom[RES_EYE]->Set_State(CTransform::STATE_POSITION, vPosition);
    m_pTransformCom[RES_EYE]->Look_At_Position(pRaidenESDesc->vInitPosition);

    m_pTransformCom[RES_EYE]->Set_State(CTransform::STATE_POSITION, vPosition + _vector{ 0.f, 1.25f, 0.f, 0.f });

    return S_OK;
}

HRESULT CFX_RaidenES::PreviousTick(_float fTimeDelta)
{
    if (CDeciderObject::Get_RenderMode() != CDeciderObject::RMODE_UI)
    {
        if (FAILED(__super::PreviousTick(fTimeDelta)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CFX_RaidenES::Tick(_float fTimeDelta)
{
    if (CDeciderObject::Get_RenderMode() != CDeciderObject::RMODE_UI)
    {
        m_vFrame.x += fTimeDelta;

        m_pTransformCom[RES_LINE]->Set_Scale({10.f, max(((0.5f - m_vFrame.x) / 0.5f), 0.01f), 1.f, 0.f});
        m_pTransformCom[RES_EYE]->Set_Scale({ 0.25f, max(0.25f * ((m_vFrame.x - 0.4f) * 10.f), 0.0001f), 0.25f, 0.f });

        if (m_vFrame.x > 0.5f)
        {
            // CFX_EffigyFireExplode::EXPLODE_DESC tExplodeDesc = { };
            // tExplodeDesc.fExplodeExpand = 1.5f;
            // tExplodeDesc.fScale = 2.5f;
            // tExplodeDesc.ePosType = CAssetFX::ASFX_DESC::POS_FIX;
            // tExplodeDesc.vInitPosition = m_pTransformCom[RES_EYE]->Get_State(CTransform::STATE_POSITION);
            // 
            // m_pGameInstance->Play_AssetFX(L"EffigyFireExplode", Get_ParticleTag(L"EffigyFireExplode"), &tExplodeDesc);

            m_bDuration = TRUE;
            m_fDuration = -1.f;
        }
    }

    return S_OK;
}

HRESULT CFX_RaidenES::LateTick(_float fTimeDelta)
{
    m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);

    return S_OK;
}

HRESULT CFX_RaidenES::Render()
{
    if (FAILED(CFX_RaidenES::Bind_ShaderResources()))
        return E_FAIL;

    /* Line */
    if (FAILED(m_pShaderCom[RES_LINE]->Begin(VTXTEX_RAIDEN_LIGHT)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    /* Eye */
    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (_uint siMesh = 0; siMesh < iNumMeshes; siMesh++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom[RES_EYE], "g_DiffuseTexture", static_cast<_uint>(siMesh), TEXTURETYPE::aiTextureType_DIFFUSE)))
            return E_FAIL;

        if (FAILED(m_pShaderCom[RES_EYE]->Begin(VTXMESH_CHAKRA)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(siMesh)))
            return E_FAIL;
    }

    return S_OK;
}

CFX_RaidenES* CFX_RaidenES::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CFX_RaidenES* pInstance = new CFX_RaidenES(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CFX_RaidenES"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CFX_RaidenES::Clone(void* pArg)
{
    CFX_RaidenES* pInstance = new CFX_RaidenES(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CFX_RaidenES"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFX_RaidenES::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom[RES_LINE]);
    Safe_Release(m_pShaderCom[RES_EYE]);
    Safe_Release(m_pRendererCom);
    Safe_Release(m_pTransformCom[RES_LINE]);
    Safe_Release(m_pTransformCom[RES_EYE]);
    Safe_Release(m_pVIBufferCom);
}
