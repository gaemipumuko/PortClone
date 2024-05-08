#pragma once

#include "Client_Defines.h"
#include "Part_Status.h"

BEGIN(Client)

class CStatus_Andrius final : public CPart_Status
{
private:
	CStatus_Andrius(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStatus_Andrius(const CStatus_Andrius& rhs);
	virtual ~CStatus_Andrius() = default;

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
	static CStatus_Andrius*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END