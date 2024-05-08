#pragma once

#include "Client_Defines.h"
#include "Part_Weapon.h"

BEGIN(Client)

class CCatalyst_Zephyrus final : public CPart_Weapon
{
private:
	CCatalyst_Zephyrus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCatalyst_Zephyrus(const CCatalyst_Zephyrus& rhs);
	virtual ~CCatalyst_Zephyrus() = default;

private:
	_float	m_fAttackEndTime = { 0.f };
	_bool	m_bHand = { FALSE };
	_bool	m_bMargin = { FALSE };

private:
	virtual HRESULT	Invalidate_Animation(_float fTimeDelta);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CCatalyst_Zephyrus*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END