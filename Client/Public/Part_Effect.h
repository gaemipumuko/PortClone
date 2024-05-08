#pragma once
class CPart_Effect
{
};

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Client)
class CFiniteStateMachine;
END

BEGIN(Engine)
class CBone;
class CModel;
class CShader;
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CPart_Weapon abstract : public CPartObject
{
public:
	typedef struct tagPartWeaponDesc
	{
		class CModel* pBody;
		class CBone* pBone;

		class CTransform* pParentTransform;
		class CFiniteStateMachine* pFSMCom;
		RECEIVED_COM		tModelComponent;
	} PARTWEAPON_DESC;

protected:
	CPart_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPart_Weapon(const CPart_Weapon& rhs);
	virtual ~CPart_Weapon() = default;

protected:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CFiniteStateMachine* m_pParentFSMCom = { nullptr };

protected:
	CTransform* m_pTransformCom = { nullptr };
	CTransform* m_pParentTransformCom = { nullptr };

	_float4x4		m_WorldMatrix = { };

protected:
	virtual HRESULT	Invalidate_Animation(_float fTimeDelta) PURE;

protected:
	HRESULT Add_Component(void* pArg);
	HRESULT Bind_ShaderResources();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static vector<CPart_Weapon*>	Create_Weapons(CREATUREID eID, PARTWEAPON_DESC tWeaponDesc);
	virtual void					Free() override;
};

END