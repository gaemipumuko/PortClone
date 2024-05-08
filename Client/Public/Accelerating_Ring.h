#pragma once
#include "Client_Defines.h"
#include "Creature.h"

BEGIN(Engine)
class CModel;
class CShader;
END

BEGIN(Client)
class CAccelerating_Ring final : public CCreature
{
public:
	typedef struct tagAcceleratingRingDesc
	{
		_vector			vPos;
		_float			fRadian;
	}ACCELERATING_DESC;

private:
	CAccelerating_Ring(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAccelerating_Ring(const CAccelerating_Ring& rhs);
	virtual ~CAccelerating_Ring() = default;

private:
	_float4x4			m_WorldMatrix = {};

private:
	_float				m_fAcceleration = {  };

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };

private:
	HRESULT Add_Components(void* pArg);
	HRESULT Bind_ShaderResources();

private:
	virtual HRESULT Add_Parts(void* pArg);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CAccelerating_Ring*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};
END