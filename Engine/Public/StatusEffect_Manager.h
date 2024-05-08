#pragma once
#include "StatusEffect.h"

BEGIN(Engine)

class CStatusEffect_Manager final : public CBase
{
private:
	CStatusEffect_Manager();
	virtual ~CStatusEffect_Manager() = default;

private:
	map<wstring, CStatusEffect*>* m_StatusEffects;
	_uint	m_iLayerNum = { 0 };

public:
	HRESULT Add_StatusEffect(_uint iLayer, wstring strObjectID, _int iCount = 0, _float fDuration = 0.f, _bool bCompareMax = TRUE);
	HRESULT Release_StatusEffect(_uint iLayer, wstring strObjectID);
	HRESULT Add_Count_StatusEffect(_uint iLayer, wstring strObjectID, _int iCount);
	_int	Get_Count_StatusEffect(_uint iLayer, wstring strObjectID);

	_bool	IsProgress_StatusEffect(_uint iLayer, wstring strObjectID);

public:
	HRESULT Initialize(_uint iNumLayer);
	HRESULT Tick(_float fTimeDelta);

public:
	static CStatusEffect_Manager* Create(_uint iNumLayer);
	virtual void Free() override;
};

END

