#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	typedef struct tagCameraDesc
	{
		_float4		vEye, vAt;
		_float		fFovy, fAspect, fNear, fFar;
	}CAMERA_DESC;

protected:
	CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;

protected:
	_bool	m_bActivated = { FALSE };
	_bool	m_bActivated_Move = { FALSE };
	_bool	m_bActivated_Weapon = { FALSE };

public:
	_bool	Get_Activated() { return m_bActivated; }
	void	Set_Activated(_bool bActivated) { m_bActivated = bActivated; }
	void	Set_Activated_Move(_bool bActivated_Move) { m_bActivated_Move = bActivated_Move; }
	void	Set_Activated_Weapon(_bool bActivated_Weapon) { m_bActivated_Weapon = bActivated_Weapon; }

private:
	_float	m_fFovy = { 0.0f };
	_float	m_fAspect = { 0.0f };
	_float	m_fNear = { 0.0f };
	_float	m_fFar = { 0.0f };

public:
	_float4	Get_FovArNearFar() { return _float4{ m_fFovy, m_fAspect, m_fNear, m_fFar }; } // FOV, AspectRatio, NearZ, FarZ

	_float	Get_CamNear() { return m_fNear; }
	_float	Get_CamFar() { return m_fFar; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg = nullptr) override;
	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;

public:
	virtual CGameObject*	Clone(void* pArg) PURE;
	virtual void			Free() override;
};

END