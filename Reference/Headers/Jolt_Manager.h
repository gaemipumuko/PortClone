#pragma once

#include "Base.h"

BEGIN(Engine)

class CJolt_Manager final : public CBase
{
private:
	CJolt_Manager();
	virtual ~CJolt_Manager() = default;

public:
	HRESULT Initialize();
	
public:
	static CJolt_Manager*	Create();
	virtual void			Free();
};

END
