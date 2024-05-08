#pragma once
#include "EffectTool_Defines.h"
#include "Base.h"
#include <fstream>

BEGIN(Engine)
class CGameInstance;
END

BEGIN(EffectTool)

class CToolWindow final : public CBase
{
private:
	CToolWindow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	~CToolWindow() = default;

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	CGameInstance*			m_pGameInstance = { nullptr };

private: /* For. Particle */
	char			m_szParticleTag[MAX_PATH] = { };
	_int			m_iSelectParticleIndex = { 0 };
	vector<string>	m_ParticleTags;

	_bool			m_bRandColor = { FALSE };
	PARTICLE_INFO	m_tParticleInfo = { };

private: /* For. Play Particle */
	char			m_szParticleBoneTag[MAX_PATH] = { };

	_int			m_iSelectPlayShader = { };
	_int			m_iSelectPlayTexture = { };
	_int			m_iSelectPlayParticleIndex = { 0 };

private:
	void			Particle_Window();
	void			Effect_Window();

public:
	HRESULT			Initialize();
	void			Render_Begin();
	void			Render_End();

public:
	static CToolWindow*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void				Free();
};

END