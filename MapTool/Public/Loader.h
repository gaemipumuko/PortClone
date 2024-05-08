#pragma once

#include "Tool_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Tool)

class CLoader final : public CBase
{
private:
	CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

private:
	CGameInstance* m_pGameInstance = { nullptr };

private:
	LEVELID				m_eNextLevelID = { LEVEL_END };
	_bool				m_bFinished = { FALSE };

public:
	_bool				Get_Finished() { return m_bFinished; }
	void				Set_Finished(_bool bFinished) { m_bFinished = bFinished; }

private:
	HANDLE				m_hThread = { 0 };
	CRITICAL_SECTION	m_Critical_Section = { };

public:
	HRESULT Begin_Thread();
	HRESULT Loading_Resources();
	HRESULT End_Thread();

private:
	HRESULT Loading_Edit_Resources();
	HRESULT Loading_Model_Resources(wstring strDataPath);

public:
	HRESULT Initialize(LEVELID eLevel);

public:
	static CLoader* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, LEVELID eLevel);
	virtual void	Free() override;
};

END
