#include "stdafx.h"
#include "FX_EffigyFireImpact.h"

#include "Eff_Impact_Glow.h"
#include "Part_Body.h"
#include "DeciderObject.h"
#include "GameInstance.h"

#define CRACK_DELAY 0.75f
#define EXPLODE_DELAY 0.5f

CFX_EffigyFireImpact::CFX_EffigyFireImpact(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CAssetFX(pDevice, pContext)
{
}

CFX_EffigyFireImpact::CFX_EffigyFireImpact(const CFX_EffigyFireImpact& rhs)
    : CAssetFX(rhs)
{
}

HRESULT CFX_EffigyFireImpact::Add_Component(void* pArg)
{
    /* For.Com_Model_EffigyFire_Crack */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_EffigyFire_HalfTorus"),
        TEXT("Com_Model_EffigyFire_Crack"), (CComponent**)&m_pModelCom[FIRE_CRACK])))
        return E_FAIL;

    /* For.Com_Model_EffigyFire_Impact_1 */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_EffigyFire_HalfTorus"),
        TEXT("Com_Model_EffigyFire_Impact_1"), (CComponent**)&m_pModelCom[FIRE_IMPACT_1])))
        return E_FAIL;

    /* For.Com_Model_EffigyFire_Impact_2 */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_EffigyFire_HalfTorus"),
        TEXT("Com_Model_EffigyFire_Impact_2"), (CComponent**)&m_pModelCom[FIRE_IMPACT_2])))
        return E_FAIL;

    /* For.Com_Model_EffigyFire_Impact_3 */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_EffigyFire_HalfTorus"),
        TEXT("Com_Model_EffigyFire_Impact_3"), (CComponent**)&m_pModelCom[FIRE_IMPACT_3])))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_MeshFX"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EffigyFire_Impact"),
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    /* For.Com_Renderer */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
        TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
        return E_FAIL;

    /* For.Com_Transform_Crack */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform_Crack"), (CComponent**)&m_pTransformCom[FIRE_CRACK])))
        return E_FAIL;

    /* For.Com_Transform_Impact_1 */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform_Impact_1"), (CComponent**)&m_pTransformCom[FIRE_IMPACT_1])))
        return E_FAIL;

    /* For.Com_Transform_Impact_2 */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform_Impact_2"), (CComponent**)&m_pTransformCom[FIRE_IMPACT_2])))
        return E_FAIL;

    /* For.Com_Transform_Impact_3 */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform_Impact_3"), (CComponent**)&m_pTransformCom[FIRE_IMPACT_3])))
        return E_FAIL;

    /* For.Com_Collider_SPHERE */
    CBounding_SPHERE::BOUNDING_SPHERE_DESC		tColliderDesc = {};
    tColliderDesc.fRadius = 1.f;
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
        TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tColliderDesc)))
        return E_FAIL;

    m_pGameInstance->Add_Collider_Object(COLL_MONSTER_SKILL, &m_iColliderID, m_pColliderCom, this);

    return S_OK;
}

HRESULT CFX_EffigyFireImpact::Bind_ShaderResources()
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

HRESULT CFX_EffigyFireImpact::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CFX_EffigyFireImpact::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component(pArg)))
        return E_FAIL;

    ASFX_DESC* pAsfxDesc = static_cast<ASFX_DESC*>(pArg);
    if (pAsfxDesc->ePosType == ASFX_DESC::POS_RELATIVE)
        return E_FAIL;

    _vector vInitPosition = pAsfxDesc->vInitPosition + _vector{ 0.f, 0.01f, 0.f, 0.f };

    m_pTransformCom[FIRE_CRACK]->Set_State(CTransform::STATE_POSITION, vInitPosition);
    m_pTransformCom[FIRE_CRACK]->Set_Scale({ 2.f, 0.f, 2.f, 0.f });

    m_pTransformCom[FIRE_IMPACT_1]->Set_State(CTransform::STATE_POSITION, vInitPosition);
    m_pTransformCom[FIRE_IMPACT_1]->Rotation({ 0.f, 1.f, 0.f, 0.f}, XMConvertToRadians(45.f));
    m_pTransformCom[FIRE_IMPACT_1]->Set_Scale({ 4.f, 0.f, 4.f, 0.f });

    m_pTransformCom[FIRE_IMPACT_2]->Set_State(CTransform::STATE_POSITION, vInitPosition + _vector{ 0.f, 0.01f, 0.f, 0.f });
    m_pTransformCom[FIRE_IMPACT_2]->Rotation({ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(90.f));
    m_pTransformCom[FIRE_IMPACT_2]->Set_Scale({ 7.f, 0.f, 7.f, 0.f });

    m_pTransformCom[FIRE_IMPACT_3]->Set_State(CTransform::STATE_POSITION, vInitPosition + _vector{ 0.f, 0.02f, 0.f, 0.f });
    m_pTransformCom[FIRE_IMPACT_3]->Rotation({ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(135.f));
    m_pTransformCom[FIRE_IMPACT_3]->Set_Scale({ 10.f, 0.f, 10.f, 0.f });

    m_eStep = STEP_1;

    m_pColliderCom->Update(m_pTransformCom[FIRE_CRACK]->Get_WorldMatrix());

    return S_OK;
}
    
HRESULT CFX_EffigyFireImpact::PreviousTick(_float fTimeDelta)
{
    if (CDeciderObject::Get_RenderMode() != CDeciderObject::RMODE_UI)
    {
        if (FAILED(__super::PreviousTick(fTimeDelta)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CFX_EffigyFireImpact::Tick(_float fTimeDelta)
{
    if (CDeciderObject::Get_RenderMode() != CDeciderObject::RMODE_UI)
    {
        /* Init Eff */
        if (m_bInitThorn == FALSE)
        {
            CImpact_Glow::IMPACT_DESC tImpactDesc = { };

            tImpactDesc.eELEMENT = ELEMENT_FIRE;
            tImpactDesc.pParent = (CGameObject*)this;
            tImpactDesc.iFanSize = 85;
            tImpactDesc.eEffectID = EFFECT_IMPACT_PLANE_CIRCULAR;
            tImpactDesc.vSpawnPos = m_pTransformCom[FIRE_IMPACT_1]->Get_State(CTransform::STATE_POSITION);
            tImpactDesc.vScale = { 2.f, 3.2f, 1.f, 1.f };
            tImpactDesc.fRadius = 4.f;

            if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Impact_Glow", L"Layer_PostEffect", &tImpactDesc)))
                return E_FAIL;

            m_bInitThorn = TRUE;
        }

        /* FX Frame */
        m_fFrame += fTimeDelta;

        /* Invalidate */
        if (m_eStep == STEP_1)
        {
            if (m_fFrame > CRACK_DELAY)
            {
                m_fFrame = 0.f;
                m_vFrame = { 0.f, 0.f, 0.f, 0.f };

                m_eStep = STEP_2;

                CImpact_Glow::IMPACT_DESC tImpactDesc = { };

                tImpactDesc.eELEMENT = ELEMENT_FIRE;
                tImpactDesc.pParent = (CGameObject*)this;
                tImpactDesc.iFanSize = 85;
                tImpactDesc.eEffectID = EFFECT_IMPACT_PLANE_CIRCULAR;
                tImpactDesc.vSpawnPos = m_pTransformCom[FIRE_IMPACT_1]->Get_State(CTransform::STATE_POSITION);
                tImpactDesc.vScale = { 2.f, 3.2f, 1.f, 1.f };
                tImpactDesc.fRadius = 6.f;

                if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Impact_Glow", L"Layer_PostEffect", &tImpactDesc)))
                    return E_FAIL;

                CBounding_SPHERE::BOUNDING_SPHERE_DESC	tColliderDesc = {};
                tColliderDesc.fRadius = tImpactDesc.fRadius * 0.5f;
                m_pColliderCom->Update_Desc(&tColliderDesc);
                m_pColliderCom->Update(m_pTransformCom[FIRE_CRACK]->Get_WorldMatrix());
                m_bAttack = TRUE;

                /* Particle */
                wstring strParticleTag1 = Get_ParticleTag(L"Effigy_PaperBurn_Eruption_1_1");
                wstring strParticleTag2 = Get_ParticleTag(L"Effigy_PaperBurn_Eruption_1_2");
                wstring strParticleTag3 = Get_ParticleTag(L"Effigy_PaperBurn_Eruption_1_3");
                wstring strParticleTag4 = Get_ParticleTag(L"Effigy_PaperBurn_Eruption_1_4");

                m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_Eruption_1_1", strParticleTag1,
                    PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, nullptr, m_pTransformCom[FIRE_CRACK]);
                m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_Eruption_1_2", strParticleTag2,
                    PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, nullptr, m_pTransformCom[FIRE_CRACK]);
                m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_Eruption_1_3", strParticleTag3,
                    PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, nullptr, m_pTransformCom[FIRE_CRACK]);
                m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_Eruption_1_4", strParticleTag4,
                    PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, nullptr, m_pTransformCom[FIRE_CRACK]);

                m_pGameInstance->Stop_Particle(strParticleTag1, 5.f);
                m_pGameInstance->Stop_Particle(strParticleTag2, 5.f);
                m_pGameInstance->Stop_Particle(strParticleTag3, 5.f);
                m_pGameInstance->Stop_Particle(strParticleTag4, 5.f);

                m_pGameInstance->PlaySoundW(L"EffigyFire_Chopping.mp3");
            }
            else
            {
                /* FX Frame */
                m_vFrame.x = abs(abs(m_fFrame - (CRACK_DELAY * 0.5f)) - (CRACK_DELAY * 0.5f)) * (10.f / (CRACK_DELAY * 0.5f));    // Crack
            }
        }
        else if (m_eStep == STEP_2)
        {
            if (m_fFrame > EXPLODE_DELAY * 0.5f)
            {
                m_eStep = STEP_3;

                CImpact_Glow::IMPACT_DESC tImpactDesc = { };

                tImpactDesc.eELEMENT = ELEMENT_FIRE;
                tImpactDesc.pParent = (CGameObject*)this;
                tImpactDesc.iFanSize = 85;
                tImpactDesc.eEffectID = EFFECT_IMPACT_PLANE_CIRCULAR;
                tImpactDesc.vSpawnPos = m_pTransformCom[FIRE_IMPACT_1]->Get_State(CTransform::STATE_POSITION);
                tImpactDesc.vScale = { 2.f, 3.2f, 1.f, 1.f };
                tImpactDesc.fRadius = 12.f;

                if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Impact_Glow", L"Layer_PostEffect", &tImpactDesc)))
                    return E_FAIL;

                CBounding_SPHERE::BOUNDING_SPHERE_DESC	tColliderDesc = {};
                tColliderDesc.fRadius = tImpactDesc.fRadius * 0.5f;
                m_pColliderCom->Update_Desc(&tColliderDesc);
                m_pColliderCom->Update(m_pTransformCom[FIRE_CRACK]->Get_WorldMatrix());
                m_bAttack = TRUE;

                /* Particle */
                wstring strParticleTag1 = Get_ParticleTag(L"Effigy_PaperBurn_Eruption_2_1");
                wstring strParticleTag2 = Get_ParticleTag(L"Effigy_PaperBurn_Eruption_2_2");
                wstring strParticleTag3 = Get_ParticleTag(L"Effigy_PaperBurn_Eruption_2_3");
                wstring strParticleTag4 = Get_ParticleTag(L"Effigy_PaperBurn_Eruption_2_4");

                m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_Eruption_2_1", strParticleTag1,
                    PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, nullptr, m_pTransformCom[FIRE_CRACK]);
                m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_Eruption_2_2", strParticleTag2,
                    PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, nullptr, m_pTransformCom[FIRE_CRACK]);
                m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_Eruption_2_3", strParticleTag3,
                    PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, nullptr, m_pTransformCom[FIRE_CRACK]);
                m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_Eruption_2_4", strParticleTag4,
                    PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, nullptr, m_pTransformCom[FIRE_CRACK]);

                m_pGameInstance->Stop_Particle(strParticleTag1, 5.f);
                m_pGameInstance->Stop_Particle(strParticleTag2, 5.f);
                m_pGameInstance->Stop_Particle(strParticleTag3, 5.f);
                m_pGameInstance->Stop_Particle(strParticleTag4, 5.f);

                m_pGameInstance->PlaySoundW(L"EffigyFire_FloorExplosion.mp3");
            }
            else
            {
                /* FX Frame */
                m_vFrame.x = abs(abs(m_fFrame - (CRACK_DELAY * 0.5f)) - (CRACK_DELAY * 0.5f)) * (10.f / (CRACK_DELAY * 0.5f));          // Crack
                m_vFrame.y = abs(abs(m_fFrame - (EXPLODE_DELAY * 0.5f)) - (EXPLODE_DELAY * 0.5f)) * (10.f / (EXPLODE_DELAY * 0.5f));    // Impact_1
            }
        }
        else if (m_eStep == STEP_3)
        {
            if (m_fFrame > EXPLODE_DELAY * 1.f)
            {
                m_eStep = STEP_4;

                CImpact_Glow::IMPACT_DESC tImpactDesc = { };

                tImpactDesc.eELEMENT = ELEMENT_FIRE;
                tImpactDesc.pParent = (CGameObject*)this;
                tImpactDesc.iFanSize = 85;
                tImpactDesc.eEffectID = EFFECT_IMPACT_PLANE_CIRCULAR;
                tImpactDesc.vSpawnPos = m_pTransformCom[FIRE_IMPACT_1]->Get_State(CTransform::STATE_POSITION);
                tImpactDesc.vScale = { 2.f, 3.2f, 1.f, 1.f };
                tImpactDesc.fRadius = 15.f;

                if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Impact_Glow", L"Layer_PostEffect", &tImpactDesc)))
                    return E_FAIL;

                CBounding_SPHERE::BOUNDING_SPHERE_DESC	tColliderDesc = {};
                tColliderDesc.fRadius = tImpactDesc.fRadius * 0.5f;
                m_pColliderCom->Update_Desc(&tColliderDesc);
                m_pColliderCom->Update(m_pTransformCom[FIRE_CRACK]->Get_WorldMatrix());
                m_bAttack = TRUE;

                /* Particle */
                wstring strParticleTag1 = Get_ParticleTag(L"Effigy_PaperBurn_Eruption_3_1");
                wstring strParticleTag2 = Get_ParticleTag(L"Effigy_PaperBurn_Eruption_3_2");
                wstring strParticleTag3 = Get_ParticleTag(L"Effigy_PaperBurn_Eruption_3_3");
                wstring strParticleTag4 = Get_ParticleTag(L"Effigy_PaperBurn_Eruption_3_4");

                m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_Eruption_3_1", strParticleTag1,
                    PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, nullptr, m_pTransformCom[FIRE_CRACK]);
                m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_Eruption_3_2", strParticleTag2,
                    PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, nullptr, m_pTransformCom[FIRE_CRACK]);
                m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_Eruption_3_3", strParticleTag3,
                    PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, nullptr, m_pTransformCom[FIRE_CRACK]);
                m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_Eruption_3_4", strParticleTag4,
                    PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, nullptr, m_pTransformCom[FIRE_CRACK]);

                m_pGameInstance->Stop_Particle(strParticleTag1, 5.f);
                m_pGameInstance->Stop_Particle(strParticleTag2, 5.f);
                m_pGameInstance->Stop_Particle(strParticleTag3, 5.f);
                m_pGameInstance->Stop_Particle(strParticleTag4, 5.f);

                m_pGameInstance->PlaySoundW(L"EffigyFire_FloorExplosion.mp3");
            }
            else
            {
                /* FX Frame */
                m_vFrame.x = abs(abs(m_fFrame - (CRACK_DELAY * 0.5f)) - (CRACK_DELAY * 0.5f)) * (10.f / (CRACK_DELAY * 0.5f));                          // Crack
                m_vFrame.y = abs(abs(m_fFrame - (EXPLODE_DELAY * 0.5f)) - (EXPLODE_DELAY * 0.5f)) * (10.f / (EXPLODE_DELAY * 0.5f));                    // Impact_1
                m_vFrame.z =
                    abs(abs((m_fFrame - (EXPLODE_DELAY * 0.5f)) - (EXPLODE_DELAY * 0.5f)) - (EXPLODE_DELAY * 0.5f)) * (10.f / (EXPLODE_DELAY * 0.5f));  // Impact_2
            }
        }
        else if (m_eStep == STEP_4)
        {
            if (m_fFrame > EXPLODE_DELAY * 1.5f)
            {
                m_eStep = STEP_5;
            }
            else
            {
                /* FX Frame */
                m_vFrame.z =
                    abs(abs((m_fFrame - (EXPLODE_DELAY * 0.5f)) - (EXPLODE_DELAY * 0.5f)) - (EXPLODE_DELAY * 0.5f)) * (10.f / (EXPLODE_DELAY * 0.5f));  // Impact_2
                m_vFrame.w =
                    abs(abs((m_fFrame - (EXPLODE_DELAY)) - (EXPLODE_DELAY * 0.5f)) - (EXPLODE_DELAY * 0.5f)) * (10.f / (EXPLODE_DELAY * 0.5f));         // Impact_3
            }
        }
        else if (m_eStep == STEP_5)
        {
            if (m_fFrame > EXPLODE_DELAY * 2.f)
            {
                m_bDuration = TRUE;
                m_fDuration = -1.f;
                m_pGameInstance->Release_Collider_Object(COLL_MONSTER_SKILL, m_iColliderID);
            }
            else
            {
                m_vFrame.w =
                    abs(abs((m_fFrame - (EXPLODE_DELAY)) - (EXPLODE_DELAY * 0.5f)) - (EXPLODE_DELAY * 0.5f)) * (10.f / (EXPLODE_DELAY * 0.5f));         // Impact_3
            }
        }

        if (m_bAttack == TRUE)
        {
            m_bAttack = FALSE;
            CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
            vector<CGameObject*> HitPlayer = m_pGameInstance->Find_Collision_Objects(COLL_MONSTER_SKILL, m_iColliderID, COLL_PLAYER_BODY);
            for (auto PlayerBody : HitPlayer)
            {
                PLAYERINDEX eIndex = pPlayer->Get_PlayerIndex(static_cast<CPart_Body*>(PlayerBody)->Get_CreatureId());

                _vector vSelfPos = m_pTransformCom[FIRE_CRACK]->Get_State(CTransform::STATE_POSITION);
                vSelfPos.m128_f32[1] = 0.f;
                _vector vPlayerPos = pPlayer->Get_Position(eIndex);
                vPlayerPos.m128_f32[1] = 0.f;
                _float fDistance = XMVectorGetX(XMVector3Length(vSelfPos - vPlayerPos));

                if (m_eStep == STEP_2)
                {
                    if (fDistance > 2.f)
                        pPlayer->Set_AutoDamage(eIndex, (_float)m_pGameInstance->Get_Int_Random(943, 2123), ELEMENT_FIRE, FSM_HIT_THROW, 2.5f, 0.f, {});
                }
                else if (m_eStep == STEP_3)
                {
                    if (fDistance > 4.f)
                        pPlayer->Set_AutoDamage(eIndex, (_float)m_pGameInstance->Get_Int_Random(943, 2123), ELEMENT_FIRE, FSM_HIT_THROW, 2.5f, 0.f, {});
                }
                else if (m_eStep == STEP_4)
                {
                    if (fDistance > 6.f)
                        pPlayer->Set_AutoDamage(eIndex, (_float)m_pGameInstance->Get_Int_Random(943, 2123), ELEMENT_FIRE, FSM_HIT_THROW, 2.5f, 0.f, {});
                }

            }
        }
    }

    return S_OK;
}

HRESULT CFX_EffigyFireImpact::LateTick(_float fTimeDelta)
{
    m_pRendererCom->Add_RenderGroup(CRenderer::RG_BLEND, this);

    return S_OK;
}

HRESULT CFX_EffigyFireImpact::Render()
{
    static _int iIndex_1 = 0;
    static _int iIndex_2 = 1;
    static _int iIndex_3 = 2;

    if (FAILED(CFX_EffigyFireImpact::Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = { };

    switch (m_eStep)
    {
    case STEP_1:
        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom[FIRE_CRACK]->Get_WorldFloat4x4())))
            return E_FAIL;

        iNumMeshes = m_pModelCom[FIRE_CRACK]->Get_NumMeshes();
        for (_uint siMesh = 0; siMesh < iNumMeshes; siMesh++)
        {
            if (FAILED(m_pShaderCom->Begin(MESHFX_EFFIGYFIRE_CRACK)))
                return E_FAIL;

            if (FAILED(m_pModelCom[FIRE_CRACK]->Render(siMesh)))
                return E_FAIL;
        }
        break;

    case STEP_2:
        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom[FIRE_CRACK]->Get_WorldFloat4x4())))
            return E_FAIL;

        iNumMeshes = m_pModelCom[FIRE_CRACK]->Get_NumMeshes();
        for (_uint siMesh = 0; siMesh < iNumMeshes; siMesh++)
        {
            if (FAILED(m_pShaderCom->Begin(MESHFX_EFFIGYFIRE_CRACK)))
                return E_FAIL;

            if (FAILED(m_pModelCom[FIRE_CRACK]->Render(siMesh)))
                return E_FAIL;
        }

        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom[FIRE_IMPACT_1]->Get_WorldFloat4x4())))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_iIndex", &iIndex_1, sizeof(_int))))
            return E_FAIL;

        iNumMeshes = m_pModelCom[FIRE_IMPACT_1]->Get_NumMeshes();
        for (_uint siMesh = 0; siMesh < iNumMeshes; siMesh++)
        {
            if (FAILED(m_pShaderCom->Begin(MESHFX_EFFIGYFIRE_IMPACT)))
                return E_FAIL;

            if (FAILED(m_pModelCom[FIRE_IMPACT_1]->Render(siMesh)))
                return E_FAIL;
        }
        break;

    case STEP_3:
        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom[FIRE_CRACK]->Get_WorldFloat4x4())))
            return E_FAIL;

        iNumMeshes = m_pModelCom[FIRE_CRACK]->Get_NumMeshes();
        for (_uint siMesh = 0; siMesh < iNumMeshes; siMesh++)
        {
            if (FAILED(m_pShaderCom->Begin(MESHFX_EFFIGYFIRE_CRACK)))
                return E_FAIL;

            if (FAILED(m_pModelCom[FIRE_CRACK]->Render(siMesh)))
                return E_FAIL;
        }

        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom[FIRE_IMPACT_1]->Get_WorldFloat4x4())))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_iIndex", &iIndex_1, sizeof(_int))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_ColorRate", &m_vFrame.y, sizeof(_float))))
            return E_FAIL;

        iNumMeshes = m_pModelCom[FIRE_IMPACT_1]->Get_NumMeshes();
        for (_uint siMesh = 0; siMesh < iNumMeshes; siMesh++)
        {
            if (FAILED(m_pShaderCom->Begin(MESHFX_EFFIGYFIRE_IMPACT_DISSOLVE)))
                return E_FAIL;

            if (FAILED(m_pModelCom[FIRE_IMPACT_1]->Render(siMesh)))
                return E_FAIL;
        }

        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom[FIRE_IMPACT_2]->Get_WorldFloat4x4())))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_iIndex", &iIndex_2, sizeof(_int))))
            return E_FAIL;

        iNumMeshes = m_pModelCom[FIRE_IMPACT_2]->Get_NumMeshes();
        for (_uint siMesh = 0; siMesh < iNumMeshes; siMesh++)
        {
            if (FAILED(m_pShaderCom->Begin(MESHFX_EFFIGYFIRE_IMPACT)))
                return E_FAIL;

            if (FAILED(m_pModelCom[FIRE_IMPACT_2]->Render(siMesh)))
                return E_FAIL;
        }
        break;

    case STEP_4:
        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom[FIRE_IMPACT_2]->Get_WorldFloat4x4())))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_iIndex", &iIndex_2, sizeof(_int))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_ColorRate", &m_vFrame.z, sizeof(_float))))
            return E_FAIL;

        iNumMeshes = m_pModelCom[FIRE_IMPACT_2]->Get_NumMeshes();
        for (_uint siMesh = 0; siMesh < iNumMeshes; siMesh++)
        {
            if (FAILED(m_pShaderCom->Begin(MESHFX_EFFIGYFIRE_IMPACT_DISSOLVE)))
                return E_FAIL;

            if (FAILED(m_pModelCom[FIRE_IMPACT_2]->Render(siMesh)))
                return E_FAIL;
        }

        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom[FIRE_IMPACT_3]->Get_WorldFloat4x4())))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_iIndex", &iIndex_3, sizeof(_int))))
            return E_FAIL;

        iNumMeshes = m_pModelCom[FIRE_IMPACT_3]->Get_NumMeshes();
        for (_uint siMesh = 0; siMesh < iNumMeshes; siMesh++)
        {
            if (FAILED(m_pShaderCom->Begin(MESHFX_EFFIGYFIRE_IMPACT)))
                return E_FAIL;

            if (FAILED(m_pModelCom[FIRE_IMPACT_3]->Render(siMesh)))
                return E_FAIL;
        }
        break;

    case STEP_5:
        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom[FIRE_IMPACT_3]->Get_WorldFloat4x4())))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_iIndex", &iIndex_3, sizeof(_int))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_ColorRate", &m_vFrame.w, sizeof(_float))))
            return E_FAIL;

        iNumMeshes = m_pModelCom[FIRE_IMPACT_3]->Get_NumMeshes();
        for (_uint siMesh = 0; siMesh < iNumMeshes; siMesh++)
        {
            if (FAILED(m_pShaderCom->Begin(MESHFX_EFFIGYFIRE_IMPACT_DISSOLVE)))
                return E_FAIL;

            if (FAILED(m_pModelCom[FIRE_IMPACT_3]->Render(siMesh)))
                return E_FAIL;
        }
        break;
    }

    //m_pColliderCom->Render();

    return S_OK;
}

CFX_EffigyFireImpact* CFX_EffigyFireImpact::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CFX_EffigyFireImpact* pInstance = new CFX_EffigyFireImpact(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CFX_EffigyFireImpact"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CFX_EffigyFireImpact::Clone(void* pArg)
{
    CFX_EffigyFireImpact* pInstance = new CFX_EffigyFireImpact(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CFX_EffigyFireImpact"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFX_EffigyFireImpact::Free()
{
    __super::Free();

    Safe_Release(m_pColliderCom);

    Safe_Release(m_pModelCom[FIRE_CRACK]);
    Safe_Release(m_pModelCom[FIRE_IMPACT_1]);
    Safe_Release(m_pModelCom[FIRE_IMPACT_2]);
    Safe_Release(m_pModelCom[FIRE_IMPACT_3]);

    Safe_Release(m_pTransformCom[FIRE_CRACK]);
    Safe_Release(m_pTransformCom[FIRE_IMPACT_1]);
    Safe_Release(m_pTransformCom[FIRE_IMPACT_2]);
    Safe_Release(m_pTransformCom[FIRE_IMPACT_3]);

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pRendererCom);

}