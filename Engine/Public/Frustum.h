#pragma once

#include "Base.h"

BEGIN(Engine)

class CFrustum final : public CBase
{
private:
	CFrustum();
	virtual ~CFrustum() = default;

private:
	class CGameInstance*	m_pGameInstance = { nullptr };

private:
	_float3					m_vPoints[8] = { };
	_float3					m_vWorldPoints[8] = { };
	_float4					m_vWorldPlanes[6] = { };
	_float4					m_vLocalPlanes[6] = { };

private:
	HRESULT Make_Plane(const _float3* pPoints, _Inout_ _float4* pPlanes);

public:
	_float3*	Get_WorldFrustum() { return m_vWorldPoints; }

public:
	_bool	PointInFrustum_InLocal(_fvector vLocalPos, _float fRange);
	_bool	PointInFrustum_InWorld(_fvector vWorldPos, _float fRange);

	void Transform_LocalSpace(_fmatrix WorldMatrixInv);

public:
	HRESULT Initialize();
	HRESULT Tick(_float fTimeDelta);

public:
	static CFrustum*	Create();
	virtual void		Free() override;
};

END