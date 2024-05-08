#pragma once

#include "Client_Defines.h"
#include "PartObject.h"
#include "Player.h"
#include "CharacterController.h"

BEGIN(Engine)
class CModel;
class CShader;
class CTexture;
class CCollider;
class CRenderer;
class CTransform;
class CVIBuffer_Trail;
class CFiniteStateMachine;
END

BEGIN(Client)

class CPart_Body abstract : public CPartObject
{
public:
	typedef struct tagPartBodyDesc
	{
		RECEIVED_COM			tModelComponent;
		PARTOBJ_DESC			tPartObjDesc;
		CREATUREID				tCreatureID;
		CREATURETYPE			eType;
		
		CTransform*				pTransform;
		CFiniteStateMachine*	pFiniteStateMachineCom;
		CCharacterController*	pController;
	} PARTBODY_DESC;

protected:
	CPart_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPart_Body(const CPart_Body& rhs);
	virtual ~CPart_Body() = default;

protected:
	CModel*					m_pModelCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };
	CCollider*				m_pColliderCom = { nullptr };
	CRenderer*				m_pRendererCom = { nullptr };
	CFiniteStateMachine*	m_pParentFSMCom = { nullptr };

	// Trail
	CShader*				m_pTrailShaderCom = { nullptr };
	CTexture*				m_pTrailTextureCom = { nullptr };
	CVIBuffer_Trail*		m_pTrailBufferCom = { nullptr };
	CVIBuffer_Trail*		m_pTrailBufferCom2 = { nullptr }; 

	// Face Shadow
	CTexture*				m_pFaceShadowTexCom = { nullptr };

protected:
	CTransform*						m_pTransformCom = { nullptr };
	CTransform*						m_pParentTransformCom = { nullptr };
	class CCharacterController*		m_pParentController = { nullptr };
	
protected:
	_int					m_iColliderID = { -1 };

	_float4x4				m_CombinedWorldMatrix = { };	/* Transform * ParentTransform */

protected:
	CREATUREID		m_eCreatureID = CREATURE_END;
	_int			m_iFaceMeshIndex = { -1 };
	_int			m_iFaceEyeMeshIndex = { -1 };

	_float					m_fReceiveDamage = { 0.f };
	FSM_OBJECT_STATE		m_eReceiveState = { FSM_OBJECT_END };

	_float4			m_vBoneWorldPosition = {};

	/* Element */
	ELEMENT					m_eReceiveElement = { ELEMENT_END };
	ELEMENTAL_REACTION		m_eElementalReaction = { REACT_END };
	_uint					m_bHostElement = { 0b00000000 };

	/* Anim */
	_bool		m_bChangeAnim = { FALSE };
	
protected:
	virtual HRESULT	Invalidate_Animation(_float fTimeDelta) PURE;

protected:
	HRESULT Add_Component(void* pArg);
	HRESULT Bind_ShaderResources();
	virtual HRESULT Bind_ShaderResources_Trail();

protected:
	void	Set_ControllerTranslation(_vector vCurrentTranslation);
	void	Set_EtcTranslation(_vector vCurrentTranslation);
	void	Player_Hit(_vector vDir);
	void	Set_FaceIndex(_int iIndex) { m_iFaceMeshIndex = iIndex; }

	HRESULT	ElementalReaction();

public:
	_bool Get_ChangeAnim() { return m_bChangeAnim; }

public:
	_bool	isFunctionTrue();

public:
	_float4x4		Get_CombinedMatrix() { return m_CombinedWorldMatrix; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Trail() override;
	virtual HRESULT Render_Shadow(_float4x4* pLightViewProj, _int iNumArray);

public:
	virtual _int				Get_CollisionID() override { return m_iColliderID; }
	virtual _float				Get_Damage() override;
	virtual FSM_OBJECT_STATE	Get_ReceiveState() override;
	virtual ELEMENT				Get_ReceiveElement() override;
	virtual ELEMENTAL_REACTION	Get_ElementalReaction() override;

	virtual void				Set_Damage(_float fDamage, ELEMENT eElement = ELEMENT_END, FSM_OBJECT_STATE eForceState = FSM_OBJECT_HIT_H, _bool isPetrification = FALSE) override;

	virtual _bool				isHostElement(ELEMENT eElement) override; // ������ �������� �˻�

public:
	_vector Get_Position();

	void		Set_CreatureID(CREATUREID eID);
	void		Set_HostElement(ELEMENT eElement, _bool bSet = TRUE);
	CREATUREID	Get_CreatureId() { return m_eCreatureID; }
	_vector		Get_BodyWorldPosition();

public:
	static CPart_Body*		Create_Body(CREATUREID eID, PARTBODY_DESC tBodyDesc);
	virtual void			Free() override;
};

END