#pragma once

#include "Client_Defines.h"
#include "Part_Body.h"

BEGIN(Client)

class CBody_Effigy_Fire final : public CPart_Body
{
public:
	enum EFFIGY_WEAPON { EFFIGY_WEAPON_BODY, EFFIGY_WEAPON_CLAP, EFFIGY_WEAPON_END };

private:
	CBody_Effigy_Fire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Effigy_Fire(const CBody_Effigy_Fire& rhs);
	virtual ~CBody_Effigy_Fire() = default;

private:
	class CTransform*	m_pAuraTransform = { nullptr };
	class CTransform*	m_pMagicCircleTransform = { nullptr };
	class CTransform*	m_pParticleTransform = { nullptr };

private:
	CCollider*			m_pWeaponColliderCom[EFFIGY_WEAPON_END] = {};
	_int				m_iWeaponColliderID[EFFIGY_WEAPON_END] = {};

private:
	_float4		m_vTrailOriginalColor = { 0.f, 0.f, 0.f, 1.f };
	_float4		m_vTrailAttenuationColor = { 1.f, 1.f, 1.f, 1.f };

private:
	virtual HRESULT	Invalidate_Animation(_float fTimeDelta);
	virtual HRESULT Bind_ShaderResources_Trail();

private:
	HRESULT Add_Component(void* pArg);
	
private:
	// ���� ���� �θ��� ȸ������ �������ֱ� ���� ������
	ROTATION_DIRECTION  m_eRotationDir = { ROT_END };
	_int m_iRRPrevState = -1;		// RR : Return Rotation
	_float	m_fAccumulateRRRadian = { 0.f };
	_bool	m_bReturnRotation = { FALSE };
	_bool	m_bInitRRAnimation = { FALSE };

	// ī�޶� �ƽ�
	_int	m_iCameraSceneIndex = 0;

	// ���� ����
	_int  m_iAnimCount = 0;

	// ù ������ ����
	_bool m_bIsFirstFrame = TRUE;

	// ���� ����
	_bool m_bHasMoved = FALSE;

	// ť�� ����ü
	_float m_fFire_Cube_Timer = 0.f;
	_bool m_bAlready_Shoot_Fire_Cube = FALSE;
	_int m_iAttack_Cube_Index = 0;
	vector<_vector>Fire_Cube_Attack_Pos = {};
	vector<_int>Fire_Cube_Mesh_Index = {};

	// ������ �Ǳ� ��ƼŬ
	_bool m_bAlertParticle = { FALSE };

	// ��ȭ ����
	_bool m_bIsIgnite = FALSE;
	_bool m_bIsIgniteSuccess = FALSE;

	// ������ũ �Ҵ�����
	_int  m_iFireBallStep = { 0 };
	_int  m_iFireBallIndex = { 0 };

	// �ָ���� ����ġ��
	_bool m_bPunchEff = { FALSE };
	_bool m_bPunchTrail = { FALSE };

	// �������� ����Ʈ
	_bool m_bTrsFishEff[2] = { FALSE };
	_bool m_bTrsBombEff	   = { FALSE };

	// �ڼ� ¦¦
	_bool m_bClapEff = { FALSE };

	// ��ƼŬ Ʈ����
	_bool				m_bParticleMove = { FALSE };

	list<wstring>		m_ParticleTags;
	list<wstring>		m_ParticleMoveTags;

public:
	void	Set_HasMoved(_bool _bHasMoved) { m_bHasMoved = _bHasMoved; }
	void	Set_IsIgnite(_bool _IsIgnite) { m_bIsIgnite = _IsIgnite; }
	void	Set_IsIgniteSuccess(_bool _IsIgniteSuccess) { m_bIsIgniteSuccess = _IsIgniteSuccess; }

	_int	Get_WeaponCollider(_int iIndex) { return m_iWeaponColliderID[iIndex]; }

	HRESULT	Spawn_Fire_Cube();

private:
	HRESULT	Invalidate_Particle();

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
	static CBody_Effigy_Fire*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END