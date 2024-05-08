#pragma once

#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CEff_Effigy_Recharge_Trail final : public CEffect
{
private:
	CEff_Effigy_Recharge_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEff_Effigy_Recharge_Trail(const CEff_Effigy_Recharge_Trail& rhs);
	virtual ~CEff_Effigy_Recharge_Trail() = default;
	 
private:
	CShader*		m_pShaderCom = { nullptr };
	CTexture*		m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	
private:
	_float4x4		m_WorldMatrix = {};

	// 제어
	_float			m_fLifeTime = 0.f;
	_vector			m_vScale = {1.f,1.f,0.f,0.f};
	_vector			m_vColor = {};

	_int			m_iGrade = {};

	// Shader 관련
	_float			m_fTimeAcc = 0.f;
	_float			m_fEmittionTime = {0.f};


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
	void	Set_Matrix(_matrix vWorldMat);
	void	Set_Color(_vector vColor) { m_vColor = vColor; }

public:
	static CEff_Effigy_Recharge_Trail*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END