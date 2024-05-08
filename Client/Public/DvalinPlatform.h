#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CTransform;
class CModel;
class CGameObject;
class CGameInstance;
#ifdef _DEBUG
class CShader;
#endif
END

BEGIN(Client)

class CDvalinPlatform final : public CBase
{
private:
	CDvalinPlatform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CDvalinPlatform() = default;

public:
	HRESULT Initialize();

public:
	HRESULT Move_ToPlatform(_float4 vPosition, _float4 vDirection);
	HRESULT Look_Platform(_float4 vDirection);

#ifdef _DEBUG
	HRESULT Render();
#endif

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };

private:
	CTransform* m_pTransformCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	PxRigidStatic* m_pRigidStatic = {nullptr};

#ifdef _DEBUG
	CShader* m_pShaderCom = { nullptr };
#endif

public:
	static CDvalinPlatform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
