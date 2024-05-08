#pragma once

#include "Client_Defines.h"
#include "Part_Weapon.h"

BEGIN(Client)

class CPole_Kunwu final : public CPart_Weapon
{
private:
	CPole_Kunwu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPole_Kunwu(const CPole_Kunwu& rhs);
	virtual ~CPole_Kunwu() = default;
	
private:
	//_float4	m_vTrailOriginalColor = { 0.f, 0.f, 0.f, 1.f };
	//_float4	m_vTrailAttenuationColor = { 1.f, 1.f, 1.f, 1.f };

private:
	_float	m_fAttackEndTime = { 0.f };
	_bool	m_bHand = { FALSE };

public:
	//virtual HRESULT Bind_ShaderResources_Trail();

public:
	//_matrix Get_CombinedMatrix();

private:
	virtual HRESULT	Invalidate_Animation(_float fTimeDelta);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	//virtual HRESULT Render_Trail() override;
	virtual HRESULT Render() override;

public:
	static CPole_Kunwu*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END