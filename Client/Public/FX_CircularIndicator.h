#pragma once

#include "Client_Defines.h"
#include "AssetFX.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CFX_CircularIndicator final : public CAssetFX
{
public:
	enum INDICATOR { IND_GAGE, IND_BORDER, IND_END };
	typedef struct tagCircularIndicatorDesc : public ASFX_DESC
	{
		_float	fDistance = { 0.f };
		_float	fScale = {};
	} CIND_DESC;

private:
	CFX_CircularIndicator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFX_CircularIndicator(const CFX_CircularIndicator& rhs);
	virtual ~CFX_CircularIndicator() = default;

private:
	CModel*				m_pModelCom[IND_END] = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CTransform*			m_pTransformCom = { nullptr };

	CTransform*			m_pMeteoriteTransformCom = { nullptr };

private:
	_float				m_fFrame = { };

	_float				m_fDistance = { };

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
	static CFX_CircularIndicator*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg);
	virtual void					Free() override;
};

END