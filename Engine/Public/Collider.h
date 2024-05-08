#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
public:
	enum COLLIDER_TYPE { CTYPE_AABB, CTYPE_OBB, CTYPE_SPHERE, CTYPE_FRUSTUM, CTYPE_END };

private:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

private:
	COLLIDER_TYPE		m_eType = { CTYPE_END };
	class CBounding*	m_pBounding = { nullptr };

public:
	_bool	Intersect(const class CCollider* pTargetCollider);
	_bool	Intersect(_float3 vOrigin, _float3 vDirection, _float& fDist);
	void	Update(_fmatrix WorldMatrix);
	void	Update_Desc(void* pBoundingDesc);

public:
	virtual HRESULT Initialize_Prototype(COLLIDER_TYPE eType);
	virtual HRESULT Initialize_Clone(void* pArg) override;

#ifdef _DEBUG
private:
	static _bool	m_bDebugRender;

public:
	static _bool	Get_DebugRender() { return m_bDebugRender; }
	static void		Toggle_DebugRender() { m_bDebugRender = !m_bDebugRender; }

private:
	BasicEffect*							m_pEffect = { nullptr };
	ID3D11InputLayout*						m_pInputLayout = { nullptr };
	PrimitiveBatch<VertexPositionColor>*	m_pBatch = { nullptr };

public:
	virtual HRESULT Render() override;
#endif 

public:
	static	CCollider*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, COLLIDER_TYPE eType);
	virtual CComponent*		Clone(void* pArg) override;
	virtual void			Free() override;
};

END