#pragma once

#include "Collider.h"

BEGIN(Engine)

class CBounding abstract : public CBase
{
public:
	typedef struct tagBoundingDesc
	{
		_float3		vCenter;
	} BOUNDING_DESC;

protected:
	CBounding();
	virtual ~CBounding() = default;

protected:
	_bool				m_bColled = { false };

public:
	virtual HRESULT		Update(_fmatrix WorldMatrix) PURE;
	virtual HRESULT		Update_Desc(BOUNDING_DESC* pDesc) { return S_OK; }
	virtual _bool		Intersect(CCollider::COLLIDER_TYPE eType, CBounding* pTargetBounding) PURE;
	virtual _bool		Intersect(_float3 vOrigin, _float3 vDirection, _float& fDist) PURE;

public:
	virtual HRESULT		Initialize(BOUNDING_DESC* pBoundingDesc) PURE;
#ifdef _DEBUG
	_vector				Compute_Color();
	virtual HRESULT		Render(PrimitiveBatch<DirectX::VertexPositionColor>* pBatch) PURE;
#endif

public:
	virtual void		Free() override;
};

END