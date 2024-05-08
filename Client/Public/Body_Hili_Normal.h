#pragma once

#include "Client_Defines.h"
#include "Part_Body.h"

BEGIN(Client)

class CBody_Hili_Normal final : public CPart_Body
{
private:
	CBody_Hili_Normal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Hili_Normal(const CBody_Hili_Normal& rhs);
	virtual ~CBody_Hili_Normal() = default;

private:
	virtual HRESULT	Invalidate_Animation(_float fTimeDelta);

private:
	HRESULT Add_Component(void* pArg);

private:
	_bool			m_bSetDieAnim = { FALSE };

	_bool			m_bRunAtkSound1 = { FALSE };
	_bool			m_bRunAtkSound2 = { FALSE };
	_bool			m_bRunAtkSound3 = { FALSE };

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void	Release_ColliderID() override;

public:
	static CBody_Hili_Normal*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END