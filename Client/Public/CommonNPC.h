#pragma once

#include "Client_Defines.h"
#include "Creature.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CCollider;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CCommon_NPC final : public CCreature
{
public:
	typedef struct tagNPCDesc
	{
		_vector			vInitPos;
		_float			fAngle;
		CREATUREID		eObjectType;
		NPCID			eNPCType;
	} NPC_DESC;

private:
	CCommon_NPC(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCommon_NPC(const CCommon_NPC& rhs);
	virtual ~CCommon_NPC() = default;

private:
	typedef struct tagPartySlots
	{
		class CPartObject*	pPartySlot = { };
	} PARTY_SLOTS;

	enum PART { PART_BODY, PART_END };

	vector<PARTY_SLOTS>	m_Parts;

private:
	NPCID		m_eNPCId = { NPC_END };

private: /* Walk */
	_float		m_fWalkTime = {};
	_bool		m_bWalk = { FALSE };
	_vector		m_vInitPos = {};

private:
	class CAlphaRect* m_pOverHeadUI = { nullptr };
	class CInteraction_Box* m_pInteractionBox = { nullptr };
	wstring	m_strNPCName = {};
	wstring	m_strNPCDetail = {};

private:
	HRESULT		NPC_Action(_float fTimeDelta);

	HRESULT		Standby_Action(_float fTimeDelta);
	HRESULT		Pray_Action(_float fTimeDelta);
	HRESULT		Clap_Action(_float fTimeDelta);
	HRESULT		Solider_Action(_float fTimeDelta);
	HRESULT		SitHand_Action(_float fTimeDelta);
	HRESULT     Akimbo_Action(_float fTimeDelta);
	HRESULT		HoldArmHand_Action(_float fTimeDelta);
	HRESULT		HoldArm_Action(_float fTimeDelta);
	HRESULT		Talk_Action(_float fTimeDelta, _int iAnim);
	HRESULT     Request_Action(_float fTimeDelta);
	HRESULT		Knight_Action(_float fTimeDelta);
	HRESULT		Walk_Action(_float fTimeDelta);

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
	static CCommon_NPC*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END