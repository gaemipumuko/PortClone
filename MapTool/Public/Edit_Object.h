#pragma once

#include "Tool_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTransform;
class CRenderer;
class CShader;
class CCollider;
END

BEGIN(Tool)

class CEdit_Object final : public CGameObject
{
public:
	typedef struct tagEditObjectDesc 
	{
		WORLD_ELEMENT_TYPE eType;
		_float4x4 vWorldMatrix;
		wstring strModelTag;
	}EDIT_DESC;

private:
	CEdit_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEdit_Object(const CEdit_Object& rhs);
	virtual ~CEdit_Object() = default;

public:
	WORLD_ELEMENT_TYPE Get_Type() { return m_eType; }
	_float4x4 Get_WorldMatrix();
	wstring Get_Tag() { return m_strModelTag; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components(EDIT_DESC* pEditDesc);
	HRESULT Bind_ShaderResources();

private:
	CTransform* m_pTransformCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	class CEdit_Model* m_pModelCom = { nullptr };

private:
	WORLD_ELEMENT_TYPE m_eType = WORLD_ELEMENT_END;
	wstring m_strModelTag;

public:
	static CEdit_Object* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
