#pragma once

#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CImpact_Glow final : public CEffect
{
public:
	enum SEQUENCE_DIR { DIR_FROM_RIGHT, DIR_FROM_LEFT, DIR_RANDOM, DIR_END };
	enum EFF_PLANE_DIR { EFF_PLANE_RIGHT, EFF_PLANE_REVERSED, EFF_PLANE_LEFT, EFF_PLANE_END };
public:
	typedef struct tagImpactDesc :EFFECT_DESC
	{
		_int			iRotation = {};
		_int			iRotAxisY = {};
		_int			iFanSize = {60};
		_int			iNumSpike = {};
		_float			fRadius = {};
		SEQUENCE_DIR	eSequenceDir = { DIR_END };
		EFF_PLANE_DIR	ePlaneDir = { EFF_PLANE_END };
		ELEMENT			eELEMENT = {ELEMENT_END};
	}IMPACT_DESC;
private:
	CImpact_Glow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CImpact_Glow(const CImpact_Glow& rhs);
	virtual ~CImpact_Glow() = default;
	 
private:
	CShader*			m_pShaderCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom = { nullptr };
	
private:
	_float4x4			m_WorldMatrix = {};

	//이펙트 제어
	vector<_int>		m_vecRotation = { };
	vector<_float4x4>	m_vecGlowMat = {};
	vector<_float>		m_vecTimeAcc = {};
	vector<_float2>		m_vecUVPos = {};
	
	_int				m_iNumGlow = {};
	_int				m_iFanAngle = {60};
	_int				m_iRotationY = {};

	//Sequence
	SEQUENCE_DIR		m_eSequenceDir = { DIR_END };
	EFF_PLANE_DIR		m_ePlaneDir = { EFF_PLANE_END };
	_int				m_iSequeceIdx = {};

	_float				m_fRadius = {};
	_float				m_fSpacing = {0.f};

	// Shader 관련
	_vector				m_vElementColor = {};
	ELEMENT				m_eElement = {ELEMENT_END};

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
	static CImpact_Glow*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END