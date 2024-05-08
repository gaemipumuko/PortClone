#pragma once

#include "Client_Defines.h"
#include "Part_Body.h"

BEGIN(Client)

class CBody_Dvalin final : public CPart_Body
{
public:
	enum DVALIN_COLL { 
		DVALIN_COLL_LHAND, DVALIN_COLL_RHAND, DVALIN_COLL_HEAD, DVALIN_COLL_NECK, DVALIN_COLL_BODY,
		DVALIN_COLL_RUSH,
		DVALIN_COLL_END };

private:
	CBody_Dvalin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Dvalin(const CBody_Dvalin& rhs);
	virtual ~CBody_Dvalin() = default;

private:
	virtual HRESULT	Invalidate_Animation(_float fTimeDelta);
	virtual HRESULT Bind_ShaderResources_Trail();

private:
	HRESULT Add_Component(void* pArg);

private:
	CCollider*	m_pBodyColliderCom[DVALIN_COLL_END] = {};
	_int		m_iBodyColliderID[DVALIN_COLL_END] = {};
	
private:
	// 뼈에 따라서 부모의 회전값을 적용해주기 위한 변수들
	ROTATION_DIRECTION  m_eRotationDir = { ROT_END };
	_int	m_iRRPrevState = -1;		// RR : Return Rotation
	_float	m_fAccumulateRRRadian = { 0.f };
	_bool	m_bReturnRotation = { FALSE };
	_bool	m_bInitRRAnimation = { FALSE };

	_bool	m_bActivatedCollider = { TRUE };

	// 카메라 컷신
	_int	m_iCameraSceneIndex = 0;

	_int  m_iAnimCount = 0;

	// 첫 프레임 실행
	_bool m_bIsFirstFrame = TRUE;

	CTexture* m_pTextureCom = { nullptr };
	_float2 m_vTextureOffset = _float2(-0.3f, 0.0f);

	_float4x4 m_BreatheMatrix = {};

	// 테스트용
	_uint m_iBoneIndex = { 278 };
	_bool	bAnimStop = { FALSE };

private: /* For.Platform */
	class CDvalinPlatform* m_pPlatform = { nullptr };
	_bool m_bOnPlatform = false;
	_uint m_iIsFloorCollapseCount = 0;
	_bool bColl_1 = FALSE;
	_bool bColl_2 = FALSE;

public:
	_int		Get_WeaponColliderID(DVALIN_COLL eCollider) { return m_iBodyColliderID[eCollider]; }
	_matrix		Get_BreatheMatrix() { return XMLoadFloat4x4(&m_BreatheMatrix); }
	_vector		Get_BoneWorldPosition(const _char* strBoneTag);

public:
	void		Set_ActivatedColliderBody(_bool bActivate) { m_bActivatedCollider = bActivate;  }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow(_float4x4* pLightViewProj, _int iNumArray) override;

public:
	virtual void	Release_ColliderID() override;

public:
	static CBody_Dvalin*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END