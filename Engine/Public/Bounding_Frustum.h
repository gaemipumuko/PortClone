#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_FRUSTUM final : public CBounding
{
public:
	typedef struct tagBoundingFrustumDesc : public BOUNDING_DESC
	{
		_float4x4 vViewMatrix;
		_float4x4 vProjMatrix;
	} BOUNDING_FRUSTUM_DESC;

private:
	CBounding_FRUSTUM();
	virtual ~CBounding_FRUSTUM() = default;

public:
	const BoundingFrustum* Get_BoundingDesc() const { return m_pBoundingDesc; }

public:
	virtual HRESULT Update(_fmatrix WorldMatrix) override;
	virtual _bool Intersect(CCollider::COLLIDER_TYPE eType, CBounding* pTargetBounding) override;
	virtual _bool Intersect(_float3 vOrigin, _float3 vDirection, _float& fDist) override;

public:
	virtual HRESULT Initialize(BOUNDING_DESC* pBoundingDesc) override;

#ifdef _DEBUG
	virtual HRESULT Render(PrimitiveBatch<DirectX::VertexPositionColor>* pBatch) override;
#endif

private:
	BoundingFrustum* m_pBoundingDesc_Original = { nullptr };
	BoundingFrustum* m_pBoundingDesc = { nullptr };

public:
	static CBounding_FRUSTUM* Create(BOUNDING_DESC* pBoundingDesc);
	virtual void Free() override;
};

END
