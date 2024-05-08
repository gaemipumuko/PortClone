#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CMini_Game abstract : public CBase
{
public:
	typedef struct tagMiniGameDesc
	{
		wstring			strMiniGameName;
		_float			fTotalTime;
	}MINIGAME_DESC;

protected:
	CMini_Game();
	CMini_Game(const CMini_Game& rhs);
	virtual ~CMini_Game() = default;

protected:
	wstring			m_strMiniGameName;

	/* 미니게임 제한시간 */
	_float			m_fTotalTime = {};
	_float			m_fCurTime = {};

protected: /* Clear 여부 */
	_bool			m_bClearGame = { FALSE };

public:
	_bool			Get_ClearGame() { return m_bClearGame; }

public:
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Update(_float fTimeDelta) PURE;

public:
	virtual void	Free() override;
};

END