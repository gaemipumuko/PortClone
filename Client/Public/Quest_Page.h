#pragma once

#include "Client_Defines.h"
#include "Ortho.h"

BEGIN(Client)

class CQuest_Page : public COrtho
{
private:
	enum QUEST_TAB {TAB_ALL,TAB_ACCEPTED,TAB_COMPLETED,TAB_END};

private:
	CQuest_Page(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CQuest_Page(const CQuest_Page& rhs);
	virtual ~CQuest_Page() = default;

private:
	_int				m_iMOverIndex = { -1 };

	vector<_float4x4>	m_IconMatrix = { };

private:
	HRESULT Add_Component(void* pArg);
	HRESULT Bind_ShaderResources();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CQuest_Page*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END