#pragma once

#include "Client_Defines.h"
#include "Part_Weapon.h"

BEGIN(Client)

class CPole_Narukami final : public CPart_Weapon
{
private:
	CPole_Narukami(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPole_Narukami(const CPole_Narukami& rhs);
	virtual ~CPole_Narukami() = default;
	
private:
	CModel*		m_pSwordModelCom = { nullptr };

private:
	_float	m_fAttackEndTime = { 0.f };
	_bool	m_bInitTime = { FALSE };
	_bool	m_bHand = { FALSE };

private:
	HRESULT Add_ExtraComponent();
	HRESULT Render_Sword();

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
	static CPole_Narukami*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END