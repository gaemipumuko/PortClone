#pragma once

#include "Client_Defines.h"
#include "Part_Glider.h"

BEGIN(Client)

class CGlider_Origin final : public CPart_Glider
{
private:
	CGlider_Origin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGlider_Origin(const CGlider_Origin& rhs);
	virtual ~CGlider_Origin() = default;
	
private:
	_float			fRate = { 0.f };

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
	static CGlider_Origin*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END