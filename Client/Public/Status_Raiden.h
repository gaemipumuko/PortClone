#pragma once

#include "Client_Defines.h"
#include "Part_Status.h"

BEGIN(Client)

class CStatus_Raiden final : public CPart_Status
{
private:
	CStatus_Raiden(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStatus_Raiden(const CStatus_Raiden& rhs);
	virtual ~CStatus_Raiden() = default;

private:
	HRESULT Set_Status();

public:
	virtual void Level_BreakThrough() override;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override { return S_OK; }

public:
	static CStatus_Raiden*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END