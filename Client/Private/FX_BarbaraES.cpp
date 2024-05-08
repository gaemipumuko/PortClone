#include "stdafx.h"
#include "FX_BarbaraES.h"
#include "Player.h"

#include "DeciderObject.h"
#include "GameInstance.h"

CFX_BarbaraES::CFX_BarbaraES(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CAssetFX(pDevice, pContext)
{
}

CFX_BarbaraES::CFX_BarbaraES(const CFX_BarbaraES& rhs)
    : CAssetFX(rhs)
{
}

HRESULT CFX_BarbaraES::Add_Component(void* pArg)
{
    /* For.Com_Model_ES1 */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Barbara_ES_1"),
        TEXT("Com_Model_ES1"), (CComponent**)&m_pModelCom[BES_ES1])))
        return E_FAIL;

    /* For.Com_Model_ES2 */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Barbara_ES_2"),
        TEXT("Com_Model_ES2"), (CComponent**)&m_pModelCom[BES_ES2])))
        return E_FAIL;

    /* For.Com_Model_ES3 */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Barbara_ES_3"),
        TEXT("Com_Model_ES3"), (CComponent**)&m_pModelCom[BES_ES3])))
        return E_FAIL;

    /* For.Com_Model_ES4 */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Barbara_ES_4"),
        TEXT("Com_Model_ES4"), (CComponent**)&m_pModelCom[BES_ES4])))
        return E_FAIL;

    /* For.Com_Shader_Mesh */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_MeshFX"),
        TEXT("Com_Shader_Mesh"), (CComponent**)&m_pShaderCom[SHADER_MESH])))
        return E_FAIL;

    /* For.Com_Shader_Anim */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimFX"),
        TEXT("Com_Shader_Anim"), (CComponent**)&m_pShaderCom[SHADER_ANIM])))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Barbara_ES"),
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    /* For.Com_Renderer */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
        TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
        return E_FAIL;

    /* For.Com_Transform_ES1 */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform_ES1"), (CComponent**)&m_pTransformCom[BES_ES1])))
        return E_FAIL;

    /* For.Com_Transform_ES2 */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform_ES2"), (CComponent**)&m_pTransformCom[BES_ES2])))
        return E_FAIL;

    /* For.Com_Transform_ES3 */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform_ES3"), (CComponent**)&m_pTransformCom[BES_ES3])))
        return E_FAIL;

    /* For.Com_Transform_ES4 */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform_ES4"), (CComponent**)&m_pTransformCom[BES_ES4])))
        return E_FAIL;

    return S_OK;
}

HRESULT CFX_BarbaraES::Bind_ShaderResources()
{
    /* Mesh */
    if (FAILED(m_pShaderCom[SHADER_MESH]->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom[SHADER_MESH]->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResources(m_pShaderCom[SHADER_MESH], "g_EffectTextures")))
        return E_FAIL;

    if (FAILED(m_pShaderCom[SHADER_MESH]->Bind_RawValue("g_vUVRate", &m_vFrame, sizeof(_float4))))
        return E_FAIL;

    /* Anim */
    if (FAILED(m_pShaderCom[SHADER_ANIM]->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom[SHADER_ANIM]->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResources(m_pShaderCom[SHADER_ANIM], "g_EffectTextures")))
        return E_FAIL;

    if (FAILED(m_pShaderCom[SHADER_ANIM]->Bind_RawValue("g_vUVRate", &m_vFrame, sizeof(_float4))))
        return E_FAIL;

    return S_OK;
}

HRESULT CFX_BarbaraES::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CFX_BarbaraES::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component(pArg)))
        return E_FAIL;

    ASFX_DESC* pAsfxDesc = static_cast<ASFX_DESC*>(pArg);
    if (pAsfxDesc->ePosType == ASFX_DESC::POS_FIX)
        return E_FAIL;

    m_pRelativeTransform = pAsfxDesc->pTransform;
    Safe_AddRef(m_pRelativeTransform);

    _vector vPosition   = pAsfxDesc->pTransform->Get_State(CTransform::STATE_POSITION) + _vector{0.f, 1.00f, 0.f, 0.f};
    m_pTransformCom[BES_ES1]->Set_State(CTransform::STATE_POSITION, vPosition);
    m_pTransformCom[BES_ES2]->Set_State(CTransform::STATE_POSITION, vPosition);
    m_pTransformCom[BES_ES3]->Set_State(CTransform::STATE_POSITION, vPosition);
    vPosition           = pAsfxDesc->pTransform->Get_State(CTransform::STATE_POSITION) + _vector{ 0.f, 0.75f, 0.f, 0.f };
    m_pTransformCom[BES_ES4]->Set_State(CTransform::STATE_POSITION, vPosition);

    m_pTransformCom[BES_ES1]->Set_Scale({ 1.000f, 1.000f, 1.000f, 0.f });
    m_pTransformCom[BES_ES2]->Set_Scale({ 0.025f, 0.025f, 0.025f, 0.f });
    m_pTransformCom[BES_ES3]->Set_Scale({ 0.025f, 0.025f, 0.025f, 0.f });
    m_pTransformCom[BES_ES4]->Set_Scale({ 0.025f, 0.025f, 0.025f, 0.f });

    m_eStep = STEP_1;

    m_pGameInstance->Add_StatusEffect(SE_BARABRA_ES, L"Barbara", 0); // 지속 시간이 있는 버프 or 디버프 스킬 등록

    return S_OK;
}
    
HRESULT CFX_BarbaraES::PreviousTick(_float fTimeDelta)
{
    if (CDeciderObject::Get_RenderMode() != CDeciderObject::RMODE_UI)
    {
        if (FAILED(__super::PreviousTick(fTimeDelta)))
            return E_FAIL;

        m_fTickHeal += fTimeDelta;
        if (m_fTickHeal > 4.f)
        {
            m_fTickHeal = 0.f;
            CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Player"));
            if (pPlayer->Get_CombatMode() == FALSE)
            {
                pPlayer->Set_RatioHeal(pPlayer->Get_CurrentIndex(), 0.07f, 722);
                pPlayer->Set_Damage(0.f, ELEMENT_WATER, FSM_END);
            }
            else
            {
                pPlayer->Set_RatioHeal(PLAYER_THREE, 0.07f, 722);
                pPlayer->Set_AutoDamage(PLAYER_THREE, 0.f, ELEMENT_WATER, FSM_END);
            }
        }
    }

    return S_OK;
}

HRESULT CFX_BarbaraES::Tick(_float fTimeDelta)
{
    if (CDeciderObject::Get_RenderMode() != CDeciderObject::RMODE_UI)
    {
        /* FX Frame */
        m_vFrame.x += fTimeDelta;
        m_vFrame.y += fTimeDelta;
        m_vFrame.x = fmod(m_vFrame.x, 2.f);

        /* Turn Matrix */
        m_pTransformCom[BES_ES2]->Turn({ 0.f, 1.f, 0.f, 0.f }, fTimeDelta);
        m_pTransformCom[BES_ES3]->Turn({ 0.f, 1.f, 0.f, 0.f }, fTimeDelta);

        /* Invalidate */
        if (m_eStep == STEP_1)
        {
            m_pModelCom[BES_ES1]->Set_Animation(0);

            _float fRate = m_pModelCom[BES_ES1]->Play_Animation(FALSE, m_pTransformCom[BES_ES1], fTimeDelta * m_fAnimSpeed);
            if (fRate > 60.f)
                m_fAnimSpeed = 2.5f;
            else if (fRate > 45.f)
                m_fAnimSpeed = 1.5f;

            if (fRate > 95.f)
            {
                m_vFrame.x = 0.f;
                m_vFrame.y = 0.f;
                m_vFrame.z = 2.f;
                m_eStep = STEP_2;
            }
        }
        else if (m_eStep == STEP_2)
        {
            if (m_vFrame.y > 0.5f)
            {
                m_vFrame.x = 0.f;
                m_vFrame.y = 0.f;
                m_vFrame.z = 3.f;
                m_eStep = STEP_3;
            }
        }
        else if (m_eStep == STEP_3)
        {
            if (m_vFrame.y > 0.5f)
            {
                m_vFrame.x = 0.f;
                m_vFrame.y = 0.f;
                m_vFrame.z = 4.f;
                m_eStep = STEP_4;
            }
        }
        else if (m_eStep == STEP_4)
        {
            if (m_vFrame.y > 0.5f)
            {
                m_vFrame.x = 0.f;
                m_vFrame.y = 0.f;
                m_vFrame.z = 5.f;
                m_eStep = STEP_5;
            }
        }
        else if (m_eStep == STEP_5)
        {
            if (m_vFrame.y > 0.5f)
            {
                m_vFrame.x = 0.f;
                m_vFrame.y = 0.f;
                m_vFrame.z = 6.f;
                m_eStep = STEP_6;
            }
        }
        else if (m_eStep == STEP_6)
        {
            if (m_vFrame.y > 0.3f)
            {
                m_vFrame.x = 0.f;
                m_vFrame.y = 0.f;
                m_vFrame.z = 7.f;
                m_eStep = STEP_7;
            }
        }
    }

    return S_OK;
}

HRESULT CFX_BarbaraES::LateTick(_float fTimeDelta)
{
    /* Move Matrix */
    CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Player"));
    _vector vPosition = {};

    if (pPlayer->Get_CombatMode() == FALSE)
    {
        CTransform* pPlayerTransform = static_cast<CTransform*>(pPlayer->Get_PlayerInfo(pPlayer->Get_CurrentIndex()).pTransform);
        vPosition = pPlayerTransform->Get_State(CTransform::STATE_POSITION) + _vector{ 0.f, 1.00f, 0.f, 0.f };
    }
    else
    {
        vPosition = m_pRelativeTransform->Get_State(CTransform::STATE_POSITION) + _vector{ 0.f, 1.00f, 0.f, 0.f };
    }


    m_pTransformCom[BES_ES1]->Set_State(CTransform::STATE_POSITION, vPosition);
    m_pTransformCom[BES_ES2]->Set_State(CTransform::STATE_POSITION, vPosition);
    m_pTransformCom[BES_ES3]->Set_State(CTransform::STATE_POSITION, vPosition);
    //vPosition           = m_pRelativeTransform->Get_State(CTransform::STATE_POSITION) + _vector{ 0.f, 0.75f, 0.f, 0.f };
    vPosition -= _vector{ 0.f, 0.25f, 0.f, 0.f };
    m_pTransformCom[BES_ES4]->Set_State(CTransform::STATE_POSITION, vPosition);


    m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);

    return S_OK;
}

HRESULT CFX_BarbaraES::Render()
{
    if (FAILED(CFX_BarbaraES::Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = { };

    switch (m_eStep)
    {
    case STEP_1:
        if (FAILED(m_pShaderCom[SHADER_ANIM]->Bind_Matrix("g_WorldMatrix", m_pTransformCom[BES_ES1]->Get_WorldFloat4x4())))
            return E_FAIL;

        iNumMeshes = m_pModelCom[BES_ES1]->Get_NumMeshes();
        for (_uint siMesh = 0; siMesh < iNumMeshes; siMesh++)
        {
            if (FAILED(m_pModelCom[BES_ES1]->Bind_BoneMatrices(static_cast<_uint>(siMesh), m_pShaderCom[SHADER_ANIM], "g_BoneMatrices")))
                return E_FAIL;

            if (FAILED(m_pShaderCom[SHADER_ANIM]->Begin(ANIMFX_BARBARA_ES1)))
                return E_FAIL;

            if (FAILED(m_pModelCom[BES_ES1]->Render(siMesh)))
                return E_FAIL;
        }
        break;
    case STEP_2:    // ES1 Dissolve
        if (FAILED(m_pShaderCom[SHADER_ANIM]->Bind_Matrix("g_WorldMatrix", m_pTransformCom[BES_ES1]->Get_WorldFloat4x4())))
            return E_FAIL;

        iNumMeshes = m_pModelCom[BES_ES1]->Get_NumMeshes();
        for (_uint siMesh = 0; siMesh < iNumMeshes; siMesh++)
        {
            if (FAILED(m_pModelCom[BES_ES1]->Bind_BoneMatrices(static_cast<_uint>(siMesh), m_pShaderCom[SHADER_ANIM], "g_BoneMatrices")))
                return E_FAIL;

            if (FAILED(m_pShaderCom[SHADER_ANIM]->Begin(ANIMFX_BARBARA_ES1)))
                return E_FAIL;

            if (FAILED(m_pModelCom[BES_ES1]->Render(siMesh)))
                return E_FAIL;
        }

        if (FAILED(m_pShaderCom[SHADER_MESH]->Bind_Matrix("g_WorldMatrix", m_pTransformCom[BES_ES2]->Get_WorldFloat4x4())))
            return E_FAIL;

        iNumMeshes = m_pModelCom[BES_ES2]->Get_NumMeshes();
        for (_uint siMesh = 0; siMesh < iNumMeshes; siMesh++)
        {
            if (FAILED(m_pShaderCom[SHADER_MESH]->Begin(MESHFX_BARBARA_ES2)))
                return E_FAIL;

            if (FAILED(m_pModelCom[BES_ES2]->Render(siMesh)))
                return E_FAIL;
        }
        break;
    case STEP_3:
        if (FAILED(m_pShaderCom[SHADER_MESH]->Bind_Matrix("g_WorldMatrix", m_pTransformCom[BES_ES2]->Get_WorldFloat4x4())))
            return E_FAIL;

        iNumMeshes = m_pModelCom[BES_ES2]->Get_NumMeshes();
        for (_uint siMesh = 0; siMesh < iNumMeshes; siMesh++)
        {
            if (FAILED(m_pShaderCom[SHADER_MESH]->Begin(MESHFX_BARBARA_ES2)))
                return E_FAIL;

            if (FAILED(m_pModelCom[BES_ES2]->Render(siMesh)))
                return E_FAIL;
        }
        break;
    case STEP_4:    // ES2 Dissolve
        if (FAILED(m_pShaderCom[SHADER_MESH]->Bind_Matrix("g_WorldMatrix", m_pTransformCom[BES_ES2]->Get_WorldFloat4x4())))
            return E_FAIL;

        iNumMeshes = m_pModelCom[BES_ES2]->Get_NumMeshes();
        for (_uint siMesh = 0; siMesh < iNumMeshes; siMesh++)
        {
            if (FAILED(m_pShaderCom[SHADER_MESH]->Begin(MESHFX_BARBARA_ES2)))
                return E_FAIL;

            if (FAILED(m_pModelCom[BES_ES2]->Render(siMesh)))
                return E_FAIL;
        }

        if (FAILED(m_pShaderCom[SHADER_MESH]->Bind_Matrix("g_WorldMatrix", m_pTransformCom[BES_ES3]->Get_WorldFloat4x4())))
            return E_FAIL;

        iNumMeshes = m_pModelCom[BES_ES3]->Get_NumMeshes();
        for (_uint siMesh = 0; siMesh < iNumMeshes; siMesh++)
        {
            if (FAILED(m_pShaderCom[SHADER_MESH]->Begin(MESHFX_BARBARA_ES3)))
                return E_FAIL;

            if (FAILED(m_pModelCom[BES_ES3]->Render(siMesh)))
                return E_FAIL;
        }
        break;
    case STEP_5:
        if (FAILED(m_pShaderCom[SHADER_MESH]->Bind_Matrix("g_WorldMatrix", m_pTransformCom[BES_ES3]->Get_WorldFloat4x4())))
            return E_FAIL;

        iNumMeshes = m_pModelCom[BES_ES3]->Get_NumMeshes();
        for (_uint siMesh = 0; siMesh < iNumMeshes; siMesh++)
        {
            if (FAILED(m_pShaderCom[SHADER_MESH]->Begin(MESHFX_BARBARA_ES3)))
                return E_FAIL;

            if (FAILED(m_pModelCom[BES_ES3]->Render(siMesh)))
                return E_FAIL;
        }
        break;
    case STEP_6:    // ES3 Dissolve
        if (FAILED(m_pShaderCom[SHADER_MESH]->Bind_Matrix("g_WorldMatrix", m_pTransformCom[BES_ES3]->Get_WorldFloat4x4())))
            return E_FAIL;

        iNumMeshes = m_pModelCom[BES_ES3]->Get_NumMeshes();
        for (_uint siMesh = 0; siMesh < iNumMeshes; siMesh++)
        {
            if (FAILED(m_pShaderCom[SHADER_MESH]->Begin(MESHFX_BARBARA_ES3)))
                return E_FAIL;

            if (FAILED(m_pModelCom[BES_ES3]->Render(siMesh)))
                return E_FAIL;
        }
        break;
    case STEP_7:
        if (FAILED(m_pShaderCom[SHADER_MESH]->Bind_Matrix("g_WorldMatrix", m_pTransformCom[BES_ES4]->Get_WorldFloat4x4())))
            return E_FAIL;

        iNumMeshes = m_pModelCom[BES_ES4]->Get_NumMeshes();
        for (_uint siMesh = 0; siMesh < iNumMeshes; siMesh++)
        {
            if (FAILED(m_pShaderCom[SHADER_MESH]->Begin(MESHFX_BARBARA_ES4)))
                return E_FAIL;

            if (FAILED(m_pModelCom[BES_ES4]->Render(siMesh)))
                return E_FAIL;
        }
        break;
    }

    return S_OK;
}

CFX_BarbaraES* CFX_BarbaraES::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CFX_BarbaraES* pInstance = new CFX_BarbaraES(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CFX_BarbaraES"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CFX_BarbaraES::Clone(void* pArg)
{
    CFX_BarbaraES* pInstance = new CFX_BarbaraES(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CFX_BarbaraES"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFX_BarbaraES::Free()
{
    __super::Free();

    m_pGameInstance->Release_StatusEffect(SE_BARABRA_ES, L"Barbara");

    Safe_Release(m_pModelCom[BES_ES1]);
    Safe_Release(m_pModelCom[BES_ES2]);
    Safe_Release(m_pModelCom[BES_ES3]);
    Safe_Release(m_pModelCom[BES_ES4]);

    Safe_Release(m_pShaderCom[SHADER_MESH]);
    Safe_Release(m_pShaderCom[SHADER_ANIM]);

    Safe_Release(m_pTransformCom[BES_ES1]);
    Safe_Release(m_pTransformCom[BES_ES2]);
    Safe_Release(m_pTransformCom[BES_ES3]);
    Safe_Release(m_pTransformCom[BES_ES4]);

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pRendererCom);
}