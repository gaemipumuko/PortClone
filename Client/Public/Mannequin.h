#pragma once

#include "Client_Defines.h"
#include "DeciderObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CMannequin final : public CDeciderObject
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
	CTexture*				m_pDissolveCom = { nullptr };

private:
	FACEID	m_eFaceID = { };

	_float4	m_vRootTranslation = { };

	_bool	m_bPlayAnimation = { FALSE };
	_uint	m_iAnimationIndex = { };
	_float	m_fAnimationSpeed = { 1.f };

	_float				m_fDissolve = { 0.f };
	_float				m_fDissolveSize = { 0.05f };
	_float3				m_vDissolveColor = {};
	_bool				m_bForceDissolve = { FALSE };

	_bool	m_bChangeModel = { FALSE };

public:
	_bool	Get_PlayAnimation() { return m_bPlayAnimation; }
	void	Set_PlayAnimation(_bool bPlayAnim) { m_bPlayAnimation = bPlayAnim; }

	_uint	Get_AnimationIndex() { return m_iAnimationIndex; }
	void	Set_AnimationIndex(_uint iAnimIndex) { m_iAnimationIndex = iAnimIndex; }

	_float	Get_AnimationSpeed() { return m_fAnimationSpeed; }
	void	Set_AnimationSpeed(_float fAnimationSpeed) { m_fAnimationSpeed = fAnimationSpeed; }

	void Set_Dissolve(_bool bDissolve) { m_bForceDissolve = bDissolve; }

public:
	HRESULT	Bind_Animation(_uint iAnimIndex);
	HRESULT	Bind_SpecificModel(LEVELID eID, wstring strPrototypeTag);
	HRESULT Change_Effects();

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



