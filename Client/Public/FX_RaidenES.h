#pragma once

#include "Client_Defines.h"
#include "AssetFX.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CFX_RaidenES final : public CAssetFX
{
	enum RAIDENES { RES_LINE, RES_EYE, RES_END };

public:
	typedef struct tagRaidenESDesc : public ASFX_DESC
	{
		_float4	vLookVector = { };
	} RAIDENES_DESC;

private:
	CFX_RaidenES(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFX_RaidenES(const CFX_RaidenES& rhs);
	virtual ~CFX_RaidenES() = default;

private:
	CModel*				m_pModelCom = { nullptr };
	CShader*			m_pShaderCom[RES_END] = {nullptr};
	CRenderer*			m_pRendererCom = { nullptr };
	CTransform*			m_pTransformCom[RES_END] = {nullptr};
	CVIBuffer_Rect*		m_pVIBufferCom = { nullptr };

private:
	_float4	m_vFrame = { };

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
	static CFX_RaidenES*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END