#pragma once

#include "Animation_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CTransform;
END

BEGIN(AnimationTool)

class CMannequin final : public CGameObject
{
private:
	CMannequin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMannequin(const CMannequin& rhs);
	virtual ~CMannequin() = default;

private:
	CModel*					m_pModelCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };
	CRenderer*				m_pRendererCom = { nullptr };
	CTransform*				m_pTransformCom = { nullptr };

private:
	_float4	m_vRootTranslation = { };

	_bool	m_bPlayAnimation = { FALSE };
	_uint	m_iAnimationIndex = { };
	_float	m_fAnimationSpeed = { 1.f };

public:
	_bool	Get_PlayAnimation() { return m_bPlayAnimation; }
	void	Set_PlayAnimation(_bool bPlayAnim) { m_bPlayAnimation = bPlayAnim; }

	_uint	Get_AnimationIndex() { return m_iAnimationIndex; }
	void	Set_AnimationIndex(_uint iAnimIndex) { m_iAnimationIndex = iAnimIndex; }

	_float	Get_AnimationSpeed() { return m_fAnimationSpeed; }
	void	Set_AnimationSpeed(_float fAnimationSpeed) { m_fAnimationSpeed = fAnimationSpeed; }

public:
	HRESULT	Bind_Animation(_uint iAnimIndex);
	HRESULT	Bind_SpecificModel(LEVELID eID, wstring strPrototypeTag);

protected:
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
	static CMannequin*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END