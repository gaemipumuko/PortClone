#pragma once

#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CEff_Smoke final : public CEffect
{
public:
	enum SMOKE_TYPE { SMOKE_TYPE_ONE, SMOKE_TYPE_TWO, SMOKE_TYPE_THREE, SMOKE_TYPE_FOUR, SMOKE_TYPE_END};
private:
	CEff_Smoke(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEff_Smoke(const CEff_Smoke& rhs);
	virtual ~CEff_Smoke() = default;
	 
private:
	CShader*		m_pShaderCom = { nullptr };
	CTexture*		m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	
private:
	_float4x4		m_WorldMatrix = {};

	// 제어
	_float			m_fLifeTime = 0.f;
	_float2			m_fPos = {};
	_vector			m_vScale = {1.f,1.f,0.f,0.f};

	// Shader 관련
	_float			m_fTimeAcc = 0.f;
	_float			m_fColorRate0 = 0.f;

	SMOKE_TYPE		m_eSmokeType = { SMOKE_TYPE_ONE };

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
	static CEff_Smoke*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END