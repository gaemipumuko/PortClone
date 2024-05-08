#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_AABB final : public CBounding
{
public:
	typedef struct tagBoundingAABBDesc : public BOUNDING_DESC
	{
		_float3			vExtents;
	} BOUNDING_AABB_DESC;

private:
	CBounding_AABB();
	virtual ~CBounding_AABB() = default;

private:
	BoundingBox* m_pBoundingDesc_Original = { nullptr };
	BoundingBox* m_pBoundingDesc = { nullptr };

public:
	const BoundingBox*		Get_BoundingDesc() const { return m_pBoundingDesc; }

public:
	virtual HRESULT			Update(_fmatrix WorldMatrix) override;
	virtual _bool			Intersect(CCollider::COLLIDER_TYPE eType, CBounding* pTargetBounding) override;
	virtual _bool			Intersect(_float3 vOrigin, _float3 vDirection, _float& fDist) override;

public:
	virtual HRESULT			Initialize(BOUNDING_DESC* pBoundingDesc) override;
#ifdef _DEBUG
	virtual HRESULT			Render(PrimitiveBatch<DirectX::VertexPositionColor>* pBatch) override;
#endif

public:
	static CBounding_AABB*	Create(BOUNDING_DESC* pBoundingDesc);
	virtual void			Free() override;
};

END