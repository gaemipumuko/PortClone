#pragma once

#include "Base.h"

BEGIN(Engine)

class CTimer final : public CBase
{
private:
	CTimer();
	virtual ~CTimer() = default;

private:
	LARGE_INTEGER		m_CurrentTime;
	LARGE_INTEGER		m_OldTime;
	LARGE_INTEGER		m_OriginTime;
	LARGE_INTEGER		m_CpuTick;

	_float				m_fTimeDelta;

public:
	_float				Compute_TimeDelta(void);

public:
	HRESULT				Initialize();

public:
	static CTimer*	Create();
	virtual void	Free();
};

END