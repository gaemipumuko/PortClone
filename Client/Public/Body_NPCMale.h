#pragma once

#include "Client_Defines.h"
#include "Part_Body.h"

BEGIN(Client)

class CBody_NPCMale final : public CPart_Body
{
private:
	enum HAIR_COLOR { HAIR_BLACK, HAIR_BROWN, HAIR_LIGHTBROWN, HAIR_YELLOWBROWN, HAIR_END };

private:
	CBody_NPCMale(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_NPCMale(const CBody_NPCMale& rhs);
	virtual ~CBody_NPCMale() = default;

private:
	virtual HRESULT	Invalidate_Animation(_float fTimeDelta);

private:
	HRESULT Add_Component(void* pArg);
	
private:
	HRESULT	Set_HairColor();

private:
	HAIR_COLOR		m_eHairColor = {};
	_int			m_iHairMesh = {};
	_int			m_iBeardMesh = { 10 };

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Release_ColliderID() override;

public:
	static CBody_NPCMale*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END