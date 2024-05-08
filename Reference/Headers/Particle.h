#pragma once

#include "Base.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Particle_Instancing;
class CVIBuffer_Rect;
END

BEGIN(Engine)

class CParticle final : public CBase
{
public:
	typedef struct tagParticleDesc
	{
		_bool				bFade = { FALSE };

		_int				iShaderPass = { };
		_int				iParticleIdx = { };

		_float				fDuration = { };

		const _float4x4*	pParentBoneMatrix = { };
		class CTransform*	pParentWorldMatrix = { };

		CShader*			pShaderCom = { };
		CTexture*			pTextureCom = { };
		CVIBuffer_Particle_Instancing*	pVIBufferCom = { };
	} PARTICLE_DESC;

private:
	CParticle();
	CParticle(const CParticle& rhs);
	virtual ~CParticle() = default;

private:
	class CGameInstance*			m_pGameInstance = { nullptr };

private:
	CShader*						m_pShaderCom = { nullptr };
	CTexture*						m_pTextureCom = { nullptr };
	CVIBuffer_Particle_Instancing*	m_pVIBufferCom = { nullptr };

private:
	_bool				m_bPause = { FALSE };

	_float				m_fDelay = { 0.f };
	_bool				m_bDelayStop = { FALSE };

public:
	void				Set_Pause(_bool bPause) { m_bPause = bPause; }
	_bool				Get_Pause() { return m_bPause; }

	_float				Get_Delay() { return m_fDelay; }
	void				Set_Delay(_float fDelay) { m_fDelay = fDelay; }

	_bool				Get_DelayStop() { return m_bDelayStop; }
	void				Set_DelayStop(_bool bStop) { m_bDelayStop = bStop; }

private:
	const _float4x4*	m_pParentBoneMatrix = { };
	class CTransform*	m_pParentWorldMatrix = { };
	_float4x4			m_WorldMatrix = { };

	_int				m_iShaderPass = { };
	_int				m_iParticleIdx = { };

	_float				m_fMaxCycle = { };
	_float				m_fCurrCycle = { };

	_bool				m_bCloned = { FALSE };
	_bool				m_bDuration = { FALSE };
	_bool				m_bFade = { FALSE };
	_float				m_fDuration = { 0.f };
	_float				m_fTimeAcc = {0.f};

	_bool				m_isCombined = { false };

	PARTICLE_INFO		m_tParticleInfo = { };

public:
	_float				Get_Duration() { return m_fDuration; }
	PARTICLE_INFO&		Get_ParticleInfo() { return m_tParticleInfo; }

public:
	HRESULT Invalidate_Instance(_float fTimeDelta);

private:
	HRESULT Add_Component(void* pArg);
	HRESULT Bind_ShaderResources();

public:
	HRESULT	Initialize_Prototype(PARTICLE_INFO tParticleInfo);
	HRESULT	Initialize_Clone(void* pArg);
	HRESULT Render();

public:
	static CParticle*		Create(PARTICLE_INFO tParticleInfo);
	CParticle*				Clone(void* pArg);
	virtual void			Free() override;
};

END