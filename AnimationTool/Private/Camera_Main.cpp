#include "framework.h"
#include "Camera_Main.h"

#include "GameInstance.h"

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

    m_pTransformCom->Set_Desc({ 30.f, XMConvertToRadians(90.f) });

    return S_OK;
}

void CCamera_Main::Camera_Move(_float fTimeDelta)
{
    if (m_pGameInstance->Key_Pressing('W'))
    {
        m_pTransformCom->Go_Straight(fTimeDelta * 0.2f);
    }
    if (m_pGameInstance->Key_Pressing('S'))
    {
        m_pTransformCom->Go_Backward(fTimeDelta * 0.2f);
    }
    if (m_pGameInstance->Key_Pressing('A'))
    {
        m_pTransformCom->Go_Left(fTimeDelta * 0.2f);
    }
    if (m_pGameInstance->Key_Pressing('D'))
    {
        m_pTransformCom->Go_Right(fTimeDelta * 0.2f);
    }

    if (m_pGameInstance->Key_Pressing(VK_LBUTTON))
    {
        _long	MouseMove = 0;
        if (MouseMove = m_pGameInstance->Mouse_Move(MMS_X))
        {
            m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), MouseMove * m_fMouseSensor * fTimeDelta * 0.2f);
        }

        if (MouseMove = m_pGameInstance->Mouse_Move(MMS_Y))
        {
            m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), MouseMove * m_fMouseSensor * fTimeDelta * 0.2f);
        }
    }
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

    if (FAILED(Add_Component(pArg)))
        return E_FAIL;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pCameraMainDesc->tCameraDesc.vEye));
    m_pTransformCom->Look_At_Position(XMLoadFloat4(&pCameraMainDesc->tCameraDesc.vAt));

    m_fMouseSensor          = pCameraMainDesc->tCameraInfo.fMouseSensor;

    return S_OK;
}

HRESULT CCamera_Main::Tick(_float fTimeDelta)
{
    if (FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;

    m_pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransformCom->Get_WorldFloat4x4_Inverse());

    return S_OK;
}

HRESULT CCamera_Main::LateTick(_float fTimeDelta)
{
    if (FAILED(__super::LateTick(fTimeDelta)))
        return E_FAIL;

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
}
