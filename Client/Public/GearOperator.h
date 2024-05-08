#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

#include "SpawnBox.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CGearOperator final : public CGameObject
{
public:
	enum GEAR { GEAR_OPERATOR, GEAR_RUNE, GEAR_END };
	enum OPERATOR_TYPE { OPR_PLATFORM, OPR_WINDFIELD, OPR_END };
	enum CHALLENGE_TYPE { CHL_SEALEDAUTO, CHL_INTERACTION, CHL_END };

	typedef struct tagGearDesc
	{
		OPERATOR_TYPE	eOperatorType = { OPR_END };
		CHALLENGE_TYPE	eChallengeType = { CHL_END };

		_bool			bSealed = { };
		LEVELID			eGearLevel = { LEVEL_END };

		wstring			strInteraction = { };
		wstring			strSpawnBoxTag = { };
		vector<CSpawnBox::MONSTER_INFO>	SpawnBoxDesc;
		class CDungeonPlatform*	pPlatform = { nullptr };

		_float4			vGearRotation = { };
		_float4			vGearPosition = { };
	} GEAR_DESC;

private:
	CGearOperator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGearOperator(const CGearOperator& rhs);
	virtual ~CGearOperator() = default;

private:
	CModel*			m_pModelCom[GEAR_END] = { nullptr };
	CShader*		m_pShaderCom[GEAR_END] = { nullptr };
	CCollider*		m_pColliderCom = { nullptr };
	CRenderer*		m_pRendererCom = { nullptr };
	CTransform*		m_pTransformCom[GEAR_END] = { nullptr };

private:
	_int					m_iColliderID = { -1 };
	class CInteraction_Box* m_pInteractionBox = { nullptr };
	class CDungeonPlatform*	m_pDungeonPlatform = { nullptr };

private:
	LEVELID			m_eLevel = { LEVEL_END };

	OPERATOR_TYPE	m_eOperatorType = { OPR_END };
	CHALLENGE_TYPE	m_eChallengeType = { CHL_END };

	_float			m_fTimer = { 60.f };

	_float4			m_vFrame = { };
	byte			m_byStep = { 0 };
	_bool			m_bSealed = { FALSE };		// 봉인 여부
	_bool			m_bOperated = { FALSE };	// 동작 여부
	wstring			m_strSpawnBoxTag = { };
	vector<CSpawnBox::MONSTER_INFO>	m_SpawnBoxDesc;

private:
	_bool			isInPlayer();
	HRESULT			Create_ChallengeBoss();

private:
	HRESULT			Add_Component(void* pArg);
	HRESULT			Bind_ShaderResources();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CGearOperator*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END