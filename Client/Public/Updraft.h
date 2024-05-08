#pragma once

#include "Client_Defines.h"
#include "EventCollider.h"

#include "Eff_WindField.h"

BEGIN(Engine)
class CCollider;
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CUpdraft final : public CEventCollider
{
public:
	typedef struct tagUpdraftDesc
	{
		_float4 vPosition;
		_float3 vColliderSize;
		_float3 vRadians;
		_float	fLifeTime;
	}UPDRAFT_DESC;

private:
	CUpdraft(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUpdraft(const CUpdraft& rhs);
	virtual ~CUpdraft() = default;

private:
	CEff_WindField* m_pWindField = { nullptr };

private:
	_float	m_fUpSpeed = { 0.f };
	_float	m_fLifeTime = { 0.f };
	_bool	m_bLoop = { FALSE };

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CUpdraft* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void	Free() override;
};

END