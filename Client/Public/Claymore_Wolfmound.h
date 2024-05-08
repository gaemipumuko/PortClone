#pragma once

#include "Client_Defines.h"
#include "Part_Weapon.h"

BEGIN(Client)

class CClaymore_Wolfmound final : public CPart_Weapon
{
private:
	CClaymore_Wolfmound(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CClaymore_Wolfmound(const CClaymore_Wolfmound& rhs);
	virtual ~CClaymore_Wolfmound() = default;

private:
	_float	m_fAttackEndTime = { 0.f };
	_bool	m_bHand = { FALSE };

public:
	virtual HRESULT Bind_ShaderResources_Trail();

public:
	_matrix Get_CombinedMatrix();

private:
	virtual HRESULT	Invalidate_Animation(_float fTimeDelta);

public:
	HRESULT	Create_FireBlade();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CClaymore_Wolfmound*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END