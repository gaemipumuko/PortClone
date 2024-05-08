#include "Transform.h"

#include "Navigation.h"

#define ONE_EIGHTY cos(XMConvertToRadians(180.f))

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent(pDevice, pContext)
{
}

CTransform::CTransform(const CTransform& rhs)
    : CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
{
}

_float4x4 CTransform::Get_WorldFloat4x4_Inverse()
{
	_float4x4	WorldMatrixInverse;
	XMStoreFloat4x4(&WorldMatrixInverse, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));
	return WorldMatrixInverse;
}

_vector CTransform::Get_State(STATE eState)
{
	return XMLoadFloat4x4(&m_WorldMatrix).r[eState];
}

void CTransform::Set_State(STATE eState, _fvector vState)
{
	_matrix	TransformMatrix = XMLoadFloat4x4(&m_WorldMatrix);
	TransformMatrix.r[eState] = vState;
	XMStoreFloat4x4(&m_WorldMatrix, TransformMatrix);
}

_vector CTransform::Get_Scale()
{
	_float fRight	= XMVectorGetX(XMVector3Length(Get_State(STATE_RIGHT)));
	_float fUp		= XMVectorGetX(XMVector3Length(Get_State(STATE_UP)));
	_float fLook	= XMVectorGetX(XMVector3Length(Get_State(STATE_LOOK)));

	return _vector{ fRight, fUp, fLook, 0.f };
}

void CTransform::Set_Scale(const _fvector& vScale)
{
	_vector	vRight	= Get_State(STATE_RIGHT);
	_vector	vUp		= Get_State(STATE_UP);
	_vector	vLook	= Get_State(STATE_LOOK);
	
	Set_State(STATE_RIGHT,	XMVector3Normalize(vRight)	* vScale.m128_f32[0]);
	Set_State(STATE_UP,		XMVector3Normalize(vUp)		* vScale.m128_f32[1]);
	Set_State(STATE_LOOK,	XMVector3Normalize(vLook)	* vScale.m128_f32[2]);
}

_matrix CTransform::Calculate_Scale(const _fvector& vScale)
{
	_matrix	CSMatrix = XMMatrixIdentity();

	_vector	vRight	= Get_State(STATE_RIGHT);
	_vector	vUp		= Get_State(STATE_UP);
	_vector	vLook	= Get_State(STATE_LOOK);

	CSMatrix.r[0] = XMVector3Normalize(vRight)	* vScale.m128_f32[0];
	CSMatrix.r[1] = XMVector3Normalize(vUp)		* vScale.m128_f32[1];
	CSMatrix.r[2] = XMVector3Normalize(vLook)	* vScale.m128_f32[2];
	CSMatrix.r[3] = Get_State(STATE_POSITION);

	return CSMatrix;
}

void CTransform::Set_Parabola_Jump(_fvector vDirection, _float fInitSpeed, _float fGravity)
{
	m_fInitY = 0.f;
	m_fParabolaTime = 0.f;

	m_fGravity = fGravity;
	m_fInitSpeed = fInitSpeed;
	m_fMaxHeightTime = m_fInitSpeed / (2 * m_fGravity);
	XMStoreFloat4(&m_vParabolaDir, vDirection);
}

void CTransform::Go_Straight(_float fTimeDelta, _fvector vSpecificDir, class CNavigation* pNavigation)
{
	_vector		vPosition	= Get_State(STATE_POSITION);
	_vector		vLook		= (XMVector4Equal(vSpecificDir, { }) == TRUE ? Get_State(STATE_LOOK) : vSpecificDir);

	vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	if (pNavigation == nullptr ||
		pNavigation->isMove(vPosition) == TRUE)
	{
		Set_State(STATE_POSITION, vPosition);
	}
}

void CTransform::Go_Backward(_float fTimeDelta, _fvector vSpecificDir, class CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vLook = (XMVector4Equal(vSpecificDir, { }) == TRUE ? Get_State(STATE_LOOK) : vSpecificDir);

	vPosition -= XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	if (pNavigation == nullptr ||
		pNavigation->isMove(vPosition) == TRUE)
	{
		Set_State(STATE_POSITION, vPosition);
	}
}

void CTransform::Go_Left(_float fTimeDelta, _fvector vSpecificDir, class CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vRight = (XMVector4Equal(vSpecificDir, { }) == TRUE ? Get_State(STATE_RIGHT) : vSpecificDir);

	vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	if (pNavigation == nullptr ||
		pNavigation->isMove(vPosition) == TRUE)
	{
		Set_State(STATE_POSITION, vPosition);
	}
}

void CTransform::Go_Right(_float fTimeDelta, _fvector vSpecificDir, class CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vRight = (XMVector4Equal(vSpecificDir, { }) == TRUE ? Get_State(STATE_RIGHT) : vSpecificDir);

	vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	if (pNavigation == nullptr ||
		pNavigation->isMove(vPosition) == TRUE)
	{
		Set_State(STATE_POSITION, vPosition);
	}
}

void CTransform::Go_Up(_float fTimeDelta, _fvector vSpecificDir, class CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vUp = Get_State(STATE_UP);

	vPosition += XMVector3Normalize(vUp) * m_fSpeedPerSec * fTimeDelta;

	if (pNavigation == nullptr ||
		pNavigation->isMove(vPosition) == TRUE)
	{
		Set_State(STATE_POSITION, vPosition);
	}
}

void CTransform::Go_Down(_float fTimeDelta, _fvector vSpecificDir, class CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vUp = Get_State(STATE_UP);

	vPosition -= XMVector3Normalize(vUp) * m_fSpeedPerSec * fTimeDelta;

	if (pNavigation == nullptr ||
		pNavigation->isMove(vPosition) == TRUE)
	{
		Set_State(STATE_POSITION, vPosition);
	}
}

void CTransform::Set_TranslationPosition(_vector vTranslation)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vLook = Get_State(STATE_LOOK);

	vLook = XMVector3Normalize(vLook) * XMVector3Length(vTranslation);
	vPosition += vLook;

	Set_State(STATE_POSITION, vPosition);
}

/// <summary>
/// 현재 상태에서 특정각도로 틱마다 돌리기
/// </summary>
/// <param name="vAxis"></param>
/// <param name="fTimeDelta"></param>
void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		vUp = Get_State(STATE_UP);
	_vector		vLook = Get_State(STATE_LOOK);

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);

	Set_State(STATE_RIGHT, XMVector4Transform(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector4Transform(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector4Transform(vLook, RotationMatrix));
}

/// <summary>
/// 그냥 특정각도로 돌리기
/// </summary>
/// <param name="vAxis"></param>
/// <param name="fRadian"></param>
void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_vector		vScaled = Get_Scale();

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.m128_f32[0];
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.m128_f32[1];
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.m128_f32[2];

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	Set_State(STATE_RIGHT, XMVector4Transform(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector4Transform(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector4Transform(vLook, RotationMatrix));
}

/// <summary>
/// 현재 상태에서 특정 각도만큼 돌리기
/// </summary>
/// <param name="vAxis"></param>
/// <param name="fRadian"></param>
void CTransform::RotationFromCurrentState(_fvector vAxis, _float fRadian)
{
	_vector		vScaled = Get_Scale();

	_vector		vRight	= XMVector4Normalize(Get_State(STATE_RIGHT));
	_vector		vUp		= XMVector4Normalize(Get_State(STATE_UP));
	_vector		vLook	= XMVector4Normalize(Get_State(STATE_LOOK));

	// 회전 행렬을 생성한다.
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	// 회전 행렬을 적용한다.
	Set_State(STATE_RIGHT,	XMVector4Transform(vRight,	RotationMatrix) * vScaled.m128_f32[0]);
	Set_State(STATE_UP,		XMVector4Transform(vUp,		RotationMatrix) * vScaled.m128_f32[1]);
	Set_State(STATE_LOOK,	XMVector4Transform(vLook,	RotationMatrix) * vScaled.m128_f32[2]);
}

/// <summary>
/// 선생님 버전 Rotation
/// </summary>
/// <param name="vAxis"></param>
/// <param name="fRadian"></param>
void CTransform::Rotation_Original(_fvector vAxis, _float fRadian)
{
	_vector		vScaled = Get_Scale(); // 크기를 가져옴

	// 각 축을 크기와 곱하여 벡터로 설정
	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.m128_f32[0];
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.m128_f32[1];
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.m128_f32[2];

	// 주어진 축과 각도에 대한 회전 행렬을 생성
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	// 회전 변환을 수행하여 각 축의 새로운 방향을 설정
	Set_State(STATE_RIGHT, XMVector4Transform(vRight, RotationMatrix)); // 오른쪽 방향 설정
	Set_State(STATE_UP, XMVector4Transform(vUp, RotationMatrix)); // 위쪽 방향 설정
	Set_State(STATE_LOOK, XMVector4Transform(vLook, RotationMatrix)); // 정면 방향 설정
}

void CTransform::Rotation_Matrix(_matrix mRotationMatrix)
{
	// 현재 월드 행렬 가져오기
	_matrix mWorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);

	mWorldMatrix = mRotationMatrix * mWorldMatrix;

	// 회전한 로컬을 월드행렬에 저장해
	XMStoreFloat4x4(&m_WorldMatrix, mWorldMatrix);
}

void CTransform::Rotations(vector<_vector> vAxises, vector<_float> fRadians)
{
	_vector		vScaled = Get_Scale();

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.m128_f32[0];
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.m128_f32[1];
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.m128_f32[2];

	_matrix		RotationMatrix = XMMatrixIdentity();
	for (_int iAxis = 0; iAxis < vAxises.size(); ++iAxis)
	{
		RotationMatrix = RotationMatrix * XMMatrixRotationAxis(vAxises[iAxis], fRadians[iAxis]);
	}

	Set_State(STATE_RIGHT, XMVector4Transform(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector4Transform(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector4Transform(vLook, RotationMatrix));
}

_bool CTransform::Move_To(_fvector vDestPoint, _float fTimeDelta, _float fMargin, class CNavigation* pNavigation)
{
	_bool		bMove = FALSE;

	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vMoveDir = vDestPoint - vPosition;
	_float		fDistance = XMVectorGetX(XMVector3Length(vMoveDir));

	if (fDistance >= fMargin)
		vPosition += XMVector3Normalize(vMoveDir) * m_fSpeedPerSec * fTimeDelta;

	if (pNavigation == nullptr ||
		pNavigation->isMove(vPosition) == TRUE)
	{
		bMove = TRUE;
		Set_State(STATE_POSITION, vPosition);
	}

	else if (pNavigation != nullptr)
	{
		_vector vSlidingVector = pNavigation->Get_SlidingVector(vPosition, vMoveDir);

		if (fDistance >= fMargin)
			vPosition = Get_State(STATE_POSITION) + XMVector3Normalize(vSlidingVector) * m_fSpeedPerSec * fTimeDelta;

		if (pNavigation->isMove(vPosition) == TRUE)
			Set_State(STATE_POSITION, vPosition);
	}

	return bMove;
}

void CTransform::Move_To_Dir(_fvector vDestDir, _float fTimeDelta, _float fMargin, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vMoveDir = vDestDir;
	_float		fDistance = XMVectorGetX(XMVector3Length(vMoveDir));

	if (fDistance >= fMargin)
		vPosition += XMVector3Normalize(vMoveDir) * m_fSpeedPerSec * fTimeDelta;

	if (pNavigation == nullptr ||
		pNavigation->isMove(vPosition) == TRUE)
	{
		Set_State(STATE_POSITION, vPosition);
	}
}

_bool CTransform::Move_To_Parabola_Jump(_float fTimeDelta, CNavigation* pNavigation, _float fMargin)
{
	_vector		vPosition = Get_State(STATE_POSITION);

	if (m_fInitY == 0.f)
		m_fInitY = vPosition.m128_f32[1];

	m_fParabolaTime += fTimeDelta;

	vPosition += XMVector3Normalize(XMLoadFloat4(&m_vParabolaDir)) * m_fSpeedPerSec * fTimeDelta * 1.5f;
	vPosition.m128_f32[1]
		= m_fInitY + m_fInitSpeed * sin(XMConvertToRadians(90.f)) * m_fParabolaTime - m_fGravity * m_fParabolaTime * m_fParabolaTime;

	if (pNavigation == nullptr || pNavigation->isMove(vPosition) == TRUE)
	{
		Set_State(STATE_POSITION, vPosition);
	}
	else
	{
		vPosition = Get_State(STATE_POSITION);
		vPosition.m128_f32[1]
			= m_fInitY + m_fInitSpeed * sin(XMConvertToRadians(90.f)) * m_fParabolaTime - m_fGravity * m_fParabolaTime * m_fParabolaTime;
	
		Set_State(STATE_POSITION, vPosition);
	}

	if (pNavigation == nullptr || pNavigation->Get_CurrCellCenter().m128_f32[1] < vPosition.m128_f32[1])
		return FALSE;
	else
		return TRUE;
}

_bool CTransform::Max_Height()
{
	_float fEpsilon = 0.1f;

	if (fabs(m_fMaxHeightTime - m_fParabolaTime) < fEpsilon)
		return true;
	else
		return false;
}

_bool CTransform::Fly_To(_float fTimeDelta, _fvector vSpecificDir, class CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(STATE_POSITION);

	vPosition += XMVector3Normalize(vSpecificDir) * m_fSpeedPerSec * fTimeDelta;
	vPosition.m128_f32[1] -= m_fSpeedPerSec * fTimeDelta;

	if (pNavigation->isMove(vPosition) == TRUE)
	{
		Set_State(STATE_POSITION, vPosition);
	}
	else
	{
		vPosition = Get_State(STATE_POSITION);
		vPosition.m128_f32[1] -= m_fSpeedPerSec * fTimeDelta;

		Set_State(STATE_POSITION, vPosition);
	}

	if (pNavigation->Get_CurrCellCenter().m128_f32[1] > vPosition.m128_f32[1])
		return TRUE;
	else
		return FALSE;
}

void CTransform::Look_At_Position(_fvector vTargetPoint)
{
	_vector		vScaled = Get_Scale();

	_vector		vPosition = Get_State(STATE_POSITION);

	_vector		vLook = vTargetPoint - vPosition;
	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector		vUp = XMVector3Cross(vLook, vRight);

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.m128_f32[0]);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScaled.m128_f32[1]);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScaled.m128_f32[2]);
}

void CTransform::Look_At_Direction(_fvector vTargetDirection)
{
	_vector		vScaled = Get_Scale();

	_vector		vPosition = Get_State(STATE_POSITION);

	_vector		vLook = vTargetDirection;
	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector		vUp = XMVector3Cross(vLook, vRight);

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.m128_f32[0]);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScaled.m128_f32[1]);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScaled.m128_f32[2]);
}

void CTransform::Look_At_Direction_FreeRot(_fvector vTargetDirection, _fvector _vUp)
{
	_vector		vScaled = Get_Scale();

	_vector		vPosition = Get_State(STATE_POSITION);

	_vector		vLook = vTargetDirection;
	_vector		vRight = XMVector3Cross(_vUp, vLook);
	_vector		vUp = XMVector3Cross(vLook, vRight);

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.m128_f32[0]);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScaled.m128_f32[1]);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScaled.m128_f32[2]);
}

void CTransform::Set_Look_At_Direction(_fvector vLookVector, _float fLerpSpeed)
{
	XMStoreFloat4(&m_vLastLookAtDir, Get_State(STATE_LOOK));
	XMStoreFloat4(&m_vTargetLookDir, XMLoadFloat4(&m_vTargetLookDir) + vLookVector);
	m_fLerpSpeed = fLerpSpeed;
	m_fLerpOneEightySpeed = fLerpSpeed - 10;
}

void CTransform::Reset_Look_At_Direction()
{
	m_bLerpOneEighty = FALSE; 
	m_fLerpDirRatio = 0.f; 
	m_vTargetLookDir = { };
}

void CTransform::Verify_Look_At_Direction()
{
	_float fDot = XMVectorGetX(XMVector4Dot(XMLoadFloat4(&m_vTargetLookDir), XMLoadFloat4(&m_vLastLookAtDir)));
	_float fLength = XMVectorGetX(XMVector4Length(XMLoadFloat4(&m_vTargetLookDir))) * XMVectorGetX(XMVector4Length(XMLoadFloat4(&m_vLastLookAtDir)));

	if (fLength != 0.f)
	{
		_float fDegree = XMConvertToDegrees(acos(fDot / fLength));
		if (fDegree > 179.f)
			m_bLerpOneEighty = TRUE;
		else
			m_bLerpOneEighty = FALSE;
	}
}

void CTransform::Invalidate_Look_At_Lerp(_float fTimeDelta)
{
	if (m_bIsSpecificRotation)
	{
		m_vTargetLookDir = {};
		return;
	}

	if (!XMVector4Equal(XMLoadFloat4(&m_vTargetLookDir), { 0.f, 0.f, 0.f, 0.f }) &&
		(XMLoadFloat4(&m_vTargetLookDir).m128_f32[0] != 0.f || XMLoadFloat4(&m_vTargetLookDir).m128_f32[2] != 0.f))
	{
		_vector		vScaled = Get_Scale();
		_vector		vLook = XMVector4Normalize(XMLoadFloat4(&m_vTargetLookDir));
		
		if(!m_bActiveYLerp)
			vLook.m128_f32[1] = 0.f;

		if (m_bLerpOneEighty == TRUE)
		{
			_vector vNormalVector = { -vLook.m128_f32[2],  vLook.m128_f32[1], vLook.m128_f32[0], vLook.m128_f32[3] };

			m_fLerpDirRatio += fTimeDelta * m_fLerpSpeed;
			if (m_fLerpDirRatio > 2.f)
				m_fLerpDirRatio = 2.f;

			if (m_fLerpDirRatio < 1.f)
				vLook = XMVectorLerp(XMVector4Normalize(XMLoadFloat4(&m_vLastLookAtDir)), XMVector4Normalize(vNormalVector), m_fLerpDirRatio);
			else
				vLook = XMVectorLerp(XMVector4Normalize(vNormalVector), XMVector4Normalize(XMLoadFloat4(&m_vTargetLookDir)), (m_fLerpDirRatio - 1.f));
		}

		if (m_bLerpOneEighty == FALSE)
		{
			m_fLerpDirRatio += fTimeDelta * m_fLerpOneEightySpeed;
			if (m_fLerpDirRatio > 1.f)
				m_fLerpDirRatio = 1.f;

			vLook = XMVectorLerp(XMVector4Normalize(XMLoadFloat4(&m_vLastLookAtDir)), XMVector4Normalize(vLook), m_fLerpDirRatio);
		}

		_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
		_vector		vUp = XMVector3Cross(vLook, vRight);

		Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.m128_f32[0]);
		Set_State(STATE_UP, XMVector3Normalize(vUp) * vScaled.m128_f32[1]);
		Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScaled.m128_f32[2]);
	}
}

void CTransform::Compare_Y(_float fY)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	vPosition.m128_f32[1] = fY;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Set_Parabola_Position(_vector vPosition)
{
	memcpy(&m_vParabolaBegin, m_WorldMatrix.m[3], sizeof(_vector));
	m_vParabolaTarget = vPosition;
	_float fDistance = XMVectorGetX(XMVector4Length(m_vParabolaTarget - m_vParabolaBegin));

	m_fTheta = asin((m_fGravity * fDistance) / (m_fInitSpeed * m_fInitSpeed)) / 2.f;

	m_fInitY = 0.f;
	m_fParabolaTime = 0.f;
}

_bool CTransform::Move_To_Parabola_Position(_float fTimeDelta, _float fMargin)
{
	_vector	vPosition = Get_State(STATE_POSITION);

	if (m_fInitY == 0.f)
		m_fInitY = vPosition.m128_f32[1];

	m_fParabolaTime += fTimeDelta;

	_float	fLength = XMVectorGetX(XMVector4Length(m_vParabolaTarget - m_vParabolaBegin));
	_float	fCurrLength = XMVectorGetX(XMVector4Length(m_vParabolaTarget - vPosition));

	vPosition = m_vParabolaBegin + XMVector3Normalize(m_vParabolaTarget - m_vParabolaBegin) * m_fInitSpeed * cos(m_fTheta) * m_fParabolaTime;
	vPosition.m128_f32[1]
		= m_fInitY + m_fInitSpeed * sin(m_fTheta) * m_fParabolaTime * 4.f - 0.5f * m_fGravity * m_fParabolaTime * m_fParabolaTime * 4.f;

	Set_State(STATE_POSITION, vPosition);

	if (fLength < XMVectorGetX(XMVector4Length(vPosition - m_vParabolaBegin)))
		return TRUE;

	return FALSE;
}

HRESULT CTransform::Initialize_Prototype()
{
    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

    return S_OK;
}

HRESULT CTransform::Initialize_Clone(void* pArg)
{
	if (pArg != nullptr)
	{
		TRANSFORM_DESC* pDesc = static_cast<TRANSFORM_DESC*>(pArg);

		m_fSpeedPerSec = pDesc->fSpeedPerSec;
		m_fRotationPerSec = pDesc->fRotationPerSec;
	}

    return S_OK;
}

CTransform* CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTransform* pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CTransform"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CTransform::Clone(void* pArg)
{
	CTransform* pInstance = new CTransform(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CTransform"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTransform::Free()
{
	__super::Free();
}
