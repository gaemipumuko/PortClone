#include "stdafx.h"
#include "Camera_Main.h"

#include "GameInstance.h"
#include "Player.h"
#include "Bounding_Frustum.h"
#include "Collider.h"

#define CUSTOM_ANGLE (XM_1DIVPI - 0.1f)

CCamera_Main::CCamera_Main(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCamera(pDevice, pContext)
{
}

CCamera_Main::CCamera_Main(const CCamera_Main& rhs)
    : CCamera(rhs)
{
}

HRESULT CCamera_Main::Add_Component(void* pArg)
{
    /* For.Com_Transform */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
        return E_FAIL;
    m_pTransformCom->Set_Desc({ 10.f, XMConvertToRadians(90.f) });

    /* For.Com_ArmTransform */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_ArmTransform"), (CComponent**)&m_pArmTransformCom)))
        return E_FAIL;

    /* For.Com_Collider */
    CBounding_FRUSTUM::BOUNDING_FRUSTUM_DESC tBounding_Desc = {};
    {
        tBounding_Desc.vViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
        tBounding_Desc.vProjMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);
    }

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_FRUSTUM"),
        TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tBounding_Desc)))
        return E_FAIL;

#ifdef _DEBUG
    /* For.Com_FreeTransform */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_FreeTransform"), (CComponent**)&m_pFreeTransformCom)))
        return E_FAIL;
    m_pFreeTransformCom->Set_Desc({ 10.f, XMConvertToRadians(90.f) });

    /* For.Com_Renderer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
        TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
        return E_FAIL;
#endif

    return S_OK;
}

void CCamera_Main::Set_ActionMode(_float fActionSpeed, _bool NearAction)
{
    m_bActionMode = TRUE;
    m_fActionSpeed = fActionSpeed;
    m_bNearAction = NearAction;
    m_iActionNum = 0;
}

HRESULT CCamera_Main::Action_Camera(_float fTimeDelta)
{
    if (m_bNearAction)
    {
        if (m_iActionNum == 0)
        {
            m_fRadius -= fTimeDelta * m_fActionSpeed;

            if (m_fRadius <= 2.f)
                m_iActionNum++;
        }
        else if (m_iActionNum == 1)
        {
            m_fRadius += fTimeDelta * m_fActionSpeed / 1.5f;

            if (m_fRadius >= 3.f) /* 원상태 */
            {
                m_fRadius = 3.f;
                m_bActionMode = FALSE;
                m_fActionSpeed = 0.f;
                m_iActionNum = 0;
            }
        }
    }
    else
    {
        if (m_iActionNum == 0)
        {
            m_fRadius += fTimeDelta * m_fActionSpeed;

            if (m_fRadius >= 4.f)
                m_iActionNum++;
        }
        else if (m_iActionNum == 1)
        {
            m_fRadius -= fTimeDelta * m_fActionSpeed / 1.5f;

            if (m_fRadius <= 3.f) /* 원상태 */
            {
                m_fRadius = 3.f;
                m_bActionMode = FALSE;
                m_fActionSpeed = 0.f;
                m_iActionNum = 0;
            }
        }
    }
    
    return S_OK;
}

void CCamera_Main::Set_ShakingMode(_float fMaxRange, _float fShakingSpeed, _int iMaxShakingNum)
{
    m_bShakingMode = TRUE;

    m_fMaxRange = fMaxRange;
    m_fShakingSpeed = fShakingSpeed;
    m_iMaxShakingNum = iMaxShakingNum;
}

HRESULT CCamera_Main::Shaking_Camera(_float fTimeDelta)
{
    if (m_iShakingNum >= m_iMaxShakingNum * 2)
    {
        m_bShakingMode = FALSE;
        m_fMoveValue = 0.f;
        m_fMaxRange = 0.f;
        m_fShakingSpeed = 0.f;
        m_iShakingNum = 0;
        m_iMaxShakingNum = 0;
        return S_OK;
    }

    _vector CameraPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    _vector AddPos = { };

    if(m_fMoveValue < m_fMaxRange)
        m_fMoveValue += m_fShakingSpeed * fTimeDelta;
    else
    {
        m_fMoveValue = 0.f;
        m_iShakingNum++;
    }

    AddPos = XMVectorSet(0.f, m_fShakingSpeed * fTimeDelta, 0.f, 0.f);

    if (m_iShakingNum % 2 == 0)
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, CameraPos + AddPos);
    else if(m_iShakingNum % 2 ==1)
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, CameraPos - AddPos);

    return S_OK;

}

void CCamera_Main::Set_SceneMode(CAMSCENEID eCamSceneId, SCENEID eSceneNum, _float fMargin, _float fCameraSpeed, _bool bSceneMode, CTransform* pTransform)
{
    m_bSceneMode = bSceneMode;
    m_eCharacterId = eCamSceneId;
    m_eSceneNum = eSceneNum;
    if (fMargin != -1.f)
        m_fMargin = fMargin;
    m_fCameraSpeed = fCameraSpeed;

    m_bSceneShaking = FALSE;

    m_vFirstCamPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    m_vFirstCamRot = m_pArmTransformCom->Get_State(CTransform::STATE_LOOK);

    m_bShakingMode = FALSE;
    m_fMoveValue = 0.f;
    m_fMaxRange = 0.f;
    m_fShakingSpeed = 0.f;
    m_iShakingNum = 0;
    m_iMaxShakingNum = 0;

    m_bEndScene = FALSE;

    m_pRefTransform = pTransform;

    /* NPC 방향 플레이어 쳐다보면서 변하기때문에 처음꺼만 받아와서 카메라 설정 */
    m_bInitDir = { FALSE };
    m_vInitDir = {};

    /*if (eSceneNum == SCENE_END)
        m_bLerpMode = TRUE;*/
}

SCENEID CCamera_Main::Get_SceneId(CAMSCENEID eCamSceneId)
{
    if(eCamSceneId == m_eCharacterId)
        return m_eSceneNum;

    return SCENEID::SCENE_END;
}

_bool CCamera_Main::End_Scene(CAMSCENEID eCamSceneId)
{
    if (eCamSceneId == m_eCharacterId)
        return m_bEndScene;

    return FALSE;
}

_bool CCamera_Main::Culling_Frustum(const CCollider* pTargetCollider)
{
    return m_pColliderCom->Intersect(pTargetCollider);
}

void CCamera_Main::Lerp_Camera(_float fTimeDelta)
{
    _vector CameraPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
    _int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

    CTransform* pPlayerTransform =
        static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

    if (pPlayerTransform == nullptr)
        return;

    /*_vector vPlayerPosition = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
    _vector vPlayerLook = pPlayerTransform->Get_State(CTransform::STATE_LOOK);

    vPlayerLook = XMVector3Normalize(vPlayerLook);
    vPlayerPosition += XMVectorSet(0.f, vPlayerPosition.m128_f32[1] + 0.5f, 0.f, 0.f);

    m_fMargin += fTimeDelta * m_fCameraSpeed;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPosition - vPlayerLook);
    m_pTransformCom->Look_At_Position(vPlayerPosition);

    _vector CameraPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    CameraPos.m128_f32[1] += 1.2f;
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, CameraPos - (XMVector3Normalize(vPlayerLook) * m_fMargin));
    m_pArmTransformCom->Look_At_Position(vPlayerPosition);
    _vector vCameraLook = m_pArmTransformCom->Get_State(CTransform::STATE_LOOK);*/

    /*_vector vPlayerPosition = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
    _vector vCameraPosition = m_pArmTransformCom->Get_State(CTransform::STATE_LOOK);

    vCameraPosition = XMVector3Normalize(vCameraPosition) * m_fRadius;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPosition + vCameraPosition);
    m_pTransformCom->Look_At_Position(vPlayerPosition);*/

    _vector vCurrentPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    _vector vPlayerPosition = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
    vPlayerPosition.m128_f32[1] += 1.5f;

    _vector vPlayerLook = pPlayerTransform->Get_State(CTransform::STATE_LOOK);
    _vector vCameraLook = m_pArmTransformCom->Get_State(CTransform::STATE_LOOK);
    
    vCameraLook = XMVector3Normalize(vCameraLook) * m_fRadius;
    _vector vTargetPosition = vPlayerPosition + vCameraLook;
    
    float LerpFactort = 1.f;
    _vector vFinalPosition = XMVectorLerp(vTargetPosition, vCurrentPosition, LerpFactort);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurrentPosition);
    m_pTransformCom->Look_At_Position(vPlayerPosition);
    m_pArmTransformCom->Set_Look_At_Direction(XMVector3Normalize(vPlayerLook));
    _vector vDistance = vCurrentPosition - vTargetPosition;
    
    //if (XMVector3Length(vDistance).m128_f32[0] <= 0.1f)
        m_bLerpMode = FALSE;
}

HRESULT CCamera_Main::SceneCamera(_float fTimeDelta)
{
    switch (m_eCharacterId)
    {
    case CAMSCENEID::CAMSCENEID_CHANGE:
        Change_Scene(fTimeDelta);
        break;
    case CAMSCENEID::CAMSCENEID_BARBARA:
        Barbara_Burst(fTimeDelta);
        break;
    case CAMSCENEID::CAMSCENEID_DILUC:
        Diluc_Burst(fTimeDelta);
        break;
    case CAMSCENEID::CAMSCENEID_ZHONGLI:
        Zhogli_Burst(fTimeDelta);
        break;
    case CAMSCENEID::CAMSCENEID_RAIDEN:
        Raiden_Burst(fTimeDelta);
        break;
    case CAMSCENEID::CAMSCENEID_ANDRIUS:
        Andrius_CutScene(fTimeDelta);
        break;
    case CAMSCENEID::CAMSCENEID_DVALIN:
        Dvalin_CutScene(fTimeDelta);
        break;
    case CAMSCENEID::CAMSCENEID_BASICTALK:
        BasicTalk_Scene(fTimeDelta);
        break;
    case CAMSCENEID::CAMSCENEID_AMBORTALK:
        AmborTalk_Scene(fTimeDelta);
        break;
    case CAMSCENEID::CAMSCENEID_GATE:
        Gate_CutScene(fTimeDelta);
        break;
    case CAMSCENEID::CAMSCENEID_DUNGEONDOOR:
        Dungeon_CutScene(fTimeDelta);
        break;
    }

    return S_OK;
}

HRESULT CCamera_Main::Diluc_Burst(_float fTimeDelta)
{
    switch (m_eSceneNum)
    {
    case SCENE_ONE:
    {
        _vector CameraPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
        _int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

        CTransform* pPlayerTransform =
            static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

        if (pPlayerTransform == nullptr)
            return E_FAIL;

        _vector vPlayerPosition = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
        _vector vPlayerLook = pPlayerTransform->Get_State(CTransform::STATE_LOOK);

        vPlayerLook = XMVector3Normalize(vPlayerLook);
        vPlayerPosition.m128_f32[1] += 1.5f;

        m_fMargin -= fTimeDelta * m_fCameraSpeed;

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPosition + vPlayerLook * m_fMargin);
        m_pTransformCom->Look_At_Position(vPlayerPosition);
    }
    break;
    case SCENE_TWO:
    {
        _vector CameraPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
        _int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

        CTransform* pPlayerTransform =
            static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

        if (pPlayerTransform == nullptr)
            return E_FAIL;

        _vector vPlayerPosition = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
        _vector vPlayerLook = pPlayerTransform->Get_State(CTransform::STATE_LOOK);

        vPlayerLook = XMVector3Normalize(vPlayerLook);
        vPlayerPosition.m128_f32[1] += 1.5f;

        m_fMargin += fTimeDelta * m_fCameraSpeed;

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPosition + vPlayerLook * m_fMargin);
        m_pTransformCom->Look_At_Position(vPlayerPosition);
    }
        break;
    case SCENE_THREE:
    {
        CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
        _int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

        CTransform* pPlayerTransform =
            static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

        if (pPlayerTransform == nullptr)
            return E_FAIL;

        _vector vPlayerPosition = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
        _vector vPlayerLook = pPlayerTransform->Get_State(CTransform::STATE_LOOK);

        vPlayerLook = XMVector3Normalize(vPlayerLook);
        vPlayerPosition.m128_f32[1] += 0.5f;

        m_fMargin += fTimeDelta * m_fCameraSpeed;

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPosition - vPlayerLook);
        m_pTransformCom->Look_At_Position(vPlayerPosition);

        _vector CameraPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        CameraPos.m128_f32[1] += 1.2f;
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, CameraPos - (XMVector3Normalize(vPlayerLook) * m_fMargin));
    }
    break;
    }

    return S_OK;
}

HRESULT CCamera_Main::Zhogli_Burst(_float fTimeDelta)
{
    switch (m_eSceneNum)
    {
        case SCENE_ONE:
        {
            CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
            _int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

            CTransform* pPlayerTransform =
                static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

            if (pPlayerTransform == nullptr)
                return E_FAIL;

            _vector vPlayerPosition = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
            _vector vPlayerRight = pPlayerTransform->Get_State(CTransform::STATE_RIGHT);
            _vector vPlayerLook = pPlayerTransform->Get_State(CTransform::STATE_LOOK);
            _vector vCameraRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
            
            vPlayerRight = XMVector3Normalize(vPlayerRight);
            vPlayerPosition.m128_f32[1] += 0.85f;
            
            m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPosition + vPlayerRight * 0.6f);
            m_fMargin -= fTimeDelta * m_fCameraSpeed;

            vPlayerPosition.m128_f32[1] += 1.f;
            m_pTransformCom->Look_At_Position(vPlayerPosition);

            _vector CameraPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
            m_pTransformCom->Set_State(CTransform::STATE_POSITION, CameraPos - (XMVector3Normalize(vPlayerLook) * m_fMargin * 0.8f));
        }
        break;
        case SCENE_TWO:
        {
            CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
            _int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

            CTransform* pPlayerTransform =
                static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

            if (pPlayerTransform == nullptr)
                return E_FAIL;

            _vector vPlayerPosition = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
            _vector vPlayerLook = pPlayerTransform->Get_State(CTransform::STATE_LOOK);

            vPlayerLook = XMVector3Normalize(vPlayerLook);
            vPlayerPosition.m128_f32[1] += 0.5f;

            m_fMargin += fTimeDelta * m_fCameraSpeed;

            m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPosition - vPlayerLook);
            m_pTransformCom->Look_At_Position(vPlayerPosition);

            _vector CameraPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
            CameraPos.m128_f32[1] += 1.2f;
            m_pTransformCom->Set_State(CTransform::STATE_POSITION, CameraPos - (XMVector3Normalize(vPlayerLook) * m_fMargin));

            if (!m_bSceneShaking)
            {
                Set_ShakingMode(0.05f, 2.f, 5);
                m_bSceneShaking = TRUE;
            }
        }
        break;
    }

    return S_OK;
}

HRESULT CCamera_Main::Barbara_Burst(_float fTimeDelta)
{
    switch (m_eSceneNum)
    {
    case SCENE_ONE:
    {
        _vector CameraPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
        _int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

        CTransform* pPlayerTransform =
            static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

        if (pPlayerTransform == nullptr)
            return E_FAIL;

        _vector vPlayerPosition = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
        _vector vPlayerLook = pPlayerTransform->Get_State(CTransform::STATE_LOOK);

        vPlayerLook = XMVector3Normalize(vPlayerLook);
        vPlayerPosition.m128_f32[1] += 1.25f;

        m_fMargin -= fTimeDelta * m_fCameraSpeed;

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPosition + vPlayerLook * m_fMargin);
        m_pTransformCom->Look_At_Position(vPlayerPosition);
    }
    break;
    case SCENE_TWO:
    {
        _vector CameraPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
        _int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

        CTransform* pPlayerTransform =
            static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

        if (pPlayerTransform == nullptr)
            return E_FAIL;

        _vector vPlayerPosition = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
        _vector vPlayerLook = pPlayerTransform->Get_State(CTransform::STATE_LOOK);

        vPlayerLook = XMVector3Normalize(vPlayerLook);
        vPlayerPosition.m128_f32[1] += 1.25f;

        m_fMargin += fTimeDelta * m_fCameraSpeed;

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPosition + vPlayerLook * m_fMargin);
        m_pTransformCom->Look_At_Position(vPlayerPosition);
    }
    break;
    }

    return S_OK;
}

HRESULT CCamera_Main::Raiden_Burst(_float fTimeDelta)
{
    switch (m_eSceneNum)
    {
    case SCENE_ONE:
    {
        _vector CameraPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
        _int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

        CTransform* pPlayerTransform =
            static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

        if (pPlayerTransform == nullptr)
            return E_FAIL;

        _vector vPlayerPosition = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
        _vector vPlayerLook = pPlayerTransform->Get_State(CTransform::STATE_LOOK);

        vPlayerLook = XMVector3Normalize(vPlayerLook);
        vPlayerPosition.m128_f32[1] += 1.4f;

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPosition + vPlayerLook * m_fMargin);
        m_pTransformCom->Look_At_Position(vPlayerPosition);
    }
    break;
    case SCENE_TWO:
    {
        CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
        _int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

        CTransform* pPlayerTransform =
            static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

        if (pPlayerTransform == nullptr)
            return E_FAIL;

        _vector vPlayerPosition = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
        _vector vPlayerLook = pPlayerTransform->Get_State(CTransform::STATE_LOOK);

        vPlayerLook = XMVector3Normalize(vPlayerLook);
        vPlayerPosition.m128_f32[1] += 0.5f;

        m_fMargin += fTimeDelta * m_fCameraSpeed;

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPosition - vPlayerLook);
        m_pTransformCom->Look_At_Position(vPlayerPosition);

        _vector CameraPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        CameraPos.m128_f32[1] += 1.2f;
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, CameraPos - (XMVector3Normalize(vPlayerLook) * m_fMargin));
    }
    break;
    }

    return S_OK;
}

HRESULT CCamera_Main::Change_Scene(_float fTimeDelta)
{
    switch (m_eSceneNum)
    {
    case SCENE_ONE:
    {
        _vector CameraPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
        _int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

        CTransform* pPlayerTransform =
            static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

        if (pPlayerTransform == nullptr)
            return E_FAIL;

        _vector vPlayerPosition = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
        _vector vPlayerLook = pPlayerTransform->Get_State(CTransform::STATE_LOOK);

        vPlayerLook = XMVector3Normalize(vPlayerLook);
        
        if(pPlayer->Get_CurrentIndex() == pPlayer->Get_PlayerIndex(CHARACTER_BARBARA))
            vPlayerPosition.m128_f32[1] += 1.25f;
        else if (pPlayer->Get_CurrentIndex() == pPlayer->Get_PlayerIndex(CHARACTER_RAIDEN))
            vPlayerPosition.m128_f32[1] += 1.35f;
        else
            vPlayerPosition.m128_f32[1] += 1.5f;

        m_fMargin -= fTimeDelta * m_fCameraSpeed;

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPosition + vPlayerLook * m_fMargin);
        m_pTransformCom->Look_At_Position(vPlayerPosition);
    }
    break;
    }

    return S_OK;
}

HRESULT CCamera_Main::Andrius_CutScene(_float fTimeDelta)
{
    if (m_eSceneNum != SCENE_END)
    {
        _vector CameraPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        CTransform* pAndriusTransform =
            static_cast<CTransform*>(m_pGameInstance
                ->Find_Layer_Component(m_pGameInstance->Get_CurrentLevel(), L"Layer_Boss", L"Com_Transform"));
        _vector vAndriusPosition = pAndriusTransform->Get_State(CTransform::STATE_POSITION);
        _vector vAndriusRight = XMVector3Normalize(pAndriusTransform->Get_State(CTransform::STATE_RIGHT));
        _vector vAndriusUp= XMVector3Normalize(pAndriusTransform->Get_State(CTransform::STATE_UP));
        _vector vAndriusLook = XMVector3Normalize(pAndriusTransform->Get_State(CTransform::STATE_LOOK));
         
        if (pAndriusTransform == nullptr)
            return E_FAIL;

        switch (m_eSceneNum)
        {
        case Client::SCENE_ONE:     // 등장 씬 가까워지기
        {
            _vector NewCameraPos = vAndriusPosition;
            m_fMargin -= fTimeDelta * m_fCameraSpeed;
            m_pTransformCom->Set_State(CTransform::STATE_POSITION, NewCameraPos + (vAndriusRight * 1.5f) + (vAndriusUp * 2.f) + (vAndriusLook * m_fMargin));
            m_pTransformCom->Look_At_Position(vAndriusPosition);
        }
            break;
        case Client::SCENE_TWO:     // 등장 씬 멀어지기
        {
            _vector CameraPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
            CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
            _int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

            CTransform* pPlayerTransform =
                static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

            if (pPlayerTransform == nullptr)
                return E_FAIL;
            
            _vector vCurrentPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
            _vector vPlayerPosition = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
            vPlayerPosition.m128_f32[1] += 1.5f;

            _vector vCameraPosition = m_pArmTransformCom->Get_State(CTransform::STATE_LOOK);

            vCameraPosition = XMVector3Normalize(vCameraPosition) * m_fRadius;
            _vector vTargetPosition = vPlayerPosition + vCameraPosition;
            //m_vFirstCamPos += m_vFirstCamRot;
            _float LerpFactort = 0.06f;
            _vector vFinalPosition = XMVectorLerp(m_vFirstCamPos, vTargetPosition, LerpFactort);
            //_vector vFinalPosition = XMVectorLerp(vCurrentPosition, vTargetPosition, LerpFactort);
            m_pTransformCom->Set_State(CTransform::STATE_POSITION, vFinalPosition);
            m_pTransformCom->Look_At_Position(vAndriusPosition);
        }
            break;
        case Client::SCENE_THREE:       // 2페이즈 포효씬
        {
            _vector NewCameraPos = vAndriusPosition;
            NewCameraPos += XMVectorSet(0.f, vAndriusPosition.m128_f32[1] + 2.f, 0.f, 0.f); 
            m_pTransformCom->Set_State(CTransform::STATE_POSITION, NewCameraPos + (vAndriusRight * 6.f) + (vAndriusLook * 8.f));
            m_pTransformCom->Look_At_Position(vAndriusPosition + (vAndriusRight * 2.f) + (vAndriusUp * 2.f) + (vAndriusLook * 5.f));
        }
            break;
        case Client::SCENE_FOUR:        // 2페이지 포효 동안 멀어지기
        {
            //_vector CameraPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

            _vector vCurrentPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
            _vector vMovePosition = { vCurrentPosition + (vAndriusRight * 3.f) + (vAndriusLook * 1.f) };
            //_vector vMovePosition = { vCurrentPosition.m128_f32[0] - 1.5f, vCurrentPosition.m128_f32[1], vCurrentPosition.m128_f32[2] + 1.5f, 1.f};
            _vector vCameraPosition = m_pArmTransformCom->Get_State(CTransform::STATE_LOOK);

            //vCameraPosition = XMVector3Normalize(vCameraPosition) * m_fRadius;
            //_vector vTargetPosition = vMovePosition + vCameraPosition;
            _vector vTargetPosition = vMovePosition;

            _float LerpFactort = 0.01f;
            _vector vFinalPosition = XMVectorLerp(vCurrentPosition, vTargetPosition, LerpFactort);
            m_pTransformCom->Set_State(CTransform::STATE_POSITION, vFinalPosition);
            //m_pTransformCom->Look_At_Position(vAndriusPosition);
        }
            break;
        default:
            break;
        }
    }

    return S_OK;
}

HRESULT CCamera_Main::Dvalin_CutScene(_float fTimeDelta)
{
    switch (m_eSceneNum)
    {
    case SCENE_ONE:
    {
        _vector vCameraPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        CTransform* pDvalinTransform = 
            static_cast<CTransform*>(m_pGameInstance
                ->Find_Layer_Component(m_pGameInstance->Get_CurrentLevel(), L"Layer_Boss", L"Com_Transform"));
        if (pDvalinTransform == nullptr)
            return E_FAIL;
        _vector vDvalinPos = pDvalinTransform->Get_State(CTransform::STATE_POSITION);
        _vector vDvalinLook = pDvalinTransform->Get_State(CTransform::STATE_LOOK);
        _vector vDvalinRight = pDvalinTransform->Get_State(CTransform::STATE_RIGHT);
        vDvalinLook.m128_f32[1] = 0.f; vDvalinLook = XMVector4Normalize(vDvalinLook);
        vDvalinRight.m128_f32[1] = 0.f; vDvalinRight = XMVector4Normalize(vDvalinRight);

        vDvalinPos.m128_f32[1] += 15.f;

        if (m_fMargin > 0.f)
            m_fMargin -= fTimeDelta * m_fCameraSpeed;
        else
            m_fMargin = 0.f;

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vDvalinPos - vDvalinRight * m_fMargin);
        vCameraPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCameraPos + vDvalinLook * 50.f);
        
        vDvalinPos.m128_f32[1] += 15.f;
        m_pTransformCom->Look_At_Position(vDvalinPos);
    }
        break;
    case SCENE_TWO:
    {
        _vector vCameraPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        CTransform* pDvalinTransform =
            static_cast<CTransform*>(m_pGameInstance
                ->Find_Layer_Component(m_pGameInstance->Get_CurrentLevel(), L"Layer_Boss", L"Com_Transform"));
        if (pDvalinTransform == nullptr)
            return E_FAIL;
        _vector vDvalinPos = pDvalinTransform->Get_State(CTransform::STATE_POSITION);
        _vector vDvalinLook = pDvalinTransform->Get_State(CTransform::STATE_LOOK);
        vDvalinLook.m128_f32[1] = 0.f; vDvalinLook = XMVector4Normalize(vDvalinLook);
        vDvalinPos.m128_f32[1] += 15.f;

        if (m_fMargin < 60.f)
            m_fMargin += fTimeDelta * m_fCameraSpeed;
        else
        {
            m_fMargin = 60.f;
            m_bEndScene = TRUE;
        }

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vDvalinPos + vDvalinLook * (50.f + m_fMargin));

        vDvalinPos.m128_f32[1] += 15.f;
        m_pTransformCom->Look_At_Position(vDvalinPos);
    }
    break;
    case SCENE_THREE:
    {
        _vector vCameraPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        CTransform* pDvalinTransform =
            static_cast<CTransform*>(m_pGameInstance
                ->Find_Layer_Component(m_pGameInstance->Get_CurrentLevel(), L"Layer_Boss", L"Com_Transform"));
        if (pDvalinTransform == nullptr)
            return E_FAIL;
        _vector vDvalinPos = pDvalinTransform->Get_State(CTransform::STATE_POSITION);
        _vector vDvalinLook = pDvalinTransform->Get_State(CTransform::STATE_LOOK);
        vDvalinLook.m128_f32[1] = 0.f; vDvalinLook = XMVector4Normalize(vDvalinLook);
        vDvalinPos.m128_f32[1] += 10.f;

        if (m_fMargin < 25.f)
            m_fMargin += fTimeDelta * m_fCameraSpeed;
        else
            m_fMargin = 25.f;

        vCameraPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vDvalinPos + vDvalinLook * 90.f);

        vDvalinPos.m128_f32[1] += 15.f + m_fMargin;
        m_pTransformCom->Look_At_Position(vDvalinPos);
    }
        break;
    case SCENE_FOUR:
    {
        _vector vCameraPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        CTransform* pDvalinTransform =
            static_cast<CTransform*>(m_pGameInstance
                ->Find_Layer_Component(m_pGameInstance->Get_CurrentLevel(), L"Layer_Boss", L"Com_Transform"));
        if (pDvalinTransform == nullptr)
            return E_FAIL;
        _vector vDvalinPos = pDvalinTransform->Get_State(CTransform::STATE_POSITION);
        _vector vDvalinLook = pDvalinTransform->Get_State(CTransform::STATE_LOOK);
        _vector vDvalinRight = pDvalinTransform->Get_State(CTransform::STATE_RIGHT);
        vDvalinLook.m128_f32[1] = 0.f; vDvalinLook = XMVector4Normalize(vDvalinLook);
        vDvalinRight.m128_f32[1] = 0.f; vDvalinRight = XMVector4Normalize(vDvalinRight);

        if (m_fMargin < 50.f)
            m_fMargin += fTimeDelta * m_fCameraSpeed;
        else
            m_fMargin = 50.f;

        vDvalinPos.m128_f32[1] += 25.f - m_fMargin / 5.f;

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vDvalinPos + vDvalinRight * m_fMargin / 5.f);
        vCameraPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCameraPos + vDvalinLook * (90.f - m_fMargin / 5.f));

        vDvalinPos.m128_f32[1] += 15.f - m_fMargin / 1.3f;
        vDvalinPos += vDvalinLook * (70.f + m_fMargin / 5.f);
        vDvalinPos -= vDvalinRight * 15.f;
        m_pTransformCom->Look_At_Position(vDvalinPos);
    }
    break;
    case SCENE_FIVE:
    {
        _vector vCameraPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        CTransform* pDvalinTransform =
            static_cast<CTransform*>(m_pGameInstance
                ->Find_Layer_Component(m_pGameInstance->Get_CurrentLevel(), L"Layer_Boss", L"Com_Transform"));
        if (pDvalinTransform == nullptr)
            return E_FAIL;
        _vector vDvalinPos = pDvalinTransform->Get_State(CTransform::STATE_POSITION);
        _vector vDvalinLook = pDvalinTransform->Get_State(CTransform::STATE_LOOK);
        vDvalinLook.m128_f32[1] = 0.f; vDvalinLook = XMVector4Normalize(vDvalinLook);
        vDvalinPos.m128_f32[1] += 15.f;

        if (m_fMargin < 30.f)
            m_fMargin += fTimeDelta * m_fCameraSpeed;
        else
            m_fMargin = 30.f;

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vDvalinPos + vDvalinLook * (100.f + m_fMargin / 3.f));

        vDvalinPos.m128_f32[1] -= m_fMargin / 5.f;
        vDvalinPos += vDvalinLook * (80.f + m_fMargin / 5.f);
        m_pTransformCom->Look_At_Position(vDvalinPos);
    }
    break;
    }
    return S_OK;
}

HRESULT CCamera_Main::BasicTalk_Scene(_float fTimeDelta)
{
    switch (m_eSceneNum)
    {
    case SCENE_ONE:
    {
        _vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        _vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
        vLook.m128_f32[1] = 0.f; vLook = XMVector4Normalize(vLook);

        if (m_fMargin < 3.f)
        {
            m_fMargin += fTimeDelta * m_fCameraSpeed;
            m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos + vLook * 0.07f - XMVectorSet(0.f,0.04f,0.f,0.f));
        }
        else
            m_fMargin = 3.f;
    }
    break;

    case SCENE_TWO:
    {
        _vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        _vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
        vLook.m128_f32[1] = 0.f; vLook = XMVector4Normalize(vLook);

        if (m_fMargin > 0.f)
        {
            m_fMargin -= fTimeDelta * m_fCameraSpeed;
            m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos - vLook * 0.07f + XMVectorSet(0.f, 0.04f, 0.f, 0.f));
        }
        else
        {
            m_fMargin = 0.f;
            m_bEndScene = TRUE;
        }
    }
    break;
    }
    return S_OK;
}

HRESULT CCamera_Main::AmborTalk_Scene(_float fTimeDelta)
{
    switch (m_eSceneNum)
    {
    case SCENE_ONE: /* 엠버, 플레이어 둘 다 화면에 잡힘(카메라 서서히 내려온다) */
    {
        _vector vCameraPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        CTransform* pAmborTransform =
            static_cast<CTransform*>(m_pGameInstance
                ->Find_Layer_Component(m_pGameInstance->Get_CurrentLevel(), L"Layer_Ambor", L"Com_Transform"));
        if (pAmborTransform == nullptr)
            return E_FAIL;
        _vector vAmborPos = pAmborTransform->Get_State(CTransform::STATE_POSITION);
        if (m_bInitDir == FALSE)
        {
            m_vInitDir = pAmborTransform->Get_State(CTransform::STATE_RIGHT);
            m_bInitDir = TRUE;
        }
        _vector vAmborLook = pAmborTransform->Get_State(CTransform::STATE_LOOK);
        _vector vCameraRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);

        m_vInitDir = XMVector3Normalize(m_vInitDir);
        //vAmborRight = {1.f, 0.f, 0.f, 0.f};

        if (m_fMargin > 0.f)
            m_fMargin -= fTimeDelta * m_fCameraSpeed;
        else
            m_fMargin;
        
        vAmborPos.m128_f32[1] += 1.9f + m_fMargin;

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vAmborPos + m_vInitDir * 2.6f);
        vAmborPos.m128_f32[1] -= 0.1f;
        m_pTransformCom->Look_At_Position(vAmborPos);

        vCameraPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCameraPos + XMVector3Normalize(vAmborLook) * 1.4f);
    }
    break;

    case SCENE_TWO: /* 엠버 얼빡씬 */
    {
        _vector vCameraPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        CTransform* pAmborTransform =
            static_cast<CTransform*>(m_pGameInstance
                ->Find_Layer_Component(m_pGameInstance->Get_CurrentLevel(), L"Layer_Ambor", L"Com_Transform"));
        if (pAmborTransform == nullptr)
            return E_FAIL;

        _vector vAmborPos = pAmborTransform->Get_State(CTransform::STATE_POSITION);
        if (m_bInitDir == FALSE)
        {
            m_vInitDir = pAmborTransform->Get_State(CTransform::STATE_LOOK);
            m_bInitDir = TRUE;
        }

        m_vInitDir = XMVector3Normalize(m_vInitDir);
        vAmborPos.m128_f32[1] += 1.4f;

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vAmborPos + m_vInitDir * m_fMargin);
        vAmborPos.m128_f32[1] -= 0.1f;
        m_pTransformCom->Look_At_Position(vAmborPos);
    }
    break;
    
    case SCENE_THREE: /* 페이몬 얼빡씬 */
    {
        _vector vCameraPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        CTransform* pPaimonTransform =
            static_cast<CTransform*>(m_pGameInstance
                ->Find_Layer_Component(m_pGameInstance->Get_CurrentLevel(), L"Layer_Paimon", L"Com_Transform"));
        if (pPaimonTransform == nullptr)
            return E_FAIL;

        _vector vPaimonPos = pPaimonTransform->Get_State(CTransform::STATE_POSITION);
        _vector vPaimonLook = pPaimonTransform->Get_State(CTransform::STATE_LOOK);

        vPaimonLook = XMVector3Normalize(vPaimonLook);
        vPaimonPos.m128_f32[1] += 0.7f;

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPaimonPos + vPaimonLook * m_fMargin);
        vPaimonPos.m128_f32[1] -= 0.1f;
        m_pTransformCom->Look_At_Position(vPaimonPos);
    }
    break;

    case SCENE_FOUR: /* 엠버, 플레이어 둘 다 화면에 잡힘 */
    {
        _vector vCameraPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        CTransform* pAmborTransform =
            static_cast<CTransform*>(m_pGameInstance
                ->Find_Layer_Component(m_pGameInstance->Get_CurrentLevel(), L"Layer_Ambor", L"Com_Transform"));
        if (pAmborTransform == nullptr)
            return E_FAIL;
        _vector vAmborPos = pAmborTransform->Get_State(CTransform::STATE_POSITION);
        _vector vAmborRight = pAmborTransform->Get_State(CTransform::STATE_RIGHT);
        _vector vAmborLook = pAmborTransform->Get_State(CTransform::STATE_LOOK);
        _vector vCameraRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);

        vAmborRight = XMVector3Normalize(vAmborRight);
        vAmborPos.m128_f32[1] += 1.3f + m_fMargin;

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vAmborPos + vAmborRight * 2.5f);
        vAmborPos.m128_f32[1] -= 0.1f;
        m_pTransformCom->Look_At_Position(vAmborPos);

        vCameraPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCameraPos + XMVector3Normalize(vAmborLook) * 1.f);
    }
    break;
    case SCENE_FIVE: /* 엠버 측면씬 */
    {
        _vector vCameraPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        CTransform* pAmborTransform =
            static_cast<CTransform*>(m_pGameInstance
                ->Find_Layer_Component(m_pGameInstance->Get_CurrentLevel(), L"Layer_Ambor", L"Com_Transform"));
        if (pAmborTransform == nullptr)
            return E_FAIL;

        _vector vAmborPos = pAmborTransform->Get_State(CTransform::STATE_POSITION);
        _vector vAmborLook = pAmborTransform->Get_State(CTransform::STATE_LOOK);
        _vector vAmborRight = pAmborTransform->Get_State(CTransform::STATE_RIGHT);

        vAmborRight = XMVector3Normalize(vAmborRight);
        vAmborLook = XMVector3Normalize(vAmborLook);
        vAmborPos.m128_f32[1] += 1.4f;

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vAmborPos + vAmborLook * m_fMargin);
        vAmborPos.m128_f32[1] -= 0.1f;

        vCameraPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCameraPos + vAmborRight * 0.5f);
        m_pTransformCom->Look_At_Position(vAmborPos);

    }
    break;
    case SCENE_SIX: /* 페이몬 측면씬 */
    {
        _vector vCameraPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        CTransform* pPaimonTransform =
            static_cast<CTransform*>(m_pGameInstance
                ->Find_Layer_Component(m_pGameInstance->Get_CurrentLevel(), L"Layer_Paimon", L"Com_Transform"));
        if (pPaimonTransform == nullptr)
            return E_FAIL;

        _vector vPaimonPos = pPaimonTransform->Get_State(CTransform::STATE_POSITION);
        _vector vPaimonLook = pPaimonTransform->Get_State(CTransform::STATE_LOOK);
        _vector vPaimonRight = pPaimonTransform->Get_State(CTransform::STATE_RIGHT);

        vPaimonRight = XMVector3Normalize(vPaimonRight);
        vPaimonLook = XMVector3Normalize(vPaimonLook);
        vPaimonPos.m128_f32[1] += 0.7f;

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPaimonPos + vPaimonLook * m_fMargin);
        vPaimonPos.m128_f32[1] -= 0.1f;

        vCameraPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCameraPos - vPaimonRight * 0.5f);
        m_pTransformCom->Look_At_Position(vPaimonPos);

    }
    break;
    case SCENE_SEVEN: /* 엠버, 플레이어 둘 다 화면에 잡힘(왼쪽) */
    {
        _vector vCameraPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        CTransform* pAmborTransform =
            static_cast<CTransform*>(m_pGameInstance
                ->Find_Layer_Component(m_pGameInstance->Get_CurrentLevel(), L"Layer_Ambor", L"Com_Transform"));
        if (pAmborTransform == nullptr)
            return E_FAIL;
        _vector vAmborPos = pAmborTransform->Get_State(CTransform::STATE_POSITION);
        _vector vAmborRight = pAmborTransform->Get_State(CTransform::STATE_RIGHT);
        _vector vAmborLook = pAmborTransform->Get_State(CTransform::STATE_LOOK);
        _vector vCameraRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);

        vAmborRight = XMVector3Normalize(vAmborRight);
        vAmborPos.m128_f32[1] += 1.3f + m_fMargin;

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vAmborPos - vAmborRight * 2.5f);
        vAmborPos.m128_f32[1] -= 0.1f;
        m_pTransformCom->Look_At_Position(vAmborPos);

        vCameraPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCameraPos + XMVector3Normalize(vAmborLook) * 1.f);
    }
    break;
    }
    return S_OK;
}

HRESULT CCamera_Main::Gate_CutScene(_float fTimeDelta)
{
    switch (m_eSceneNum)
    {
    case SCENE_ONE:
    {
        _vector vCameraPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        _vector vGatePos = m_pRefTransform->Get_State(CTransform::STATE_POSITION);
        _vector vGateRight = m_pRefTransform->Get_State(CTransform::STATE_RIGHT);
        vGateRight.m128_f32[1] = 0.f; vGateRight = XMVector4Normalize(vGateRight);
        vGatePos.m128_f32[1] += 3.f;

        if (m_fMargin < 3.f)
            m_fMargin += fTimeDelta * m_fCameraSpeed;
        else
            m_fMargin = 3.f;

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vGatePos - vGateRight * (m_fMargin + 8.f));
        //vGatePos.m128_f32[1] -= 0.1f;
        m_pTransformCom->Look_At_Position(vGatePos);
    }
        break;
    }

    return S_OK;
}

HRESULT CCamera_Main::Dungeon_CutScene(_float fTimeDelta)
{
    switch (m_eSceneNum)
    {
    case SCENE_ONE:
    {
        _vector vCameraPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        _vector vGatePos = m_pRefTransform->Get_State(CTransform::STATE_POSITION);
        _vector vGateLook = m_pRefTransform->Get_State(CTransform::STATE_LOOK);
        vGateLook.m128_f32[1] = 0.f; vGateLook = XMVector4Normalize(vGateLook);
        vGatePos.m128_f32[1] += 5.7f;

        if (m_fMargin < 3.f)
            m_fMargin += fTimeDelta * m_fCameraSpeed;
        else
            m_fMargin = 3.f;

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vGatePos + vGateLook * (m_fMargin + 5.f));
        m_pTransformCom->Look_At_Position(vGatePos);
    }
    break;
    }
    return S_OK;
}

HRESULT CCamera_Main::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamera_Main::Initialize_Clone(void* pArg)
{
    CAMERAMAIN_DESC* pCameraMainDesc = static_cast<CAMERAMAIN_DESC*>(pArg);

    if (FAILED(__super::Initialize_Clone(&pCameraMainDesc->tCameraDesc)))
        return E_FAIL;

    if (FAILED(__super::LateTick(0.0f)))
        return E_FAIL;

    if (FAILED(Add_Component(pArg)))
        return E_FAIL;

    m_fMouseSensor = pCameraMainDesc->tCameraInfo.fMouseSensor;

    m_fCurrAngle = CUSTOM_ANGLE * -0.5f;
    m_pArmTransformCom->Turn(m_pArmTransformCom->Get_State(CTransform::STATE_RIGHT), m_fCurrAngle);

    return S_OK;
}

HRESULT CCamera_Main::PreviousTick(_float fTimeDelta)
{
    if (m_bActivated == TRUE)
    {
        if (FAILED(__super::PreviousTick(fTimeDelta)))
            return E_FAIL;

        if (m_bShakingMode == TRUE) /* Shaking Mode */
        {
            Shaking_Camera(fTimeDelta);
            return S_OK;
        }

        if (m_bSceneMode == TRUE) /* Scene Mode */
        {
            SceneCamera(fTimeDelta);
            return S_OK;
        }

        //if (m_bLerpMode == TRUE) /* Lerp Mode */
        //{
        //    Lerp_Camera(fTimeDelta);
        //    return S_OK;
        //}

        if (m_pGameInstance->Get_CurrentLevel() != LEVEL_STORMTERROR)
        {
            if (m_bActivated_Move == TRUE && m_pGameInstance->Key_Pressing(VK_MENU) == FALSE)
            {
                _long	MouseMove = 0;
                if (MouseMove = m_pGameInstance->Mouse_Move(MMS_X))
                {
                    m_pArmTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), (MouseMove * m_fMouseSensor * fTimeDelta) /** -1.f*/);
                }

                if (MouseMove = m_pGameInstance->Mouse_Move(MMS_Y))
                {
                    _float fNewAngle = MouseMove * m_fMouseSensor * fTimeDelta;
                    if ((m_fCurrAngle + fNewAngle) > -CUSTOM_ANGLE * 1.75f && (m_fCurrAngle + fNewAngle) < CUSTOM_ANGLE * -0.1f)
                    {
                        m_fCurrAngle += fNewAngle;
                        m_pArmTransformCom->Turn(m_pArmTransformCom->Get_State(CTransform::STATE_RIGHT), fNewAngle * -1.f);
                    }
                }
            }
        }
        else
        {
            m_pArmTransformCom->Look_At_Position({ 0.f,0.f,0.f,1.f });
        }
        

#ifdef _DEBUG

        if (m_bFreeCamera == TRUE)
        {
            if (m_pGameInstance->Key_Pressing(VK_UP))
            {
                m_pFreeTransformCom->Go_Straight(fTimeDelta);
            }
            if (m_pGameInstance->Key_Pressing(VK_DOWN))
            {
                m_pFreeTransformCom->Go_Backward(fTimeDelta);
            }
            if (m_pGameInstance->Key_Pressing(VK_LEFT))
            {
                m_pFreeTransformCom->Go_Left(fTimeDelta);
            }
            if (m_pGameInstance->Key_Pressing(VK_RIGHT))
            {
                m_pFreeTransformCom->Go_Right(fTimeDelta);
            }

            if (m_pGameInstance->Key_Pressing(VK_LBUTTON))
            {
                _long	MouseMove = 0;
                if (MouseMove = m_pGameInstance->Mouse_Move(MMS_X))
                {
                    m_pFreeTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), MouseMove * m_fMouseSensor * fTimeDelta);
                }

                if (MouseMove = m_pGameInstance->Mouse_Move(MMS_Y))
                {
                    m_pFreeTransformCom->Turn(m_pFreeTransformCom->Get_State(CTransform::STATE_RIGHT), MouseMove * m_fMouseSensor * fTimeDelta);
                }
            }
        }

#endif
    }

    return S_OK;
}

HRESULT CCamera_Main::Tick(_float fTimeDelta)
{
    if (m_bActivated == TRUE)
    {
        if (FAILED(__super::Tick(fTimeDelta)))
            return E_FAIL;


        if (m_bActionMode == TRUE)  /* Action Mode */
        {
            Action_Camera(fTimeDelta);
        }

        CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
        _int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

        CTransform* pPlayerTransform =
            static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

        if (m_pGameInstance->Get_CurrentLevel() != LEVEL_STORMTERROR)
        {
            if (m_bShakingMode == FALSE && m_bSceneMode == FALSE && m_bLerpMode == FALSE && pPlayerTransform != nullptr)
            {
                _float3 vArmPosition;
                _vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
                XMStoreFloat3(&vArmPosition, vPlayerPos);

                vArmPosition.y += 1.5f;

                _float3 vArmLook;
                XMStoreFloat3(&vArmLook, m_pArmTransformCom->Get_State(CTransform::STATE_LOOK));

                _float3 vHitPoint;
                PxQueryFilterData tQueryFilterData = PxQueryFilterData();
                tQueryFilterData.flags = PxQueryFlag::eSTATIC;

                _float4 vCamPosition;

                if (m_pGameInstance->RayCast_Position(vArmPosition, vArmLook, m_fRadius, &vHitPoint, tQueryFilterData))
                {
                    _float4 vPosition = _float4(vHitPoint.x, vHitPoint.y, vHitPoint.z, 1.0f);
                    XMStoreFloat4(&vCamPosition, XMLoadFloat4(&vPosition));
                }
                else
                {
                    XMStoreFloat4(&vCamPosition, XMLoadFloat3(&vArmPosition) + XMLoadFloat3(&vArmLook) * m_fRadius);
                    vCamPosition.w = 1.0f;
                }

                m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vCamPosition));
                _float4 vRotateDir = _float4(vArmPosition.x, vArmPosition.y, vArmPosition.z, 1.0f);
                m_pTransformCom->Look_At_Position(XMLoadFloat4(&vRotateDir));
            }
        }
        else
        {
            if (m_bShakingMode == FALSE && m_bSceneMode == FALSE && m_bLerpMode == FALSE && pPlayerTransform != nullptr)
            {
                _float3 vArmPosition;
                _vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
                XMStoreFloat3(&vArmPosition, vPlayerPos);

                vArmPosition.y += 2.5f;

                _float3 vArmLook;
                XMStoreFloat3(&vArmLook, m_pArmTransformCom->Get_State(CTransform::STATE_LOOK));

                PxQueryFilterData tQueryFilterData = PxQueryFilterData();
                tQueryFilterData.flags = PxQueryFlag::eSTATIC;

                _float4 vCamPosition;

                _vector vPos = { 0.f,0.f,0.f,1.f };
                _vector vSetDir = vPos - vPlayerPos;
                vSetDir.m128_f32[1] = 0.f; vSetDir = XMVector4Normalize(vSetDir);

                XMStoreFloat4(&vCamPosition, XMLoadFloat3(&vArmPosition) - vSetDir /*XMLoadFloat3(&vArmLook)*/ * (m_fRadius + 3.5f));
                vCamPosition.w = 1.0f;

                m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vCamPosition));
                _float4 vRotateDir = _float4(vArmPosition.x, vArmPosition.y, vArmPosition.z, 1.0f);
                m_pTransformCom->Look_At_Position({0.f,0.f,0.f,1.f});
            }
        }

#ifdef _DEBUG

        if (m_bFreeCamera == TRUE)
        {
            m_pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, m_pFreeTransformCom->Get_WorldFloat4x4_Inverse());
        }
        else
        {
            m_pFreeTransformCom->Set_WorldFloat4x4(m_pTransformCom->Get_WorldFloat4x4());
            m_pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransformCom->Get_WorldFloat4x4_Inverse());
        }

#endif

#ifndef _DEBUG
        m_pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransformCom->Get_WorldFloat4x4_Inverse());
#endif
    }

    if (m_pGameInstance->Key_Down('Z'))
        m_bDebugRender = !m_bDebugRender;

    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

    return S_OK;
}

HRESULT CCamera_Main::LateTick(_float fTimeDelta)
{
    if (m_bActivated == TRUE)
    {
        if (FAILED(__super::LateTick(fTimeDelta)))
            return E_FAIL;
    }

#ifdef _DEBUG
    if (true == m_bDebugRender)
        m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif

    return S_OK;
}

HRESULT CCamera_Main::Render()
{
    return S_OK;
}

CCamera_Main* CCamera_Main::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCamera_Main* pInstance = new CCamera_Main(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CCamera_Main"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCamera_Main::Clone(void* pArg)
{
    CCamera_Main* pInstance = new CCamera_Main(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CCamera_Main"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCamera_Main::Free()
{
    __super::Free();

    Safe_Release(m_pTransformCom);
    Safe_Release(m_pArmTransformCom);
    Safe_Release(m_pColliderCom);

#ifdef _DEBUG
    Safe_Release(m_pFreeTransformCom);
    Safe_Release(m_pRendererCom);
#endif
}
