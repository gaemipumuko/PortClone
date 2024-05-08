#pragma once

#include "Client_Defines.h"
#include "Part_Body.h"

BEGIN(Client)

class CBody_Traveler final : public CPart_Body
{
private:
	CBody_Traveler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Traveler(const CBody_Traveler& rhs);
	virtual ~CBody_Traveler() = default;

private:
	virtual HRESULT	Invalidate_Animation(_float fTimeDelta);

private:
	HRESULT Add_Component(void* pArg);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CBody_Traveler*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END