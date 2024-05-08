#pragma once

#include "Client_Defines.h"
#include "Part_Body.h"

BEGIN(Client)

class CBody_Katheryne final : public CPart_Body
{
private:
	CBody_Katheryne(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Katheryne(const CBody_Katheryne& rhs);
	virtual ~CBody_Katheryne() = default;

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
	static CBody_Katheryne*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END