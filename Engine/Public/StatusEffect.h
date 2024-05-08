#pragma once
#include "Base.h"

BEGIN(Engine)

class CStatusEffect final : public CBase
{
public:
	typedef struct tagStatusEffectDesc
	{
		_int	iCount;
		_float	fDuration;
	}STATUSEFFECT_DESC;

private:
	CStatusEffect();
	virtual ~CStatusEffect() = default;

private:
	_int	m_iCount = { 0 };
	_float	m_fDuration = { 0.f };
	_float	m_fTimeAcc = { 0.f };
	_bool	m_bDestroy = { FALSE };

public:
	_int	Get_Count() { return m_iCount; }
	_bool	Get_Destroy() { return m_bDestroy; }

	void	Set_Destroy(_bool bDestroy) { m_bDestroy = TRUE; }
	void	Set_Count(_int iCount) { m_iCount = iCount; }
	void	Set_Count_Compare(_int iCount, _bool bMax);
	void	Set_Duration(_float fDuration) { m_fDuration = fDuration; m_fTimeAcc = 0.f; }

	void	Add_Count(_int iCount) { m_iCount += iCount; }

public:
	HRESULT Initialize(void* pArg);
	HRESULT Update(_float fTimeDelta);

public:
	static CStatusEffect* Create(void* pArg);
	virtual void Free() override;
};

END