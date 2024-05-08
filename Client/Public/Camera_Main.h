#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Engine)
class CTransform;
class CCollider;
#ifdef _DEBUG
class CRenderer;
#endif
END

BEGIN(Client)

class CCamera_Main final : public CCamera
{
public:
	typedef struct tagCameraInfo
	{
		_float					fMouseSensor = 0.f;
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
	CTransform* m_pTransformCom = { nullptr };
	CTransform*	m_pArmTransformCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
#ifdef _DEBUG
	CTransform* m_pFreeTransformCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
#endif

private:
	HRESULT Add_Component(void* pArg);

#ifdef _DEBUG
private:	/* For.Free Camera */
	_bool	m_bFreeCamera = { FALSE };

public:		/* For.Free Camera */
	void	Set_FreeCamera(_bool bFree) { m_bFreeCamera = bFree; }
	_bool	Get_IsFreeCamera() { return m_bFreeCamera; }
#endif

public:		/* For. Action Mode */
	void	Set_ActionMode(_float fActionSpeed, _bool NearAction = TRUE);

private:
	HRESULT Action_Camera(_float fTimeDelta);

public:		/* For. Shaking Mode */
	void	Set_ShakingMode(_float fMaxRange, _float fShakingSpeed, _int iMaxShakingNum);

private:	
	HRESULT	Shaking_Camera(_float fTimeDelta);

public:		/* For. Elemental Burst Mode */
	void Set_SceneMode(CAMSCENEID eCamSceneId, SCENEID eSceneNum, _float fMargin, _float fCameraSpeed, _bool bSceneMode = true, CTransform* pTransform = nullptr);
	CAMSCENEID Get_CamScene() { return m_eCharacterId; }
	SCENEID Get_SceneId(CAMSCENEID eCamSceneId);
	_bool	End_Scene(CAMSCENEID eCamSceneId);

public: /* For.Culling */
	_bool Culling_Frustum(const CCollider* pTargetCollider);

private:
	void Lerp_Camera(_float fTimeDelta);

private:
	HRESULT SceneCamera(_float fTimeDelta);

	HRESULT Diluc_Burst(_float fTimeDelta);
	HRESULT Zhogli_Burst(_float fTimeDelta);
	HRESULT Barbara_Burst(_float fTimeDelta);
	HRESULT Raiden_Burst(_float fTimeDelta);
	HRESULT Change_Scene(_float fTimeDelta);
	HRESULT Andrius_CutScene(_float fTimeDelta);
	HRESULT Dvalin_CutScene(_float fTimeDelta);
	HRESULT BasicTalk_Scene(_float fTimeDelta);
	HRESULT AmborTalk_Scene(_float fTimeDelta);
	HRESULT Gate_CutScene(_float fTimeDelta);
	HRESULT Dungeon_CutScene(_float fTimeDelta);

private:
	_vector m_vPrevPlayerPos = {};

private:	/* For.Main Camera */
	_float	m_fRadius = { 3.f };
	_float	m_fCurrAngle = { 0.f };

	_float	m_fMouseSensor = { 0.f };
	_vector m_vFirstCamPos = { 0.f };
	_vector m_vFirstCamRot = { 0.f };

private:	/* For.Action Camera */
	_bool	m_bActionMode = { FALSE };
	_bool	m_bNearAction = { FALSE };
	_float	m_fActionSpeed = {};
	_int	m_iActionNum = {};

private:	/* For. Shaking Mode */
	_float	m_fMoveValue = { 0.f };
	_float	m_fMaxRange = { 0.f };
	_float	m_fShakingSpeed = { 0.f };
	_int	m_iShakingNum = { 0 };
	_int	m_iMaxShakingNum = { 0 };
	_bool	m_bShakingMode = { FALSE };

private:	/* For. Scene Mode */
	_bool			m_bSceneMode = { FALSE };
	CAMSCENEID		m_eCharacterId = { CAMSCENEID_END };
	SCENEID			m_eSceneNum = { SCENE_END };
	_float			m_fCameraSpeed = { 0.f };
	_float			m_fMargin = { 0.f }; /* 대상과의 거리 */
	_bool			m_bSceneShaking = { FALSE };
	_bool			m_bEndScene = { FALSE };
	CTransform*		m_pRefTransform = { nullptr };

	_bool			m_bInitDir = { FALSE };
	_vector			m_vInitDir = {};

private:
	_bool			m_bLerpMode = { FALSE }; /* 카메라 보간 */
	_bool			m_bDebugRender = false;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CCamera_Main*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END