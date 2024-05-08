#pragma once

#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CDoor_Portal final : public CEffect
{
private:
	CDoor_Portal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDoor_Portal(const CDoor_Portal& rhs);
	virtual ~CDoor_Portal() = default;
	 
private:
	CShader*		m_pShaderCom = { nullptr };
	CTexture*		m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	
private:
	_float4x4		m_WorldMatrix = {};

	// ����
	_float			m_fLifeTime = 0.f;
	_vector			m_vScale = {1.f,1.f,0.f,0.f};
	_vector			m_vColor = {};

	_int			m_iGrade = {};

	// Shader ����
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
	void	Set_Matrix(_matrix vWorldMat);
	void	Set_Color(_vector vColor) { m_vColor = vColor; }

public:
	static CDoor_Portal*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END