 #include "stdafx.h"
#include "CharacterController.h"
#include "GameInstance.h"
#include "Transform.h"

CCharacterController::CCharacterController()
    : m_pGameInstance(CGameInstance::Get_Instance())
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CCharacterController::Initialize(_float fHeight, _float fRadius, _float fStepOffset, _float4 vPosition, CTransform* pTransformCom)
{
    m_fHeight = fHeight;
    m_fRadius = fRadius;

    m_tControllerFilterData.word0 = 1 << 0;

    m_pController = m_pGameInstance->Create_Controller(m_fHeight, m_fRadius, fStepOffset, vPosition, m_tControllerFilterData);

    if (nullptr == m_pController)
        return E_FAIL;

    m_pTransformCom = pTransformCom;
    Safe_AddRef(m_pTransformCom);

    return S_OK;
}

void CCharacterController::Movement(_float fTimeDelta, _float4 vDirection, _float fSpeed)
{
    if (true == m_bSleep)
        return;

    PxVec3 PxDirection = PxVec3(vDirection.x, vDirection.y, vDirection.z);

    PxDirection = PxDirection * fTimeDelta * fSpeed * m_fMovementSpeedControl;

    PxControllerFilters filters;

    m_pController->move(PxDirection, 0.0f, fTimeDelta, filters);

    PxExtendedVec3 PxFootPosition = m_pController->getFootPosition();

    _float4 vFootPosition;
    {
        vFootPosition.x = static_cast<_float>(PxFootPosition.x);
        vFootPosition.y = static_cast<_float>(PxFootPosition.y);
        vFootPosition.z = static_cast<_float>(PxFootPosition.z);
        vFootPosition.w = 1.0f;
    }

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vFootPosition));
}

void CCharacterController::Apply_Gravity(_float fTimeDelta)
{
    if (true == m_bSleep)
        return;

#pragma region 기본 상태 점검
    PxExtendedVec3 PxExOrigin = m_pController->getPosition();
    PxVec3 PxUnitDir = m_pController->getUpDirection() * -1.0f;

    _float3 vOrigin, vUnitDir, vHitPoint;
    _float fMaxDistance = m_fHeight * 0.5f + m_fRadius + 0.3f;
    {
        vOrigin.x = static_cast<_float>(PxExOrigin.x);
        vOrigin.y = static_cast<_float>(PxExOrigin.y);
        vOrigin.z = static_cast<_float>(PxExOrigin.z);

        vUnitDir.x = PxUnitDir.x;
        vUnitDir.y = PxUnitDir.y;
        vUnitDir.z = PxUnitDir.z;
    }

    PxQueryFilterData tQueryFilterData = PxQueryFilterData();
    //tQueryFilterData.data.word0 = m_tControllerFilterData.word0;
    tQueryFilterData.flags = PxQueryFlag::eSTATIC;

    if (m_pGameInstance->RayCast_Position(vOrigin, vUnitDir, fMaxDistance, &vHitPoint, tQueryFilterData))
    {
        m_binGround = true;

        if (false == m_bisJump) 
        {
            _float4 vPosition = _float4(vHitPoint.x, vHitPoint.y, vHitPoint.z, 1.0f);
            Set_Position(vPosition);
        }

        m_binAir = false;
        m_bisFly = false;
    }
    else
    {
        m_binGround = false;
        m_binAir = true;
    }
#pragma endregion   

    /* 점프 상태 */
    if (m_bisJump == true)
    {
        if (false == m_binAir)
            m_binAir = true;

        PxVec3 PxJumpPower = m_pController->getUpDirection() * (m_fJumpPower * fTimeDelta * 1.5f);
        PxControllerFilters filters;

        m_pController->move(PxJumpPower, 0.0f, fTimeDelta, filters);

        m_fJumpPower += m_PxGravity.y * fTimeDelta * 1.5f;

        PxExtendedVec3 PxFootPosition = m_pController->getFootPosition();
        _float4 vFootPosition;
        {
            vFootPosition.x = static_cast<_float>(PxFootPosition.x);
            vFootPosition.y = static_cast<_float>(PxFootPosition.y);
            vFootPosition.z = static_cast<_float>(PxFootPosition.z);
            vFootPosition.w = 1.0f;
        }

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vFootPosition));

        if (m_fJumpPower < 0.0f)
        {
            PxExtendedVec3 PxExOrigin = m_pController->getPosition();
            PxVec3 PxUnitDir = m_pController->getUpDirection() * -1.0f;

            _float3 vOrigin, vUnitDir, vHitPoint;
            _float fMaxDistance = m_fHeight * 0.5f + m_fRadius + 0.3f;
            {
                vOrigin.x = static_cast<_float>(PxExOrigin.x);
                vOrigin.y = static_cast<_float>(PxExOrigin.y);
                vOrigin.z = static_cast<_float>(PxExOrigin.z);

                vUnitDir.x = PxUnitDir.x;
                vUnitDir.y = PxUnitDir.y;
                vUnitDir.z = PxUnitDir.z;
            }

            PxQueryFilterData tQueryFilterData = PxQueryFilterData();
            //tQueryFilterData.data.word0 = m_tControllerFilterData.word0;
            tQueryFilterData.flags = PxQueryFlag::eSTATIC;

            if (m_pGameInstance->RayCast_Position(vOrigin, vUnitDir, fMaxDistance, &vHitPoint, tQueryFilterData))
            {
                if (true == m_bisJump)
                {
                    m_bisJump = false;
                    m_binGround = true;
                    m_fJumpPower = 0.0f;
                }
            }
        }
    }

    /* 공중에 있고 나는 상태가 아닐 때 */
    else if (m_binAir == true && m_bisFly == false && m_bisClimb == false && m_bisJump == false)
    {
        PxVec3 PxGravity = m_PxGravity * fTimeDelta;
        PxControllerFilters filters;

        m_pController->move(PxGravity, 0.0f, fTimeDelta, filters);

        PxExtendedVec3 PxFootPosition = m_pController->getFootPosition();
        _float4 vFootPosition;
        {
            vFootPosition.x = static_cast<_float>(PxFootPosition.x);
            vFootPosition.y = static_cast<_float>(PxFootPosition.y);
            vFootPosition.z = static_cast<_float>(PxFootPosition.z);
            vFootPosition.w = 1.0f;
        }

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vFootPosition));
    }

    /* 공중에 있고 날고 있을 때 */
    else if (m_binAir == true && m_bisFly == true && m_bisClimb == false && m_bisJump == false)
    {
        PxVec3 PxGravity = (m_PxGravity * 0.5f) * fTimeDelta;
        PxControllerFilters filters;

        m_pController->move(PxGravity, 0.0f, fTimeDelta, filters);

        PxExtendedVec3 PxFootPosition = m_pController->getFootPosition();
        _float4 vFootPosition;
        {
            vFootPosition.x = static_cast<_float>(PxFootPosition.x);
            vFootPosition.y = static_cast<_float>(PxFootPosition.y);
            vFootPosition.z = static_cast<_float>(PxFootPosition.z);
            vFootPosition.w = 1.0f;
        }

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vFootPosition));
    }
}

void CCharacterController::Ready_Jump(_float fForce)
{
    m_bisJump = true;
    m_binAir = true;
    m_fForce = fForce;
    m_fJumpPower = m_fForce;
}

_bool CCharacterController::isRise()
{
    if (m_fJumpPower <= 0.0f)
        return false;
    else
        return true;
}

void CCharacterController::Set_Position(_float4 vPosition)
{
    PxExtendedVec3 PxFootPosition = PxExtendedVec3(vPosition.x, vPosition.y, vPosition.z);

    m_pController->setFootPosition(PxFootPosition);

    _float4 vFootPosition;
    {
        vFootPosition.x = static_cast<_float>(PxFootPosition.x);
        vFootPosition.y = static_cast<_float>(PxFootPosition.y);
        vFootPosition.z = static_cast<_float>(PxFootPosition.z);
        vFootPosition.w = 1.0f;
    }

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vFootPosition));
}

void CCharacterController::Add_Direction(_vector Direction)
{
    if (true == m_bSleep)
        return;

    PxVec3 PxDirection = PxVec3(Direction.m128_f32[0], Direction.m128_f32[1], Direction.m128_f32[2]);

    PxControllerFilters filters;
    m_pController->move(PxDirection, 0.0f, 1.0f, filters);

    PxExtendedVec3 PxFootPosition = m_pController->getFootPosition();

    _float4 vFootPosition;
    {
        vFootPosition.x = static_cast<_float>(PxFootPosition.x);
        vFootPosition.y = static_cast<_float>(PxFootPosition.y);
        vFootPosition.z = static_cast<_float>(PxFootPosition.z);
        vFootPosition.w = 1.0f;
    }

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vFootPosition));
}

_bool CCharacterController::CanFly()
{
    PxExtendedVec3 PxExOrigin = m_pController->getPosition();
    PxVec3 PxUnitDir = m_pController->getUpDirection() * -1.0f;

    _float3 vOrigin, vUnitDir, vHitPoint;
    _float fMaxDistance = m_fHeight * 0.5f + m_fRadius + 0.3f;
    {
        vOrigin.x = static_cast<_float>(PxExOrigin.x);
        vOrigin.y = static_cast<_float>(PxExOrigin.y);
        vOrigin.z = static_cast<_float>(PxExOrigin.z);

        vUnitDir.x = PxUnitDir.x;
        vUnitDir.y = PxUnitDir.y;
        vUnitDir.z = PxUnitDir.z;
    }

    PxQueryFilterData tQueryFilterData = PxQueryFilterData();
    //tQueryFilterData.data.word0 = m_tControllerFilterData.word0;
    tQueryFilterData.flags = PxQueryFlag::eSTATIC;

    if (m_pGameInstance->RayCast_Position(vOrigin, vUnitDir, fMaxDistance, &vHitPoint, tQueryFilterData))
    {
        m_bisFly = false;
        return m_bisFly;
    }

    else
    {
        m_bisJump = false;
        m_bisFly = true;
        return m_bisFly;
    }
}

_float CCharacterController::Compute_Height()
{
    _float fHeight = 10.0f;

    PxExtendedVec3 PxExOrigin = m_pController->getPosition();
    PxVec3 PxUnitDir = m_pController->getUpDirection() * -1.0f;

    _float3 vOrigin, vUnitDir, vHitPoint;
    _float fMaxDistance = 10.0f;
    {
        vOrigin.x = static_cast<_float>(PxExOrigin.x);
        vOrigin.y = static_cast<_float>(PxExOrigin.y);
        vOrigin.z = static_cast<_float>(PxExOrigin.z);

        vUnitDir.x = PxUnitDir.x;
        vUnitDir.y = PxUnitDir.y;
        vUnitDir.z = PxUnitDir.z;
    }

    PxQueryFilterData tQueryFilterData = PxQueryFilterData();
    //tQueryFilterData.data.word0 = m_tControllerFilterData.word0;
    tQueryFilterData.flags = PxQueryFlag::eSTATIC;

    if (m_pGameInstance->RayCast_Position(vOrigin, vUnitDir, fMaxDistance, &vHitPoint, tQueryFilterData))
    {
        fHeight = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vHitPoint) - XMLoadFloat3(&vOrigin)));
    }

    return fHeight;
}

HRESULT CCharacterController::WakeUp_Controller()
{
    if (nullptr == m_pController)
        return E_FAIL;

    if (false != m_bSleep)
    {
        m_bSleep = false;

        _float4 vPosition;
        XMStoreFloat4(&vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

        PxExtendedVec3 PxPosition = PxExtendedVec3(vPosition.x, vPosition.y, vPosition.z);

        m_pController->setFootPosition(PxPosition);
    }

    return S_OK;
}

HRESULT CCharacterController::Sleep_Controller()
{
    if (nullptr == m_pController)
        return E_FAIL;

    if (true != m_bSleep) 
    {
        m_bSleep = true;
        m_pController->setFootPosition(PxExtendedVec3(0.0, 0.0, 0.0));
    }

    return S_OK;
}


CCharacterController* CCharacterController::Create(_float fHeight, _float fRadius, _float fStepOffset, _float4 vPosition, CTransform* pTransformCom)
{
    CCharacterController* pInstance = new CCharacterController();

    if (FAILED(pInstance->Initialize(fHeight, fRadius, fStepOffset, vPosition, pTransformCom)))
    {
        MSG_BOX(TEXT("Failed to Created : CCharacterController"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCharacterController::Free()
{
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pTransformCom);

    PX_RELEASE(m_pController);
}