#pragma once

#include "Client_Defines.h"
#include "DeciderObject.h"

BEGIN(Engine)
class CCollider;
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CEventCollider abstract : public CDeciderObject
{
protected:
	CEventCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEventCollider(const CEventCollider& rhs);
	virtual ~CEventCollider() = default;

protected:
	CRenderer*	m_pRendererCom = { nullptr };
	CCollider*	m_pColliderCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };

protected:
	_int		m_iColliderID = { -1 };

public:
	virtual HRESULT Initialize_Prototype() PURE;
	virtual HRESULT Initialize_Clone(void* pArg);

	virtual HRESULT PreviousTick(_float fTimeDelta) PURE;
	virtual HRESULT Tick(_float fTimeDelta) PURE;
	virtual HRESULT LateTick(_float fTimeDelta) PURE;
	virtual HRESULT Render() PURE;

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void		Free() override;
};

END
