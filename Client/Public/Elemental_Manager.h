#pragma once

#include "Elemental.h"

BEGIN(Client)

class CElemental_Manager final : public CBase
{
	DECLARE_SINGLETON(CElemental_Manager)

public:
	enum ELEMENTAL_TYPE { ELEMENTAL_PARTICLE, ELEMENTAL_END };

private:
	CElemental_Manager();
	virtual ~CElemental_Manager() = default;

private:
	ID3D11Device*			m_pDevice = { nullptr };	/* 공간 할당 (Create Buffer) */
	ID3D11DeviceContext*	m_pContext = { nullptr };	/* 기능 (Draw, Bind, Etc...) */

private:
	class CGameInstance*	m_pGameInstance = { nullptr };

private: /* For. Prototype */
	class CElemental_Particle*	m_pElementalParticle = { nullptr };

private: /* For. Pool */
	multimap<ELEMENTAL_TYPE, CElemental*>	m_ElementalPool;
	multimap<ELEMENTAL_TYPE, CElemental*>	m_ActivedElemental;

public:
	HRESULT	Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT PreviousTick(_float fTimeDelta);
	HRESULT Tick(_float fTimeDelta);
	HRESULT LateTick(_float fTimeDelta);

public:
	HRESULT	Create_Elemental(_fvector vInitPosition, _uint iNumElemental, ELEMENT eAttribute, ELEMENTAL_TYPE eType);
	HRESULT	Clear_Elemental();

public:
	virtual void	Free();
};

END