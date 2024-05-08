#pragma once

#include "Component.h"

/* Vector와 Matrix는 스택 메모리에 할당 (주소를 붙이면 안 됨) */

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

public:
	typedef struct tagTransformDesc
	{
		_float		fSpeedPerSec = { 0.f };
		_float		fRotationPerSec = { 0.f };
	}TRANSFORM_DESC;

private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

private:
	_float4x4	m_WorldMatrix = { };
	_float		m_fSpeedPerSec = { 5.f };
	_float		m_fRotationPerSec = { 5.f };

	_float		m_fInitY = { };

	/* LookAtDir Lerp */
	_bool		m_bLerpOneEighty = { FALSE };	/* 두 방향 벡터가 180도 일 때 */
	_float		m_fLerpDirRatio = { 0.f };

	_float4		m_vTargetLookDir = { };
	_float4		m_vLastLookAtDir = { };
	_float		m_fLerpSpeed = { 20.f };
	_float		m_fLerpOneEightySpeed = { 10.f };

	_bool		m_bIsSpecificRotation = { FALSE };	// 보간 껏다 켜기용
	_bool		m_bActiveYLerp = { FALSE };			// y 값 보간 유무

	/* Parabola */
	_float		m_fMaxHeightTime = { };
	_float		m_fParabolaTime = { };
	_float4		m_vParabolaDir = { };

	_float		m_fGravity = { 4.9f };
	_float		m_fInitSpeed = { 20.f };

public:
	_float		Get_LerpRatio() { return m_fLerpDirRatio; }

public:
	void		Set_WorldFloat4x4(_float4x4 WorldMatrix) { m_WorldMatrix = WorldMatrix; }
	_float4x4	Get_WorldFloat4x4() const { return m_WorldMatrix; }
	_float4x4	Get_WorldFloat4x4_Inverse();

	_matrix		Get_WorldMatrix() const { return XMLoadFloat4x4(&m_WorldMatrix); }
	_matrix		Get_WorldMatrix_Inverse() const { return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)); }
	void		Set_WorldMatrix(const _matrix& NewWorldMatrix)
	{
		XMStoreFloat4x4(&m_WorldMatrix, NewWorldMatrix);
	}

	_vector		Get_State(STATE eState);

	void		Set_State(STATE eState, _fvector vState);

	_vector		Get_Scale();
	void		Set_Scale(const _fvector& vScale);
	_matrix		Calculate_Scale(const _fvector& vScale);

	void		Set_Desc(const TRANSFORM_DESC& TransformDesc)
	{
		m_fSpeedPerSec = TransformDesc.fSpeedPerSec;
		m_fRotationPerSec = TransformDesc.fRotationPerSec;
	}
	_float		Get_SpeedPerSec() { return m_fSpeedPerSec; }
	_float		Get_RotationPerSec() { return m_fRotationPerSec; }

	void		Set_Parabola_Jump(_fvector vDirection, _float fInitSpeed, _float fGravity = 9.8f);
	void		Set_IsSpecificRotation(_bool _IsSpecificRotation) { m_bIsSpecificRotation = _IsSpecificRotation; }
	void		Set_ActiveYLerp(_bool _ActiveYLerp) { m_bActiveYLerp = _ActiveYLerp; }

public:
	void	Go_Straight(_float fTimeDelta, _fvector vSpecificDir = { }, class CNavigation* pNavigation = nullptr);
	void	Go_Backward(_float fTimeDelta, _fvector vSpecificDir = { }, class CNavigation* pNavigation = nullptr);
	void	Go_Left(_float fTimeDelta, _fvector vSpecificDir = { }, class CNavigation* pNavigation = nullptr);
	void	Go_Right(_float fTimeDelta, _fvector vSpecificDir = { }, class CNavigation* pNavigation = nullptr);
	void	Go_Up(_float fTimeDelta, _fvector vSpecificDir = { }, class CNavigation* pNavigation = nullptr);
	void	Go_Down(_float fTimeDelta, _fvector vSpecificDir = { }, class CNavigation* pNavigation = nullptr);

public:
	void	Set_TranslationPosition(_vector vTranslation);

public:
	void	Turn(_fvector vAxis, _float fTimeDelta);
	void	Rotation(_fvector vAxis, _float fRadian);
	void	RotationFromCurrentState(_fvector vAxis, _float fRadian);
	void	Rotation_Original(_fvector vAxis, _float fRadian);
	void	Rotation_Matrix(_matrix mRotationMatrix);
	void	Rotations(vector<_vector> vAxises, vector<_float> fRadians);

	_bool	Move_To(_fvector vDestPoint, _float fTimeDelta, _float fMargin = 0.1f, class CNavigation* pNavigation = nullptr);
	void	Move_To_Dir(_fvector vDestDir, _float fTimeDelta, _float fMargin = 0.1f, class CNavigation* pNavigation = nullptr);
	_bool	Move_To_Parabola_Jump(_float fTimeDelta, class CNavigation* pNavigation = nullptr, _float fMargin = 0.1f);
	_bool	Max_Height();

	_bool	Fly_To(_float fTimeDelta, _fvector vSpecificDir = { }, class CNavigation* pNavigation = nullptr);
	
	void	Look_At_Position(_fvector vTargetPoint);
	void	Look_At_Direction(_fvector vTargetDirection);
	void	Look_At_Direction_FreeRot(_fvector vTargetDirection, _fvector _vUp);

	void	Set_Look_At_Direction(_fvector vLookVector, _float fLerpSpeed = 20.f);
	void	Reset_Look_At_Direction();
	void	Verify_Look_At_Direction();

	void	Invalidate_Look_At_Lerp(_float fTimeDelta);

	void	Compare_Y(_float fY);

private: /* For. Parabola Position */
	_float	m_fTheta = { };

	_vector	m_vParabolaBegin = { };
	_vector	m_vParabolaTarget = { };

public: /* For. Parabola Position */
	void	Set_Parabola_Position(_vector vPosition);
	_bool	Move_To_Parabola_Position(_float fTimeDelta, _float fMargin);

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg) override;

public:
	static CTransform*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent*		Clone(void* pArg) override;
	virtual void			Free() override;
};

END