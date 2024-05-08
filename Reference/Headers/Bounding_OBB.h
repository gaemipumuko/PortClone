#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_OBB final : public CBounding
{
public:
	typedef struct tagBoundingOBBDesc : public BOUNDING_DESC
	{
		_float3		vExtents;
		_float3		vRadians;
	} BOUNDING_OBB_DESC;

	typedef struct tagOBBDesc
	{
		_float3		vAlignAxis[3];
		_float3		vCenterAxis[3];
	} OBB_DESC;

private:
	CBounding_OBB();
	virtual ~CBounding_OBB() = default;

private:
	BoundingOrientedBox*			m_pBoundingDesc_Original = { nullptr };
	BoundingOrientedBox*			m_pBoundingDesc = { nullptr };

public:
	const BoundingOrientedBox*		Get_BoundingDesc() const { return m_pBoundingDesc; }

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
	static CBounding_OBB*	Create(BOUNDING_DESC* pBoundingDesc);
	virtual void			Free() override;
};

END