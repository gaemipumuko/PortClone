#pragma once

#include "Client_Defines.h"
#include "Ortho.h"

BEGIN(Client)

class CTeamConfig_Page : public COrtho
{
private:
	CTeamConfig_Page(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTeamConfig_Page(const CTeamConfig_Page& rhs);
	virtual ~CTeamConfig_Page() = default;

private:
	HRESULT Add_Component(void* pArg);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CTeamConfig_Page*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END