#pragma once

#include "Base.h"

BEGIN(Engine)

class CRandom_Manager final : public CBase
{
private:
	CRandom_Manager();
	virtual ~CRandom_Manager() = default;

private:
	mt19937_64	m_Generator;

public:
	_int	Get_Int_Random(_int iStart, _int iEnd);
	_float	Get_Float_Random(_float fStart, _float fEnd);
	_vector Get_RandomPos_InCircle(_vector vCenterPos, _float fRadius, _vector vSelfPos = {});
public:
	HRESULT Initialize();

public:
	static CRandom_Manager*	Create();
	virtual void			Free();
};

END