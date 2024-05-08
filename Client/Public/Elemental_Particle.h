#pragma once

#include "Client_Defines.h"
#include "Elemental.h"

BEGIN(Client)

class CElemental_Particle final : public CElemental
{
private:
	CElemental_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CElemental_Particle(const CElemental_Particle& rhs);
	virtual ~CElemental_Particle() = default;
	
private:
	HRESULT Add_Component(void* pArg);
	HRESULT Bind_ShaderResources();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CElemental_Particle*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END