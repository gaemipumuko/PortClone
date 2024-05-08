#pragma once

#include "Client_Defines.h"
#include "Part_Body.h"

BEGIN(Client)

class CBody_Slime_Ice final : public CPart_Body
{
private:
	CBody_Slime_Ice(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Slime_Ice(const CBody_Slime_Ice& rhs);
	virtual ~CBody_Slime_Ice() = default;

private:
	virtual HRESULT	Invalidate_Animation(_float fTimeDelta);

private:
	HRESULT Add_Component(void* pArg);

private:
	_bool	m_bSoundPlayed = { false };

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
	static CBody_Slime_Ice*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END