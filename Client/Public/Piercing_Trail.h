#pragma once

#include "Client_Defines.h"
#include "DeciderObject.h"
#include "Creature.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CModel;
END

BEGIN(Client)

class CPiercing_Trail final : public CDeciderObject
{
public:
	typedef struct tagPiercing_Desc
	{
		CModel*			pModelCom;
		_float4x4*		pCombineWorldMatrix;
		_float			fTrailSpeed;
		_float3			vScale;
		_bool			bRotation;
	}PIERCING_DESC;

private:
	CPiercing_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPiercing_Trail(const CPiercing_Trail& rhs);
	virtual ~CPiercing_Trail() = default;
	 
private:
	CShader*		m_pShaderCom = { nullptr };
	CTexture*		m_pTextureCom = { nullptr };
	CRenderer*		m_pRendererCom = { nullptr };
	CTransform*		m_pTransformCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	CModel*			m_pParentModelCom = { nullptr };
	
private:
	_float4x4*		m_pParentCombineWorldMatrix = { nullptr };

	_float3			m_vScale = {};
	_float			m_fTrailSpeed = { 0.f };
	_float			m_fLifeTime = { 0.f };
	_float			m_fTimeAcc = 0.f;

private:
	HRESULT	Add_Component(void* pArg);
	HRESULT Bind_ShaderResources();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CPiercing_Trail*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END