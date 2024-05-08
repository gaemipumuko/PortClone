#pragma once

#include "Client_Defines.h"
#include "Ortho.h"

BEGIN(Client)

class CDecor final : public COrtho
{
private:
	CDecor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDecor(const CDecor& rhs);
	virtual ~CDecor() = default;

private:
	HRESULT Add_Component(void* pArg);

public:
	void Set_CharWindwRenderSwitch(_bool iswitch) {
		m_bWindowTrue = iswitch;
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CDecor*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END