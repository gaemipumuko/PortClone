#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Client)

class CPart_Status abstract : public CPartObject
{
public:
	typedef struct tagPartStatusDesc
	{
		PARTOBJ_DESC		tPartObjDesc;
	} PARTSTATUS_DESC;

public:
	enum LEVELUP_STEP { STEP_ZERO, STEP_ONE, STEP_TWO, STEP_THREE, STEP_FOUR, STEP_FIVE, STEP_SIX, STEP_END };
	enum ATK_TYPE { ATK_NORMAL, ATK_CHARGED, ATK_ELEMENTAL, ATK_BURST, ATK_END };

public:
	typedef struct tagEntityStatusDesc
	{
		LEVELUP_STEP		eLevelUp_Step = { STEP_END };
		ATK_TYPE			eAtk_Type = { ATK_END };
		
		// ������ â
		/* ����, �ִ�, ������, ���������� */
		_vector				fExp = { };
		_vector				fLevel = { }; /* ���� */ 
		_vector				fHp = { }; /* ü�� */ 
		_vector				fAtk = { }; /* ���ݷ� */
		_vector				fDef = { }; /* ���� */
		_vector				fEm = { }; /* ���� */
		_vector				fCr = { }; /* ġȮ */
		_vector				fCd = { }; /* ġ�� */
		_vector				fEr = { }; /* ���� */
		_vector				fElementalDmg = { }; /* �������غ��ʽ� */
		_vector				fRes = { }; /* ���ҳ��� */
		_vector				fHealing_Bonus = { }; /* ġ�����ʽ� */
		_vector				fReuseTime_reduction = { }; /* ������ð� ����*/
		_vector				fShield_Strength = { }; /* ��ȣ�� ��ȭ */

		// ���ڸ�
		/* ���� */
		_float				fConstellation = { }; /* ĳ���� ���� */

		// ����
		/* ����, ����, ���ҽ�ų ���ð�, �������� ���ð� */
		_vector				fReuseTime = { }; /* ������ð� */
		_float				fCur_Energy = { }; /* ���� ���� ������ */
		_float				fElemental_Energy = { }; /* ���� ������ */
		_bool				bElemental_Skill = { }; /* ���ҽ�ų ��뿩�� */
		_bool				bElemental_Burst = { }; /* �������� ��뿩�� */	

		// ����
		_bool				bLevelUp = { };
		_bool				bLevel_BreakThrough = { };

		_float				fStamina = { };

	}ENTITY_STATUS;

protected:
	CPart_Status(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPart_Status(const CPart_Status& rhs);
	virtual ~CPart_Status() = default;

protected:
	ENTITY_STATUS			m_EntityStatus;

private:
	_bool					m_bSelectMaxExp = { FALSE };

public:
	virtual void Level_BreakThrough() { } // ĳ���͸��� ���Ľ��ȴٸ�

public:
	ENTITY_STATUS	Get_EntityStatus() { return m_EntityStatus; }
	/* �⺻���� */
	_int			Get_Hp() { return static_cast<_int>(m_EntityStatus.fHp.m128_f32[0]); }
	_int			Get_Atk() { return static_cast<_int>(m_EntityStatus.fAtk.m128_f32[0]); }
	_int			Get_Def() { return static_cast<_int>(m_EntityStatus.fDef.m128_f32[0]); }
	_int			Get_Em() { return static_cast<_int>(m_EntityStatus.fEm.m128_f32[0]); }
	_int			Get_Stamina() { return static_cast<_int>(m_EntityStatus.fStamina); }
	
public:
	/* ���� */
	_bool Can_LevelUp();
	void Level_Up(_float fExp);

	/* ���� */
	_float	Get_CurrentHp() { return m_EntityStatus.fHp.m128_f32[0]; }
	_float	Get_MaxHp() { return m_EntityStatus.fHp.m128_f32[1]; }
	_float	Get_Attack(ATK_TYPE eATk);
	void	Set_Damage(_float fDamage);

	/* ���� ��ų */
	void	Set_CurSkillTime(_float fTime) { m_EntityStatus.fReuseTime.m128_f32[0] = fTime; }
	_float	Get_CurSkillTime() { return m_EntityStatus.fReuseTime.m128_f32[0]; }
	_float	Get_ElementalSkillTime() { return m_EntityStatus.fReuseTime.m128_f32[2]; }

	/* ���� ���� */
	void	Set_CurBurstTime(_float fTime) { m_EntityStatus.fReuseTime.m128_f32[1] = fTime; }
	_float	Get_CurBurstTime() { return m_EntityStatus.fReuseTime.m128_f32[1]; }
	_float	Get_ElementalBurstTime() { return m_EntityStatus.fReuseTime.m128_f32[3]; }
	void	Set_CurEnergy(_float fEnergy = 0.f) { m_EntityStatus.fCur_Energy = 0.f; }
	void	Set_MaxEnergy() { m_EntityStatus.fCur_Energy = m_EntityStatus.fElemental_Energy; }
	void	Add_CurEnergy(_float fEnergy) { m_EntityStatus.fCur_Energy += fEnergy; }
	_float	Get_CurEnergy() { return m_EntityStatus.fCur_Energy; }
	_float	Get_ElementalEnergy() { return m_EntityStatus.fElemental_Energy; }

	/* ü�� ȸ�� */
	void	Set_Heal(_float fHeal);
	_float	Set_HpRatioHeal(_float fHpRatio, _float fAddHeal = 0.f);
	void	Set_MaxHP();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override { return S_OK; }

public:
	static CPart_Status*	Create_Status(CREATUREID eID, PARTSTATUS_DESC tStatusDesc);
	virtual void			Free() override;
};

END