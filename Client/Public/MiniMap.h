#pragma once

#include "Client_Defines.h"
#include "Ortho.h"


BEGIN(Client)

class CMini_Map : public COrtho
{
public:
	enum MAP_ICON { MAP_MONSTER, MAP_NPC, MAP_QUEST,MAP_SHOP,MAP_ALCHEMY,MAP_COOKING, MAP_END };

private:
	CMini_Map(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMini_Map(const CMini_Map& rhs);
	virtual ~CMini_Map() = default;

private:
	_int					m_iNumObjectOnMap = { 0 };

	_float2					m_fIconSize = {20.f,20.f};

	_float4x4				m_vecBoundaryMat;
	_float4x4				m_vPlayerIconMat;
	_float4x4				m_vPlayerOnMapMat;
	_float4x4				m_vCamMat;
	_float4x4				m_vCamOnMapMat;
	vector<_float4x4>		m_vecObj_OnMapMat;

	vector<CTransform*>		m_vecTargetObject;
	vector<MAP_ICON>		m_vecTargetIcon;

	_vector					m_vPlayerPos;

	CTexture*				m_pMapIconCom = {nullptr};

	CTransform*				m_pPlayerTransform = { nullptr };

private:
	HRESULT Add_Component(void* pArg);
	HRESULT Bind_ShaderResources();

public:
	void	Register_OnMap(CTransform* TargetTransform, MAP_ICON eIcon = MAP_END);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CMini_Map*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;


};

END