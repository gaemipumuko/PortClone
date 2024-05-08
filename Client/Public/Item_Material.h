#pragma once

#include "Client_Defines.h"
#include "Item.h"

BEGIN(Client)

class CItem_Material final : public CItem
{
public:
	enum MATERIAL_TYPE { WTYPE_EXP, MTYPE_END };

private:
	CItem_Material(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem_Material(const CItem_Material& rhs);
	virtual ~CItem_Material() = default;

private:
	_int			m_iExp = { };


	MATERIAL_TYPE		m_eMaterialType = { MTYPE_END };

public:
	_int			Get_WeaponType() const { return static_cast<_int>(m_eMaterialType); }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render(_float2 fPos, _float2 vRenderScale = { 100.f,128.f }) override;

public:
	virtual void	Use_item(_uint iQty, PLAYERINDEX ePlayerIndex = PLAYER_END) override;
	HRESULT Render_Detail(_float4x4 vItemMatrix);

	

public:
	static CItem_Material*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END