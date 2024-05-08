#pragma once

#include "Client_Defines.h"
#include "Item.h"

BEGIN(Client)

class CItem_Consumable final : public CItem
{
public:
	enum CONSUM_TYPE { CTYPE_FOOD, CTYPE_END };

public:
	bool compareByWeaponVariable(const CItem_Consumable* other) const {
		return this->Get_Item_Rarity() > other->Get_Item_Rarity();
	}


private:
	CItem_Consumable(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem_Consumable(const CItem_Consumable& rhs);
	virtual ~CItem_Consumable() = default;

private:
	_int			m_iBaseDamage = { };
	_int			m_iHp = {};

	_float			m_fCritRate = { };
	_float			m_fCritDmg = {};

	CONSUM_TYPE		m_eConsumType = { CTYPE_END };

public:
	_int			Get_WeaponType() const { return static_cast<_int>(m_eConsumType); }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render(_float2 fPos,_float2 vRenderScale = { 100.f,128.f }) override;

	HRESULT Render_Detail(_float4x4 vItemMatrix);
	virtual void	Use_item(_uint iQty, PLAYERINDEX ePlayerIndex = PLAYER_END) override;
public:
	static CItem_Consumable*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END