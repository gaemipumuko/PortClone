#include "stdafx.h"
#include "Camera_Char.h"

#include "GameInstance.h"

CCamera_Char::CCamera_Char(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Char::CCamera_Char(const CCamera_Char& rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Char::Add_Component(void* pArg)
{
	/* For.Com_Transform */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;
	m_pTransformCom->Set_Desc({ 10.f, XMConvertToRadians(90.f) });

	_float4x4 IdentityMat = { };
	XMStoreFloat4x4(&IdentityMat, XMMatrixIdentity());
	m_pTransformCom->Set_WorldFloat4x4(IdentityMat);

	return S_OK;
}

HRESULT CCamera_Char::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Char::Initialize_Clone(void* pArg)
{
	CAMERACHAR_DESC* pCameraCharDesc = static_cast<CAMERACHAR_DESC*>(pArg);

	if (FAILED(__super::Initialize_Clone(&pCameraCharDesc->tCameraDesc)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	m_fMouseSensor = pCameraCharDesc->tCameraInfo.fMouseSensor;

	return S_OK;
}

HRESULT CCamera_Char::PreviousTick(_float fTimeDelta)
{
	if (m_bActivated == TRUE)
	{
		if (FAILED(__super::PreviousTick(fTimeDelta)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CCamera_Char::Tick(_float fTimeDelta)
{
	if (m_bActivated == TRUE)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, { 0.f, 1.1f, 2.f, 1.f });
		m_pTransformCom->Look_At_Position({ 0.f, 1.f, 0.f, 1.f });
	}

	if (m_bActivated_Weapon == TRUE)
	{

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, { 0.f, 1.4f, 1.f, 1.f });
		m_pTransformCom->Look_At_Position({ 0.f, 1.4f, 0.f, 1.f });

		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, { 0.f, 1.6f, 0.8f, 1.f });
		//m_pTransformCom->Look_At_Position({ 0.f, 1.65f, 0.f, 1.f });
	}

	//if (m_bActivated == TRUE)
	//{
	//	// 현재 위치
	//	_vector currentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//	// 목표 위치
	//	_vector targetPos = { 0.f, 1.1f, 2.f, 1.f };

	//	// 보간에 사용할 t 값 계산 (예: 0.1초에 걸쳐 이동)
	//	_float t = fTimeDelta / 0.1f;
	//	// 선형 보간을 이용하여 현재 위치에서 목표 위치까지 서서히 이동
	//	_vector lerpedPos = { Lerp(currentPos, targetPos, t)};

	//	m_pTransformCom->Set_State(CTransform::STATE_POSITION, lerpedPos);
	//	m_pTransformCom->Look_At_Position({ 0.f, 1.1f, 0.f, 1.f });
	//}

	//if (m_bActivated_Weapon == TRUE)
	//{
	//	// 현재 위치
	//	_vector currentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//	// 목표 위치
	//	_vector targetPos = { 0.f, 1.1f, 2.f, 1.f };

	//	// 보간에 사용할 t 값 계산 (예: 0.1초에 걸쳐 이동)
	//	_float t = fTimeDelta / 2.f;
	//	// 선형 보간을 이용하여 현재 위치에서 목표 위치까지 서서히 이동
	//	_vector lerpedPos = { Lerp(currentPos, targetPos, t) };

	//	m_pTransformCom->Set_State(CTransform::STATE_POSITION, lerpedPos);
	//	m_pTransformCom->Look_At_Position({ 0.f, 1.65f, 2.f, 1.f });
	//}

	return S_OK;
	//if (m_bActivated == TRUE)
	//{
	//	_vector targetPosition = { 0.f, 1.1f, 2.f, 1.f };
	//	_vector targetLookAt = { 0.f, 1.1f, 0.f, 1.f };

	//	m_pTransformCom->Set_State(CTransform::STATE_POSITION, targetPosition);
	//	m_pTransformCom->Look_At_Position(targetLookAt);
	//}


 //   if (m_bActivated_Weapon == TRUE)
	//{
	//	_float fInterpolation = 0.0f;
	//	_float fSpeed = 0.5f; // 적절한 속도를 설정하세요.

	//	fInterpolation += fTimeDelta * fSpeed;
	//	if (fInterpolation > 1.0f)
	//	{
	//		fInterpolation = 1.0f;
	//	}
	//	_float fY = (1.0f - fInterpolation) * 1.1f + fInterpolation * 1.65f;
	//	_float fZ = (1.0f - fInterpolation) * 2.f + fInterpolation * 1.f;

	//	_vector targetPosition = { 0.f, fY, fZ, 1.f };
	//	_vector targetLookAt = { 0.f, fY, 0.f, 1.f };

	//	m_pTransformCom->Set_State(CTransform::STATE_POSITION, targetPosition);
	//	m_pTransformCom->Look_At_Position(targetLookAt);
	//}
	//return S_OK;

  //      // 보간 시간 계산
  //      static _float fLerpTime = 0.f;
  //      fLerpTime += fTimeDelta;

  //      // Lerp를 사용하여 카메라 위치 보간
		//_vector vTargetPos = { 0.f, 1.65f, 1.f, 1.f };
		//_vector vCurrPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
  //      m_pTransformCom->Set_State(CTransform::STATE_POSITION, Lerp(vCurrPos, vTargetPos, fLerpTime));

  //      // Lerp를 사용하여 카메라 회전 보간
		//_vector vTargetLookAt = { 0.f, 1.65f, 0.f, 1.f };
		//_vector vCurrLookAt = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
  //      m_pTransformCom->Look_At_Position(Lerp(vCurrLookAt, vTargetLookAt, 0.f));
    


	

}

HRESULT CCamera_Char::LateTick(_float fTimeDelta)
{
	if (m_bActivated == TRUE)
	{
		if (FAILED(__super::LateTick(fTimeDelta)))
			return E_FAIL;

		m_pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransformCom->Get_WorldFloat4x4_Inverse());
	}

	return S_OK;
}

HRESULT CCamera_Char::Render()
{
	return S_OK;
}

CCamera_Char* CCamera_Char::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Char* pInstance = new CCamera_Char(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CCamera_Char"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Char::Clone(void* pArg)
{
	CCamera_Char* pInstance = new CCamera_Char(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CCamera_Char"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Char::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
}
