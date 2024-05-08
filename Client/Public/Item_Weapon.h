#pragma once

#include "Client_Defines.h"
#include "Item.h"

BEGIN(Client)

class CItem_Weapon final : public CItem
{
public:
	enum WEAPON_TYPE { WTYPE_SWORD, WTYPE_CLAYMORE, WTYPE_POLEARM, WTYPE_CATALYST, WTYPE_BOW, WTYPE_END };
private:
	CItem_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem_Weapon(const CItem_Weapon& rhs);
	virtual ~CItem_Weapon() = default;

private:
	_int			m_iBaseDamage = { };

	WEAPON_TYPE		m_eWeaponType = { WTYPE_END };

public:
	_int			Get_WeaponType() const { return static_cast<_int>(m_eWeaponType); }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render(_float2 fPos, _float2 vRenderScale = { 100.f,128.f }) override;

	HRESULT Render_Detail(_float4x4 vItemMatrix);
	virtual void	Use_item(_uint iQty, PLAYERINDEX ePlayerIndex) override;

public:
	static CItem_Weapon*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END