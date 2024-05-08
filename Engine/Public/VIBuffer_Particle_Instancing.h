#pragma once

#include "VIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Particle_Instancing final : public CVIBuffer_Instancing
{
private:
	CVIBuffer_Particle_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Particle_Instancing(const CVIBuffer_Particle_Instancing& rhs);
	virtual ~CVIBuffer_Particle_Instancing() = default;

private:
	_bool OptionCheck(_uint iTotalOption, _uint Check) {
		return iTotalOption & (0x1 << Check);
	}

private:
	_float			m_fNextCreateTime = { };

	_bool*			m_pValids = { nullptr };
	_float*			m_pSpeeds = { nullptr };
	_float2*		m_pLifeTimes = { nullptr };
	_float3*		m_pDirections = { nullptr };

	_float*			m_pGravityTime = { nullptr };

public:
	virtual HRESULT Invalidate_Instance(_float fTimeDelta, PARTICLE_INFO tParticleInfo);

public:
	virtual HRESULT Initialize_Prototype(_uint iNumInstance);
	virtual HRESULT Initialize_Clone(void* pArg) override;

public:
	static CVIBuffer_Particle_Instancing*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumInstance);
	virtual CComponent*						Clone(void* pArg) override;
	virtual void							Free() override;
};

END