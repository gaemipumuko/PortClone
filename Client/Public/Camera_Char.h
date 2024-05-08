#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CCamera_Char final : public CCamera
{
public:
	typedef struct tagCameraInfo
	{
		_float					fMouseSensor = 0.f;
	}CAMERA_INFO;

	typedef struct tagCameraCharDesc
	{
		CAMERA_INFO				tCameraInfo;
		CCamera::CAMERA_DESC	tCameraDesc;
	}CAMERACHAR_DESC;

private:
	CCamera_Char(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Char(const CCamera_Char& rhs);
	virtual ~CCamera_Char() = default;

private:
	CTransform*	m_pTransformCom = { nullptr };

private:
	HRESULT Add_Component(void* pArg);

private:
	_vector Lerp(_vector fStart, _vector fEnd, _float t) { return fStart + t * (fEnd - fStart); }

private:	/* For.Main Camera */
	_float	m_fMouseSensor = { 0.f };
	_float			m_fCameraSpeed = { 0.f };
	_float			m_fMargin = { 0.f }; /* 대상과의 거리 */


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CCamera_Char*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END