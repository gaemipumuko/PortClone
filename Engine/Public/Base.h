#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
protected:
	CBase();
	virtual ~CBase() = default;

private:
	_ulong m_dwRefCnt = { 0 };

public:
	_ulong AddRef();
	_ulong Release();

public:
	virtual void Free();
};

END