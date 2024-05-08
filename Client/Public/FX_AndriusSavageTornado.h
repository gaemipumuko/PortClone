#pragma once

#include "Client_Defines.h"
#include "AssetFX.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CFX_AndriusSavageTornado final : public CAssetFX
{
	typedef struct tagTornadoDesc
	{
		static _float	fMaxLifeTime;
		enum DIRECTION { DIR_LT, DIR_RT, DIR_LB, DIR_RB, DIR_END };

		wstring		strFXTag;
		_float		fLifeTime;
		_bool		bTrailDestroy;

		CTransform* pTransform;
		DIRECTION	eDirection;
	} TORNADO_DESC;

private:
	CFX_AndriusSavageTornado(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFX_AndriusSavageTornado(const CFX_AndriusSavageTornado& rhs);
	virtual ~CFX_AndriusSavageTornado() = default;

private:
	CTransform*			m_pTransformCom = { nullptr };

	list<TORNADO_DESC>	m_TornadoTransforms;

private:
	_float4				m_vFrame = { };

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
	static CFX_AndriusSavageTornado*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*				Clone(void* pArg);
	virtual void						Free() override;
};

END