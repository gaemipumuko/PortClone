#pragma once
#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class CInteraction_Manager final : public CBase
{
private:
	CInteraction_Manager();
	virtual ~CInteraction_Manager() = default;

public:
	void		Add_InteractionCount(wstring strType, _int iCount = 1);
	void		Register_Interaction(wstring strType);
	void		Unregister_Interaction(wstring strType);

	_bool		isInteraction(wstring strType);

	_int		Get_InteractionCount(wstring strType);
	void		Set_InteractionCount(wstring strType, _int iCount = 0);

private:
	map<wstring, _int> m_Interaction;

public:
	HRESULT Initialize();

public:
	static			CInteraction_Manager* Create();
	virtual void	Free() override;
};

END