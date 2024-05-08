#pragma once

#include "Client_Defines.h"
#include "SkillObj.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CLightning final : public CSkillObj
{
private:
	CLightning(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLightning(const CLightning& rhs);
	virtual ~CLightning() = default;

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };

private:
	_uint m_iMesIndex = {};
	_float m_fFrame = { 0.f };
	_float m_fSoundDelay = { 0.f };

	_float m_fLifeTime = { 0.f };

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
	static CLightning*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;


};

END