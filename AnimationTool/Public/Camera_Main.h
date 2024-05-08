#pragma once

#include "Animation_Defines.h"
#include "Camera.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(AnimationTool)

class CCamera_Main final : public CCamera
{
public:
	typedef struct tagCameraInfo
	{
		_float					fMouseSensor = 0.f;
		CTransform*				pPlayerTransformCom = { nullptr };
	}CAMERA_INFO;

	typedef struct tagCameraMainDesc
	{
		CAMERA_INFO				tCameraInfo;
		CCamera::CAMERA_DESC	tCameraDesc;
	}CAMERAMAIN_DESC;

private:
	CCamera_Main(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Main(const CCamera_Main& rhs);
	virtual ~CCamera_Main() = default;

private:
	CTransform*	m_pTransformCom = { nullptr };

private:
	HRESULT Add_Component(void* pArg);

private:
	_float	m_fMouseSensor = { 0.f };

public:
	void	Camera_Move(_float fTimeDelta);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CCamera_Main*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END