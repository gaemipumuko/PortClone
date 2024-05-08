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

class CPaimon final : public CCreature
{
private:
	CPaimon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPaimon(const CPaimon& rhs);
	virtual ~CPaimon() = default;

private:
	typedef struct tagPartySlots
	{
		class CPartObject*	pPartySlot = { };
	} PARTY_SLOTS;

	enum PART { PART_BODY, PART_END };

	vector<PARTY_SLOTS>	m_Parts;

private:
	virtual HRESULT Add_Parts(void* pArg);

private:
	HRESULT		Follow_Player(_float fTimeDelta);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CPaimon*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END