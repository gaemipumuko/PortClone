#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CDamage_Font final : public CBase
{
public:
	typedef struct tagDamagefont
	{
		_vector		vWorldPos;
		_int		iDamage;
		_vector		vColor;
		wstring		strAilments;

	}DMGFONT_DESC;
private:
	CDamage_Font();
	CDamage_Font(const CDamage_Font& rhs);
	virtual ~CDamage_Font() = default;

private:
	_bool		m_bDead = { false };
	_int		m_iDamage = { 0 };
	_float		m_fTimeAcc = { 0.f };
	_float4		m_vWorldPos = {};
	_float2		m_vScreenPos = {};
	_float2		m_fPosRange = {};

	wstring		m_strAilments = {};
	_vector		m_vColor = {};

protected:
	class CGameInstance* m_pGameInstance = { nullptr };

public:
	_bool	Get_Dead() { return m_bDead; }

public:
	 HRESULT Initialize_Prototype(void* pArg);

	 HRESULT PreviousTick(_float fTimeDelta) ;
	 HRESULT Tick(_float fTimeDelta) ;
	 HRESULT LateTick(_float fTimeDelta) ;
	 HRESULT Render() ;
	
public:
	static CDamage_Font*	Create(void* pArg);
	virtual void			Free() override;
};

END