#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CAssetFX abstract : public CGameObject
{
public:
	typedef struct tagAssetFXDesc
	{
		_float	fDuration;

		/* À§Ä¡ */
		enum POSTYPE		{ POS_FIX, POS_RELATIVE, POS_END };
		POSTYPE				ePosType = { POS_END };
		_vector				vInitPosition = { };
		class CTransform*	pTransform = { nullptr };
	} ASFX_DESC;

protected:
	CAssetFX(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAssetFX(const CAssetFX& rhs);
	virtual ~CAssetFX() = default;

protected:
	_bool				m_bDuration = { FALSE };
	_float				m_fDuration = { -1.f };
	class CTransform*	m_pRelativeTransform = { nullptr };

public:
	void	Set_IsTimeFX(_bool bDuration) { m_bDuration = bDuration; }
	_bool	Get_IsTimeFX() { return m_bDuration; }

	void	Set_Duration(_float fDuration) { m_fDuration = fDuration; }
	_float	Get_Duration() { return m_fDuration; }

public:
	virtual HRESULT Initialize_Prototype() PURE;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) PURE;
	virtual HRESULT LateTick(_float fTimeDelta) PURE;
	virtual HRESULT Render() PURE;

public:
	virtual CGameObject*	Clone(void* pArg) PURE;
	virtual void			Free() override;
};

END