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

class CTelePort final : public CCreature
{
public:
	enum TELEPORTID { TELEPORT_CASTLE, TELEPORT_OPENWORLD, TELEPORT_GATE, TELEPORT_DVALIN, TELEPORT_EFFIGYFIRE, TELEPORT_END };
public:
	typedef struct tagTelePortDesc
	{
		TELEPORTID		eTelePortId;
		_vector			vInitPos;
	} TELEPORT_DESC;

private:
	CTelePort(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTelePort(const CTelePort& rhs);
	virtual ~CTelePort() = default;

private:
	typedef struct tagPartySlots
	{
		class CPartObject*	pPartySlot = { };
	} PARTY_SLOTS;

	enum PART { PART_BODY, PART_END };

	vector<PARTY_SLOTS>	m_Parts;

private:
	class CInteraction_Box* m_pInteractionBox = { nullptr };

private:
	TELEPORTID					m_eTelePortId = { TELEPORT_END };
	static vector<_bool>		m_vecInteraction;

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
	static CTelePort*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END