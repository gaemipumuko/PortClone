#include "Camera.h"

#include "GameInstance.h"

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CCamera::CCamera(const CCamera& rhs)
    : CGameObject(rhs)
{
}

HRESULT CCamera::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamera::Initialize_Clone(void* pArg)
{
    if (pArg == nullptr)
        return E_FAIL;

    CAMERA_DESC* pCameraDesc = static_cast<CAMERA_DESC*>(pArg);

    m_fFovy = pCameraDesc->fFovy;
    m_fAspect = pCameraDesc->fAspect;
    m_fNear = pCameraDesc->fNear;
    m_fFar = pCameraDesc->fFar;

    return S_OK;
}

HRESULT CCamera::PreviousTick(_float fTimeDelta)
{
    return S_OK;
}

HRESULT CCamera::Tick(_float fTimeDelta)
{
    return S_OK;
}

HRESULT CCamera::LateTick(_float fTimeDelta)
{
    m_pGameInstance->Set_Transform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(m_fFovy, m_fAspect, m_fNear, m_fFar));

    return S_OK;
}

void CCamera::Free()
{
    __super::Free();
}
