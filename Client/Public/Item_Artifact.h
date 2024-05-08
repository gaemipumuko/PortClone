#pragma once

#include "Client_Defines.h"
#include "Item.h"

BEGIN(Client)

class CItem_Artifact final : public CItem
{
public:
	enum ARTIFACT_TYPE { ATYPE_FLOWER, ATYPE_FEATHER, ATYPE_WATCH, ATYPE_GRAIL, ATYPE_CROWN, ATYPE_END };

private:
	CItem_Artifact(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem_Artifact(const CItem_Artifact& rhs);
	virtual ~CItem_Artifact() = default;

private:
	_int				m_iBaseDamage = { };
	_int				m_iHitPoint = { };
	_float				m_fDefenceRate = {};
	_float				m_fCritRate = {};
	_float				m_fCritDmg = {};

	ARTIFACT_TYPE		m_eArtifactType = { ATYPE_END };

public:
	_int			Get_WeaponType() const { return static_cast<_int>(m_eArtifactType); }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render(_float2 fPos, _float2 vRenderScale = { 100.f,128.f }) override;

	HRESULT Render_Detail(_float4x4 vItemMatrix);
	virtual void	Use_item(_uint iQty, PLAYERINDEX ePlayerIndex) override;

public:
	static CItem_Artifact*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END