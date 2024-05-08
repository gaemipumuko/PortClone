#pragma once

#include "Client_Defines.h"
#include "Ortho.h"

BEGIN(Client)

class CRect final : public COrtho
{

private:
	CRect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRect(const CRect& rhs);
	virtual ~CRect() = default;

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
	static CRect*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END