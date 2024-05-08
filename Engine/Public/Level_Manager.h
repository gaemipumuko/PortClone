#pragma once

#include "Base.h"

BEGIN(Engine)

class CLevel_Manager final : public CBase
{
private:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

private:
	_uint			m_iNumLevel = { 0 };
	_uint			m_iCurrentIndex = { 0 };
	class CLevel*	m_pCurrentLevel = { nullptr };

public:
	HRESULT Open_Level(_uint iLevel, class CLevel* pLevel);

	_uint			Get_NumLevel() { return m_iNumLevel; }
	_uint			Get_CurrentLevel() { return m_iCurrentIndex; }
	class CLevel*	Get_CurrentLevelObject() { return m_pCurrentLevel; }

public:
	HRESULT Initialize(_uint iNumLevel);
	HRESULT PreviousTick(_float fTimeDelta);
	HRESULT Tick(_float fTimeDelta);
	HRESULT LateTick(_float fTimeDelta);
	HRESULT Render();

public:
	static CLevel_Manager*	Create(_uint iNumLevel);
	virtual void			Free() override;
};

END