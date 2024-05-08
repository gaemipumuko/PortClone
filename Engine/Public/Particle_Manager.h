#pragma once

#include "Renderer.h"

#include "Particle.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Particle_Instancing;
END

BEGIN(Engine)

class CParticle_Manager final : public CBase
{
private:
	CParticle_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CParticle_Manager() = default;

private:
	ID3D11Device*					m_pDevice = { nullptr };
	ID3D11DeviceContext*			m_pContext = { nullptr };

private:
	CShader*						m_pShaderCom = { nullptr };
	CTexture*						m_pTextureCom = { nullptr };
	CRenderer*						m_pRendererCom = { nullptr };
	CVIBuffer_Particle_Instancing*	m_pVIBufferCom = { nullptr };

private:
	map<wstring, class CParticle*>	m_Prototypes;	/* 프로토타입 */
	map<wstring, class CParticle*>	m_Particles;	/* 실사용객체 */

public:
	vector<wstring>		Get_ParticleTags();
	PARTICLE_INFO		Get_ParticleInfo(const wstring& strPrototypeTag);

public:
	HRESULT Prepare_ParticleManager(CTexture* pTexture, CRenderer* pRenderer);

	HRESULT	Play_Particle(const wstring& strPrototypeTag, const wstring& strParticleTag, 
		_int iShaderPass = 0, _int iParticleIdx = 0,
		const _float4x4* pParentBone = nullptr, class CTransform* pParentMatrix = nullptr, _float fDuration = -1.f, _bool fFade = FALSE);
	HRESULT	Pause_Particle(const wstring& strParticleTag, _bool bPause);
	HRESULT	Stop_Particle(const wstring& strParticleTag, _float fDelay);

public:
	HRESULT	Create_ParticlePrototype(const wstring& strParticleTag, PARTICLE_INFO tValue);
	HRESULT	Delete_ParticlePrototype(const wstring& strParticleTag);

	HRESULT	Save_Particles(const wstring& strFilePath);
	HRESULT	Load_Particles(const wstring& strFilePath);

public:
	HRESULT Initialize();
	HRESULT Tick(_float fTimeDelta);
	HRESULT LateTick(_float fTimeDelta);

public:
	static CParticle_Manager*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void						Clear();
	virtual void				Free() override;
};

END