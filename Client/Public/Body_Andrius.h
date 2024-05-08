#pragma once

#include "Client_Defines.h"
#include "Part_Body.h"

BEGIN(Client)

class CBody_Andrius final : public CPart_Body
{
public:
	enum ANDRIUS_WEAPON { WEAPON_LHAND, WEAPON_RHAND, WEAPON_SWEEP, WEAPON_END };

private:
	CBody_Andrius(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Andrius(const CBody_Andrius& rhs);
	virtual ~CBody_Andrius() = default;

private:
	_float4	m_vTrailOriginalColor = { 0.f, 0.f, 0.f, 1.f };
	_float4	m_vTrailAttenuationColor = { 1.f, 1.f, 1.f, 1.f };

private:
	virtual HRESULT	Invalidate_Animation(_float fTimeDelta);
	virtual HRESULT Bind_ShaderResources_Trail();

private:
	HRESULT Add_Component(void* pArg);

private:
	_int		m_iWeaponColliionID[WEAPON_END] = {};

	CCollider*	m_pWeaponColliderCom[WEAPON_END] = {};
	
private:
	ANIM_ANDRIUS_STATE	m_eRRPrevState = { ANIM_ANDRIUS_END };	// RR : Return Rotation
	ROTATION_DIRECTION  m_eRotationDir = { ROT_END };

	_float	m_fAccumulateRRRadian = { 0.f };
	_bool	m_bReturnRotation = { FALSE };
	_bool	m_bInitRRAnimation = { FALSE };

	_int	m_iCameraSceneIndex = 0;

	CTexture* m_pTexture_BoreasLightMap = { nullptr };
	CTexture* m_pTexture_BoreasFluid = { nullptr };
	CTexture* m_pTexture_BoreasIce = { nullptr };

public:
	_int	Get_WeaponCollisionID(ANDRIUS_WEAPON eWeapon) { return m_iWeaponColliionID[eWeapon]; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow(_float4x4* pLightViewProj, _int iNumArray) override;
	virtual HRESULT Render_MTrail() override;

public:
	virtual void	Release_ColliderID() override;

public:
	static CBody_Andrius*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END