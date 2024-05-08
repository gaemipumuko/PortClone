#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_SPHERE final : public CBounding
{
public:
	typedef struct tagBoundingSPHEREDesc : public BOUNDING_DESC
	{
		_float		fRadius;
	} BOUNDING_SPHERE_DESC;

private:
	CBounding_SPHERE();
	virtual ~CBounding_SPHERE() = default;

private:
	BoundingSphere*			m_pBoundingDesc_Original = { nullptr };
	BoundingSphere*			m_pBoundingDesc = { nullptr };

public:
	const BoundingSphere*	Get_BoundingDesc() const { return m_pBoundingDesc; }

public:
	virtual HRESULT			Update(_fmatrix WorldMatrix) override;
	virtual HRESULT			Update_Desc(BOUNDING_DESC* pDesc) override;
	virtual _bool			Intersect(CCollider::COLLIDER_TYPE eType, CBounding* pTargetBounding) override;
	virtual _bool			Intersect(_float3 vOrigin, _float3 vDirection, _float& fDist) override;

public:
	virtual HRESULT			Initialize(BOUNDING_DESC* pBoundingDesc) override;
#ifdef _DEBUG
	virtual HRESULT			Render(PrimitiveBatch<DirectX::VertexPositionColor>* pBatch) override;
#endif

public:
	static CBounding_SPHERE*	Create(BOUNDING_DESC* pBoundingDesc);
	virtual void				Free() override;
};

END