#pragma once

#include "Client_Defines.h"
#include "Mini_Game.h"

#include "Gliding_Ring.h"
#include "Accelerating_Ring.h"
#include "Updraft.h"

BEGIN(Client)

class CGliding_Test final : public CMini_Game
{
protected:
	CGliding_Test();
	CGliding_Test(const CGliding_Test& rhs);
	virtual ~CGliding_Test() = default;

private:
	_float			m_fStartTime = { };
	_bool			m_bStartGame = { FALSE };

	_bool			m_bTryAgain = { FALSE };

	_int			m_iCreateNum = {};

private:
	vector<CGliding_Ring*>			Gliding_Ring;
	vector<CAccelerating_Ring*>		Accelerating_Ring;
	vector<CUpdraft*>				UpDraft;

private:
	HRESULT			Create_Updraft();
	HRESULT			Create_Ring();
	HRESULT			Create_AddRing();
	HRESULT			Create_AcceleratingRing();

public:
	_bool			Get_TryAgain() { return m_bTryAgain; }
	void			Set_TryAgain(_bool bTryAgain) { m_bTryAgain = bTryAgain; }
	void			Release_Object();

public:
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Update(_float fTimeDelta) override;

public:
	static CGliding_Test* Create(void* pArg);
	virtual void	Free() override;
};

END