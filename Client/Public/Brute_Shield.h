#pragma once

#include "Client_Defines.h"
#include "Part_Weapon.h"

BEGIN(Client)

class CBrute_Shield final : public CPart_Weapon
{
private:
	CBrute_Shield(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBrute_Shield(const CBrute_Shield& rhs);
	virtual ~CBrute_Shield() = default;
	
private:
	_float4	m_vTrailOriginalColor = { 0.f, 0.f, 0.f, 1.f };
	_float4	m_vTrailAttenuationColor = { 1.f, 1.f, 1.f, 1.f };

public:
	_matrix Get_CombinedMatrix();

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
	static CBrute_Shield*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END