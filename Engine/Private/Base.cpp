#include "Base.h"

CBase::CBase()
{
}

_ulong CBase::AddRef()
{
	return ++m_dwRefCnt;
}

_ulong CBase::Release()
{
	if (m_dwRefCnt == 0)
	{
		Free();
		delete this;
		return 0;
	}

	return m_dwRefCnt--;
}

void CBase::Free()
{
}