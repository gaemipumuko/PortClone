#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CLoader final : public CBase
{
private:
	CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

private:
	CGameInstance*			m_pGameInstance = { nullptr };

private:
	LEVELID				m_eNextLevelID = { LEVEL_END };
	_bool				m_bFinished = { FALSE };
	_float				m_fLoadRatio = { };

public:
	_bool				Get_Finished() { return m_bFinished; }
	void				Set_Finished(_bool bFinished) { m_bFinished = bFinished; }

	_float				Get_LoadRatio() { return m_fLoadRatio; }
	void				Set_LoadRatio(_float fRatio) { m_fLoadRatio = fRatio; }

private:
	HANDLE				m_hThread = { 0 };
	CRITICAL_SECTION	m_Critical_Section = { };

public:
	HRESULT Begin_Thread();
	HRESULT Loading_Resources();
	HRESULT End_Thread();

private:
	HRESULT Loading_Entry_Resources();
	HRESULT Loading_World_Resources();
	HRESULT Loading_Temple_Lion_Resources();
	HRESULT Loading_StrongHold_AbyssMage_Resources();
	HRESULT Loading_Quest_Resources();
	HRESULT Loading_StormTerror_Resources();

public:
	HRESULT Initialize(LEVELID eLevel);

public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eLevel);
	virtual void	Free() override;
};

END