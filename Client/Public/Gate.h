#pragma once

#include "Client_Defines.h"
#include "EventCollider.h"

BEGIN(Engine)
class CTransform;
class CRenderer;
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)

class CGate final : public CEventCollider
{
public:
	enum GATE_TYPE { GATE_VISION, GATE_DVALIN, GATE_END };
	enum ENTRY_TEXTURE
	{ 
		ENTRY_GROUNDBLUR,
		ENTRY_RAMPSTAR,
		ENTRY_MULTICHANNEL,
		ENTRY_NOISEMULTI,
		ENTRY_TURBULENCE,
		ENTRY_END
	};

	typedef struct tagGateDesc 
	{
		GATE_TYPE eGateType;
		_float4x4 vWorldMatrix;
		wstring szModelTag;
		LEVELID eLevel;
		LEVELID eReserveLevel;
	}GATE_DESC;

private:
	CGate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGate(const CGate& rhs);
	virtual ~CGate() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Reserve_Level();

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Add_ShaderCom();
	HRESULT Initialize_Animation();
	_bool isInPlayer();
	HRESULT Tick_VisionGate(_float fTimeDelta);
	HRESULT Tick_DvalinGate(_float fTimeDelta);
	HRESULT Render_VisionGate();
	HRESULT Render_DvalinGate();

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

private: /* For.DvalinEntry */
	CTexture* m_pTextureCom[ENTRY_END] = {};

private:
	GATE_TYPE		m_eGateType = GATE_END;
	_float4x4		m_vWorldMatrix;
	wstring			m_szModelTag;
	LEVELID			m_eLevel = LEVEL_END;
	LEVELID			m_eReserveLevel = LEVEL_END;

	_uint			m_iModelType;
	_bool			m_bReserveLevel = false;

	_bool			m_bEntry = false;
	_bool			m_bLoop = false;

	_bool			m_bVertical = {false};
	_float			m_fTimeAcc = { };

private:
	class CInteraction_Box* m_pInteractionBox = { nullptr };

public:
	static CGate* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END
