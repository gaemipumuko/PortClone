#pragma once

#include "Client_Defines.h"
#include "SkillObj.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
class CTransform;
class CTexture;
END

BEGIN(Client)

class CSkill_Abyss_Mage_Shield final : public CSkillObj
{
public:
	typedef struct tagAbyssMageShieldDesc
	{
		wstring strTag;
		SKILLOBJ_DESC tSkillDesc;
	}ABYSS_MAGE_SHILED_DESC;

private:
	CSkill_Abyss_Mage_Shield(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_Abyss_Mage_Shield(const CSkill_Abyss_Mage_Shield& rhs);
	virtual ~CSkill_Abyss_Mage_Shield() = default;

private:
	CShader*			m_pShaderCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CModel*				m_pModelCom = { nullptr };
	CTransform*			m_pTransformCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };
	CTexture*			m_pCrackTextureCom = { nullptr };

private:
	_float4x4			m_WorldMatrix = {};

	wstring				m_fTag;

	_float				m_fCrackDisplay = { 0.f };

	_int				m_CrackIndex = {};

	_bool				m_bDead = { FALSE };

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
	static CSkill_Abyss_Mage_Shield*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END