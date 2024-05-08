#include "Camera_Free.h"
#include "Transform.h"
#include "GameInstance.h"

CCamera_Free::CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Free::CCamera_Free(const CCamera_Free& rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Free::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Free::Initialize_Clone(void* pArg)
{
	CAMERA_FREE_DESC* pCamera_Free_Desc = (CAMERA_FREE_DESC*)pArg;

	if (FAILED(__super::Initialize_Clone(&pCamera_Free_Desc->tCameraDesc)))
		return E_FAIL;

	m_fMouseSensor = pCamera_Free_Desc->fMouseSensor;

	_float4 vEye = pCamera_Free_Desc->tCameraDesc.vEye;
	_float4 vAt = pCamera_Free_Desc->tCameraDesc.vAt;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC tTransformDesc = {};
	{
		tTransformDesc.fSpeedPerSec = 50.0f;
		tTransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	}

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &tTransformDesc)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vEye));
	m_pTransformCom->Look_At_Position(XMLoadFloat4(&vAt));

	return S_OK;
}

HRESULT CCamera_Free::PreviousTick(_float fTimeDelta)
{
	m_pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransformCom->Get_WorldFloat4x4_Inverse());
	__super::PreviousTick(fTimeDelta);

	return S_OK;
}

HRESULT CCamera_Free::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Pressing('W'))
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
	}
	if (m_pGameInstance->Key_Pressing('S'))
	{
		m_pTransformCom->Go_Backward(fTimeDelta);
	}
	if (m_pGameInstance->Key_Pressing('A'))
	{
		m_pTransformCom->Go_Left(fTimeDelta);
	}
	if (m_pGameInstance->Key_Pressing('D'))
	{
		m_pTransformCom->Go_Right(fTimeDelta);
	}

	if (m_pGameInstance->Key_Pressing(VK_RBUTTON)) 
	{
		_long iMouseMove = 0;

		if (iMouseMove = m_pGameInstance->Mouse_Move(MMS_X))
			m_pTransformCom->Turn(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), iMouseMove * m_fMouseSensor * fTimeDelta);

		if (iMouseMove = m_pGameInstance->Mouse_Move(MMS_Y))
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), iMouseMove * m_fMouseSensor * fTimeDelta);

	}

	__super::Tick(fTimeDelta);

	return S_OK;
}

HRESULT CCamera_Free::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);

	return S_OK;
}

CCamera_Free* CCamera_Free::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Free* pInstance = new CCamera_Free(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CCamera_Free"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Free::Clone(void* pArg)
{
	CCamera_Free* pInstance = new CCamera_Free(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CCamera_Free"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Free::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
}
