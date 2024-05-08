#pragma once

#include "Client_Defines.h"
#include "Ortho.h"

BEGIN(Client)

class CPage : public COrtho
{
private:
	CPage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPage(const CPage& rhs);
	virtual ~CPage() = default;

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
	static CPage*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END