#pragma once

#include "Base.h"

BEGIN(Engine)

class CCascadeCamera final : public CBase
{
private:
	CCascadeCamera();
	virtual ~CCascadeCamera() = default;

private:
	class CGameInstance*	m_pGameInstance = { nullptr };

private:
	const static _int	m_iNumCascades = { 3 };

	_float		m_fFovy = { };
	_float		m_fAspect = { };

	_float4x4	m_ShadowViewMatrix = { };
	_float4x4	m_ShadowProjMatrix = { };
	_float4x4	m_ShadowVPMatrix = { };

	_float		m_fBoundRadius[m_iNumCascades] = { };
	_float4		m_vBoundCenter[m_iNumCascades] = { };
	
	_float		m_fToCascadeScale[m_iNumCascades] = { };
	_float		m_fToCascadeOffsetX[m_iNumCascades] = { };
	_float		m_fToCascadeOffsetY[m_iNumCascades] = { };

	_float		m_fCascadeEnd[m_iNumCascades + 1] = { };
	_float		m_fCascadeClip[m_iNumCascades] = { };
	_float4x4	m_CascadeMatrix[m_iNumCascades] = { };

	_float		m_fShadowBoundRadius = { };

private:
	void		ExtractFrustumBoundSphere(_vector& vBoundCenter, _float& fRadius);
	void		ExtractFrustumPoints(_int iCascadeIdx, _vector* pFrustumCorners);

public:
	void		Invalidate_Cascade(_float4 vFovArNearFar);
	void		Invalidate_CascadeCamera(const _vector vDirectionalDir);

	_float*		Get_CascadeClips() { return m_fCascadeClip; }
	_float3		Get_CascadeScale() { return _float3{ m_fToCascadeScale[0], m_fToCascadeScale[1], m_fToCascadeScale[2]}; }
	_float3		Get_CascadeOffsetX() { return _float3{ m_fToCascadeOffsetX[0], m_fToCascadeOffsetX[1], m_fToCascadeOffsetX[2] }; }
	_float3		Get_CascadeOffsetY() { return _float3{ m_fToCascadeOffsetY[0], m_fToCascadeOffsetY[1], m_fToCascadeOffsetY[2] }; }

	_float4x4	Get_ShadowVPMatrix() { return m_ShadowVPMatrix; }
	_float4x4*	Get_CascadeMatrices() { return m_CascadeMatrix; }

public:
	HRESULT					Initialize();

public:
	static CCascadeCamera*	Create();
	virtual void			Free() override;
};

END