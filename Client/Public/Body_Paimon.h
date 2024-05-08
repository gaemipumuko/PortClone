#pragma once

#include "Client_Defines.h"
#include "Part_Body.h"

BEGIN(Client)

class CBody_Paimon final : public CPart_Body
{
private:
	CBody_Paimon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Paimon(const CBody_Paimon& rhs);
	virtual ~CBody_Paimon() = default;

private:
	virtual HRESULT	Invalidate_Animation(_float fTimeDelta);

private:
	HRESULT Add_Component(void* pArg);
	
private:

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Release_ColliderID() override;

public:
	static CBody_Paimon*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END