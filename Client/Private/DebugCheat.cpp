#include "stdafx.h"

#include "DebugCheat.h"

#include "Player.h"
#include "SkillObj.h"
#include "Camera_Main.h"
#include "Ground_Item.h"
#include "Level_Loading.h"
#include "Eff_Impact_Glow.h"
#include "CharacterController.h"
#include "Shougun_Trail_TEST.h"

#include "ChainLightning.h"

#include "GameInstance.h"

CDebugCheat::CDebugCheat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CDebugCheat::CDebugCheat(const CDebugCheat& rhs)
	: CGameObject(rhs)
{
}

HRESULT CDebugCheat::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CDebugCheat::Initialize_Clone(void* pArg)
{
    return S_OK;
}

HRESULT CDebugCheat::PreviousTick(_float fTimeDelta)
{
	if (LEVEL_LOADING == m_pGameInstance->Get_CurrentLevel())
		m_iPlayerIdx = -1;
	else 
	{
		m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
		if (m_pPlayer != nullptr)
			m_iPlayerIdx = static_cast<_int>(static_cast<CPlayer*>(m_pPlayer)->Get_CurrentIndex());
		else
			m_iPlayerIdx = -1;
	}

    return S_OK;
}

HRESULT CDebugCheat::Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	if (m_pGameInstance->Key_Down('I'))
	{
		static _uint iLUTIdx = 0;

		iLUTIdx = (iLUTIdx + 1) % 62;

		m_pGameInstance->Update_LUTIndex(iLUTIdx);

		cout << iLUTIdx << endl;
	}

	/* Free Camera */
	CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
	if (pCamera != nullptr)
	{
		if (m_pGameInstance->Key_Down(VK_F1))
			pCamera->Set_FreeCamera(TRUE);

		if (m_pGameInstance->Key_Down(VK_F2))
			pCamera->Set_FreeCamera(FALSE);

		if (m_pGameInstance->Key_Down(VK_F3))
			cout << m_pGameInstance->Get_CamPosition().x << m_pGameInstance->Get_CamPosition().y << m_pGameInstance->Get_CamPosition().z << endl;

		if (m_pGameInstance->Key_Pressing(VK_F3) && m_pGameInstance->Key_Down('Q'))
		{

		}

		if (m_pGameInstance->Key_Down(VK_INSERT))
			m_pGameInstance->Clip_Mouse();

		if (m_pGameInstance->Key_Down(VK_DELETE))
			m_pGameInstance->FreeClip_Mouse();

		if (m_pGameInstance->Key_Down(VK_F5))
			m_pGameInstance->Fix_Mouse();

		if (m_pGameInstance->Key_Down(VK_F6))
			m_pGameInstance->FreeFix_Mouse();
	}

	/* Player TRANSFORMS */
	if (m_pPlayer != nullptr && m_iPlayerIdx != -1)
	{
		if (m_pGameInstance->Key_Down('J'))
		{
			cout << m_pPlayer->Get_PlayerInfo(m_iPlayerIdx).pTransform->Get_State(CTransform::STATE_POSITION).m128_f32[0] << ", "
				<< m_pPlayer->Get_PlayerInfo(m_iPlayerIdx).pTransform->Get_State(CTransform::STATE_POSITION).m128_f32[1] << ", "
				<< m_pPlayer->Get_PlayerInfo(m_iPlayerIdx).pTransform->Get_State(CTransform::STATE_POSITION).m128_f32[2] << endl;
		}
		else if (m_pGameInstance->Key_Down('5'))
		{
			CImpact_Glow::IMPACT_DESC descc = {};
			descc.eEffectID = EFFECT_IMPACT_SINGLE;
			_vector vSelfPos = m_pPlayer->Get_PlayerInfo(m_iPlayerIdx).pTransform->Get_State(CTransform::STATE_POSITION);
			_vector vSelfLook = m_pPlayer->Get_PlayerInfo(m_iPlayerIdx).pTransform->Get_State(CTransform::STATE_LOOK);
			descc.pParent = (CGameObject*)m_pPlayer;
			vSelfPos.m128_f32[2] += 1.f;
			vSelfPos.m128_f32[1] += 1.f;
			descc.vSpawnPos = vSelfPos;
			descc.vScale = { 1.f, 1.5f, 1.f, 1.f };

			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Impact_Glow", L"Layer_Effect", &descc)))
				return E_FAIL;


		}
		else if (m_pGameInstance->Key_Down('6'))
		{
			//CImpact_Glow::IMPACT_DESC descc = {};
			//descc.eEffectID = EFFECT_IMPACT_RADIAL;
			//_vector vSelfPos = m_pPlayer->Get_PlayerInfo(m_iPlayerIdx).pTransform->Get_State(CTransform::STATE_POSITION);
			//_vector vSelfLook = m_pPlayer->Get_PlayerInfo(m_iPlayerIdx).pTransform->Get_State(CTransform::STATE_LOOK);
			//descc.pParent = (CGameObject*)m_pPlayer;
			//vSelfPos.m128_f32[2] += 1.f;
			//vSelfPos.m128_f32[1] += 1.f;
			//descc.vSpawnPos = vSelfPos;
			//descc.vScale = { 1.f, 1.5f, 1.f, 1.f };

			//if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Impact_Glow", L"Layer_Effect", &descc)))
			//	return E_FAIL;

			CImpact_Glow::IMPACT_DESC descc = {};
			descc.eEffectID = EFFECT_IMPACT_PLANE;
			_vector vSelfPos = static_cast<CTransform*>(m_pPlayer->Find_Component(L"Com_Transform"))->Get_State(CTransform::STATE_POSITION);
			descc.pParent = (CGameObject*)m_pPlayer;
			vSelfPos.m128_f32[2] += 4.f;
			vSelfPos.m128_f32[1] += 3.f;
			vSelfPos.m128_f32[0] += 2.1f;
			descc.vSpawnPos = vSelfPos;
			descc.vScale = { 1.f, 1.6f, 1.f, 1.f };
			descc.fRadius = { 0.1f };
			descc.iNumSpike = { 20 };
			descc.iFanSize = { 180 };
			descc.eELEMENT = ELEMENT_FIRE;
			//descc.iRotation = 1;
			//descc.ePlaneDir = CImpact_Glow::EFF_PLANE_RIGHT;
			m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Impact_Glow", L"Layer_Effect", &descc);
		}
		else if (m_pGameInstance->Key_Pressing('7'))
		{
			CImpact_Glow::IMPACT_DESC descc = {};
			descc.eEffectID = EFFECT_IMPACT_PLANE;
			_vector vSelfPos = static_cast<CTransform*>(m_pPlayer->Find_Component(L"Com_Transform"))->Get_State(CTransform::STATE_POSITION);
			descc.pParent = (CGameObject*)m_pPlayer;
			vSelfPos.m128_f32[2] += 4.f;
			vSelfPos.m128_f32[1] += 3.f;
			vSelfPos.m128_f32[0] += 2.1f;
			descc.vSpawnPos = vSelfPos;
			descc.vScale = { 1.f, 1.6f, 1.f, 1.f };
			descc.fRadius = { 3.f };
			descc.iNumSpike = { 20 };
			descc.iFanSize = { 30 };
			descc.eELEMENT = ELEMENT_FIRE;
			descc.iRotAxisY = 0;
			descc.iRotation = 1;
			descc.ePlaneDir = CImpact_Glow::EFF_PLANE_RIGHT;
			m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Impact_Glow", L"Layer_Effect", &descc);


			vSelfPos.m128_f32[0] -= 2.1f;
			descc.vSpawnPos = vSelfPos;

			descc.ePlaneDir = CImpact_Glow::EFF_PLANE_LEFT;
			m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Impact_Glow", L"Layer_Effect", &descc);

			/*		if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Squence", L"Layer_Effect", &descc)))
						return E_FAIL;*/

			m_iRotY += 10;

		}
		else if (m_pGameInstance->Key_Down('8'))
		{
			//CImpact_Glow::IMPACT_DESC descc = {};
			//descc.eEffectID = EFFECT_IMPACT_PLANE_SEQUENCE;
			//descc.eSequenceDir = CImpact_Glow::DIR_FROM_LEFT;
			//_vector vSelfPos = m_pPlayer->Get_PlayerInfo(m_iPlayerIdx).pTransform->Get_State(CTransform::STATE_POSITION);
			//_vector vSelfLook = m_pPlayer->Get_PlayerInfo(m_iPlayerIdx).pTransform->Get_State(CTransform::STATE_LOOK);
			//descc.pParent = (CGameObject*)m_pPlayer;
			//vSelfPos.m128_f32[2] += 1.f;
			//vSelfPos.m128_f32[1] += 1.f;
			//descc.vSpawnPos = vSelfPos;
			//descc.vScale = { 1.f, 1.5f, 1.f, 1.f };
			//descc.fRadius = { 3.f };

			//if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Impact_Glow", L"Layer_Effect", &descc)))
			//	return E_FAIL;


			//CEffect::EFFECT_DESC descc = {};
			//descc.fLifeTime = 5.f;
			//_vector vSelfPos = m_pPlayer->Get_PlayerInfo(m_iPlayerIdx).pTransform->Get_State(CTransform::STATE_POSITION);
			//_vector vSelfLook = m_pPlayer->Get_PlayerInfo(m_iPlayerIdx).pTransform->Get_State(CTransform::STATE_LOOK);
			///*vSelfPos.m128_f32[2] -= 1.f;*/
			//vSelfPos.m128_f32[2] += 2.f;
			//vSelfPos.m128_f32[1] += 1.5f;
			//descc.vSpawnPos = vSelfPos;
			//descc.vScale = { 1.f, 0.5f, 1.f, 0.f };

			//m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Eff_Effigy_Recharge_Trail", L"Layer_Effect", &descc);

		}
		else if (m_pGameInstance->Key_Down('9'))
		{
			CGround_Item::DROP_ITEM_DESC titemdesc = {};
			
				titemdesc.iItemIndex = 124;
				titemdesc.iItemQuantity = 10000;
				titemdesc.vPos = m_pPlayer->Get_PlayerInfo(m_iPlayerIdx).pTransform->Get_State(CTransform::STATE_POSITION);
		
			if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_STATIC, TEXT("Prototype_GameObject_Ground_Item"), L"Layer_World", &titemdesc)))
				return E_FAIL;
		
				titemdesc.iItemIndex = 145;
				titemdesc.iItemQuantity = 11;
				titemdesc.vPos = m_pPlayer->Get_PlayerInfo(m_iPlayerIdx).pTransform->Get_State(CTransform::STATE_POSITION);

			if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_STATIC, TEXT("Prototype_GameObject_Ground_Item"), L"Layer_World", &titemdesc)))
				return E_FAIL;

			//titemdesc.iItemIndex = 126;
			//titemdesc.iItemQuantity = 1;
			//titemdesc.vPos = m_pPlayer->Get_PlayerInfo(m_iPlayerIdx).pTransform->Get_State(CTransform::STATE_POSITION);
			//
			//if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Ground_Item"), L"Layer_World", &titemdesc)))
			//	return E_FAIL;
			//titemdesc.iItemIndex = 127;
			//titemdesc.iItemQuantity = 1;
			//titemdesc.vPos = m_pPlayer->Get_PlayerInfo(m_iPlayerIdx).pTransform->Get_State(CTransform::STATE_POSITION);

			//if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Ground_Item"), L"Layer_World", &titemdesc)))
			//	return E_FAIL;
			//titemdesc.iItemIndex = 128;
			//titemdesc.iItemQuantity = 1;
			//titemdesc.vPos = m_pPlayer->Get_PlayerInfo(m_iPlayerIdx).pTransform->Get_State(CTransform::STATE_POSITION);

			//if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Ground_Item"), L"Layer_World", &titemdesc)))
			//	return E_FAIL;
			//titemdesc.iItemIndex = 130;
			//titemdesc.iItemQuantity = 1;
			//titemdesc.vPos = m_pPlayer->Get_PlayerInfo(m_iPlayerIdx).pTransform->Get_State(CTransform::STATE_POSITION);

			//if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Ground_Item"), L"Layer_World", &titemdesc)))
			//	return E_FAIL;
		}

		if (m_pGameInstance->Key_Pressing('C'))
		{
			/*CAssetFX::ASFX_DESC tAsfxDesc = { };
			tAsfxDesc.ePosType = CAssetFX::ASFX_DESC::POS_FIX;
			tAsfxDesc.vInitPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + _vector{ 10.f, 5.f, 10.f, 0.f };
			tAsfxDesc.pTransform = m_pTransformCom;
			tAsfxDesc.fDuration = 5.f;

			m_pGameInstance->Play_AssetFX(L"EffigyFireProjectile", Get_ParticleTag(L"EffigyFireProjectile"), &tAsfxDesc);*/

			/*CAssetFX::ASFX_DESC tAsfxDesc = { };
			tAsfxDesc.ePosType = CAssetFX::ASFX_DESC::POS_RELATIVE;
			tAsfxDesc.vInitPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			tAsfxDesc.fDuration = 10.f;
			tAsfxDesc.pTransform = PlayerInfo[m_ePlayerNum].pTransform;
			m_pGameInstance->Play_AssetFX(L"EffigyFireMagicCircle", Get_ParticleTag(L"EffigyFireMagicCircle"), &tAsfxDesc);*/

			//CSlash_Trail::SLASH_DESC tDesc = {};
			//tDesc.eTrail_Type = CSlash_Trail::TRAIL_RANDOM;
			//tDesc.pParentsTransform = PlayerInfo[m_ePlayerNum].pTransform;
			////tDesc.eElementColor = (ELEMENT)m_pGameInstance->Get_Int_Random(0, ELEMENT_END);
			//tDesc.eElementColor = ELEMENT_WIND;
			//tDesc.iExceptAxis = CSlash_Trail::AXIS_X + CSlash_Trail::AXIS_Z;
			//tDesc.fTrailSpeed = m_pGameInstance->Get_Float_Random(1.f, 3.f);
			//tDesc.fLifeTime = 999.f;
			//tDesc.fHeight = m_pGameInstance->Get_Float_Random(0.f, 6.f);
			//_float fRandomScale = m_pGameInstance->Get_Float_Random(0.5f, 1.5f);
			//tDesc.vScale = XMVectorSet(fRandomScale, fRandomScale, fRandomScale, 0.f);
			//if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Slash_Trail", L"Layer_Effect", &tDesc)))
			//	return E_FAIL;

			CSkillObj::SKILLOBJ_DESC desc = {};
			desc.vSpawnPos = static_cast<CTransform*>(m_pPlayer->Find_Component(L"Com_Transform"))->Get_State(CTransform::STATE_POSITION);
			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Skill_BigExplosion", L"Layer_Skill", &desc)))
				return E_FAIL;
		}

	//	if (m_pGameInstance->Key_Down('L'))
	//	{
	//		_vector vSelfPos = m_pPlayer->Get_PlayerInfo(m_iPlayerIdx).pTransform->Get_State(CTransform::STATE_POSITION);
	//		_vector vSelfLook = m_pPlayer->Get_PlayerInfo(m_iPlayerIdx).pTransform->Get_State(CTransform::STATE_LOOK);
	//		_vector vSelfRight = m_pPlayer->Get_PlayerInfo(m_iPlayerIdx).pTransform->Get_State(CTransform::STATE_RIGHT);
	//		/*vSelfPos.m128_f32[2] -= 1.f;*/
	//		_vector vDir = XMVector3Normalize(vSelfLook);
	//		_vector vRight = XMVector3Normalize(vSelfRight);
	//		vSelfPos.m128_f32[1] += 1.5f;
	//		vSelfPos += vDir * 1.f +vRight * 0.f;

	//		CShougun_Trail_TEST::SHOGUN_TRAIL_DESC Desc; 
	//		Desc.iTrailIndex = 0;
	//		Desc.vTrailPos = vSelfPos;
	//		if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Shougun_Trail", L"Layer_Effect", &Desc)))
	//			return E_FAIL;

	///*		if (m_iTrail < 9)
	//			m_iTrail++;
	//		else
	//			m_iTrail = 0;*/
	//		vSelfPos += vDir * 1.f + vRight * 0.f;
	//		Desc.vTrailPos = vSelfPos;
	//		Desc.iTrailIndex = 1;
	//		if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Shougun_Trail", L"Layer_Effect", &Desc)))
	//			return E_FAIL;
	//		vSelfPos += vDir * 1.f + vRight * 0.f;
	//		Desc.vTrailPos = vSelfPos;
	//		Desc.iTrailIndex = 2;
	//		if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Shougun_Trail", L"Layer_Effect", &Desc)))
	//			return E_FAIL;
	//		vSelfPos += vDir * ( - 1.f) + vRight * 1.f;
	//		Desc.vTrailPos = vSelfPos;
	//		Desc.iTrailIndex = 3;
	//		if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Shougun_Trail", L"Layer_Effect", &Desc)))
	//			return E_FAIL;
	//		vSelfPos += vDir * (-1.f) + vRight * 0.f;
	//		vSelfPos.m128_f32[1] += 1.f;
	//		Desc.vTrailPos = vSelfPos;
	//		Desc.iTrailIndex = 4;
	//		if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Shougun_Trail", L"Layer_Effect", &Desc)))
	//			return E_FAIL;
	//		vSelfPos += vDir * (-1.f) + vRight * 0.f;
	//		Desc.vTrailPos = vSelfPos;
	//		Desc.iTrailIndex = 5;
	//		if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Shougun_Trail", L"Layer_Effect", &Desc)))
	//			return E_FAIL;
	//		vSelfPos += vDir * 1.f + vRight * ( - 2.f);
	//		Desc.vTrailPos = vSelfPos;
	//		Desc.iTrailIndex = 6;
	//		if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Shougun_Trail", L"Layer_Effect", &Desc)))
	//			return E_FAIL;
	//		vSelfPos += vDir * 1.f + vRight *0.f;
	//		vSelfPos.m128_f32[1] += 1.f;
	//		Desc.vTrailPos = vSelfPos;
	//		Desc.iTrailIndex = 7;
	//		if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Shougun_Trail", L"Layer_Effect", &Desc)))
	//			return E_FAIL;
	//		vSelfPos += vDir * 1.f + vRight * 0.f;
	//		Desc.vTrailPos = vSelfPos;
	//		Desc.iTrailIndex = 8;
	//		if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Shougun_Trail", L"Layer_Effect", &Desc)))
	//			return E_FAIL;
	//		vSelfPos += vDir * 1.f + vRight * 0.f;
	//		Desc.vTrailPos = vSelfPos;
	//		Desc.iTrailIndex = 9;
	//		if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Shougun_Trail", L"Layer_Effect", &Desc)))
	//			return E_FAIL;

	//	}

		else if (m_pGameInstance->Key_Down('V'))
		{
			// CAssetFX::ASFX_DESC tAsfxDesc = { };
			// tAsfxDesc.ePosType = CAssetFX::ASFX_DESC::POS_FIX;
			// tAsfxDesc.vInitPosition = m_pPlayer->Get_PlayerInfo(m_iPlayerIdx).pTransform->Get_State(CTransform::STATE_POSITION);
			// tAsfxDesc.pTransform = m_pPlayer->Get_PlayerInfo(m_iPlayerIdx).pTransform;
			// tAsfxDesc.fDuration = 5.f;
			// 
			// m_pGameInstance->Play_AssetFX(L"EffigyFireTrsBomb", Get_ParticleTag(L"EffigyFireTrsBomb"), &tAsfxDesc);

			//CEffect::EFFECT_DESC descc = {};
			//descc.eEffectID = EFFECT_ETC;
			//descc.fLifeTime = 1.f;
			//_vector vSelfPos = PlayerInfo[m_ePlayerNum].pTransform->Get_State(CTransform::STATE_POSITION);
			//_vector vSelfLook = PlayerInfo[m_ePlayerNum].pTransform->Get_State(CTransform::STATE_LOOK);
			///*vSelfPos.m128_f32[2] -= 1.f;*/
			////vSelfPos += vSelfLook * 2.f;
			//descc.vDir = vSelfLook;
			//descc.vSpawnPos = vSelfPos;
			//descc.vScale = { 1.f, 1.f, 1.f, 0.f };

			//if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Wind_Field", L"Layer_Effect", &descc)))
			//	return E_FAIL;

			//if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Flame_Enchant_Cross", L"Layer_Effect", &vSelfPos)))
			//	return E_FAIL;

			// CImpact_Glow::IMPACT_DESC descc = {};
			// descc.eEffectID = EFFECT_IMPACT_PLANE_CIRCULAR;
			// _vector vSelfPos = m_pPlayer->Get_PlayerInfo(m_iPlayerIdx).pTransform->Get_State(CTransform::STATE_POSITION);
			// _vector vSelfLook = m_pPlayer->Get_PlayerInfo(m_iPlayerIdx).pTransform->Get_State(CTransform::STATE_LOOK);
			// _float4 vPos = _float4(953.0f, 0.5f, 187.0f, 1.0f);
			// vSelfPos = XMLoadFloat4(&vPos);
			// descc.pParent = (CGameObject*)m_pPlayer;
			// descc.vSpawnPos = vSelfPos;
			// descc.vScale = { 2.f, 3.2f, 1.f, 1.f };
			// descc.fRadius = { m_fSize };
			// descc.iFanSize = 75;
			// //descc.iRotation = -75;
			// 
			// m_fSize += 0.1f;
			/*		if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Squence", L"Layer_Effect", &descc)))
						return E_FAIL;*/

						//vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
						//vSelfPos.m128_f32[0] += 2.f * cos(XMConvertToRadians(-12.f * m_iNumPressed));
						//vSelfPos.m128_f32[1] += 2.f * sin(XMConvertToRadians(-12.f * m_iNumPressed));
						//descc.eEffectID = EFFECT_FLAME2;
						//descc.vSpawnPos = vSelfPos;


					//if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_TEST_CLASS", L"Layer_Effect", &descc)))
					//	return E_FAIL;

			//if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Impact_Glow", L"Layer_PostEffect", &descc)))
			//	return E_FAIL;

		}
		else if (m_pGameInstance->Key_Down('B'))
		{
			CEffect::EFFECT_DESC descc = {};
			descc.eEffectID = EFFECT_HITTING_STAR;
			descc.fLifeTime = 1.f;
			_vector vSelfPos = m_pPlayer->Get_PlayerInfo(m_iPlayerIdx).pTransform->Get_State(CTransform::STATE_POSITION);
			_vector vSelfLook = m_pPlayer->Get_PlayerInfo(m_iPlayerIdx).pTransform->Get_State(CTransform::STATE_LOOK);
			/*vSelfPos.m128_f32[2] -= 1.f;*/
			vSelfPos.m128_f32[2] += 2.f;
			vSelfPos.m128_f32[1] += 1.5f;
			descc.vSpawnPos = vSelfPos;
			descc.vScale = { 1.f, 1.f, 1.f, 0.f };


			if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Eff_Activate", L"Layer_Effect", &descc)))
				return E_FAIL;
			//if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Eff_Transition", L"Layer_Effect", &descc)))
			//	return E_FAIL;

			//if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Liquid_Splash", L"Layer_Effect", &descc)))
			//	return E_FAIL;

		}
		//else if (m_pGameInstance->Key_Down('N'))
		//{
		//	CEffect::EFFECT_DESC descc = {};
		//	descc.eEffectID = EFFECT_IMPACT_WATER_SPLASH;
		//	descc.fLifeTime = 1.f;
		//	_vector vSelfPos = m_pPlayer->Get_PlayerInfo(m_iPlayerIdx).pTransform->Get_State(CTransform::STATE_POSITION);
		//	_vector vSelfLook = m_pPlayer->Get_PlayerInfo(m_iPlayerIdx).pTransform->Get_State(CTransform::STATE_LOOK);
		//	/*vSelfPos.m128_f32[2] -= 1.f;*/
		//	vSelfPos.m128_f32[2] += 2.f;
		//	vSelfPos.m128_f32[1] += 1.5f;
		//	descc.vSpawnPos = vSelfPos;
		//	descc.vScale = { 1.f, 1.f, 1.f, 0.f };


		//	//if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Eff_Ground_Stomp", L"Layer_Effect", &descc)))
		//	//	return E_FAIL;

		//	//if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Eff_Transition", L"Layer_Effect", &descc)))
		//	//	return E_FAIL;

			//if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Liquid_Splash", L"Layer_Effect", &descc)))
			//	return E_FAIL;
		//}
		else if (m_pGameInstance->Key_Down('P'))
		{
			CEffect::EFFECT_DESC descc = {};
			descc.eEffectID = EFFECT_HITTING_STAR;
			descc.fLifeTime = 1.f;
			_vector vSelfPos = m_pPlayer->Get_PlayerInfo(m_iPlayerIdx).pTransform->Get_State(CTransform::STATE_POSITION);
			_vector vSelfLook = m_pPlayer->Get_PlayerInfo(m_iPlayerIdx).pTransform->Get_State(CTransform::STATE_LOOK);
			/*vSelfPos.m128_f32[2] -= 1.f;*/
			vSelfPos.m128_f32[1] += 0.95f;
			vSelfPos += vSelfLook * 2.f;

			descc.vSpawnPos = vSelfPos;
			descc.vScale = { 1.f, 1.f, 1.f, 0.f };

			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Eff_Smoke", L"Layer_Effect", &descc)))
				return E_FAIL;
		}
		else if (m_pGameInstance->Key_Down('O'))
		{
			//CEffect::EFFECT_DESC descc = {};
			//_vector vSelfPos = m_pPlayer->Get_PlayerInfo(m_iPlayerIdx).pTransform->Get_State(CTransform::STATE_POSITION);
			//_vector vSelfLook = m_pPlayer->Get_PlayerInfo(m_iPlayerIdx).pTransform->Get_State(CTransform::STATE_LOOK);
			///*vSelfPos.m128_f32[2] -= 1.f;*/
			//vSelfPos.m128_f32[1] += 0.95f;
			//vSelfPos += vSelfLook * 2.f;
			//descc.vSpawnPos = vSelfPos;


			//if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Wind_Field", L"Layer_Effect", &descc)))
			//	return E_FAIL;

			/*CUpdraft::UPDRAFT_DESC tDesc = {};
			XMStoreFloat4(&tDesc.vPosition, m_pPlayer->Get_PlayerInfo(m_iPlayerIdx).pTransform->Get_State(CTransform::STATE_POSITION));
			tDesc.vColliderSize = _float3(2.f, 6.f, 2.f);
			tDesc.fLifeTime = 5.f;

			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Updraft", L"Layer_Updraft", &tDesc)))
				return E_FAIL;*/

#pragma region 전기 이펙트
			/*CSkillObj::SKILLOBJ_DESC tLightningDesc = {};
			tLightningDesc.vScale = XMVectorSet(0.5f, 0.5f, 0.5f, 0.f);
			tLightningDesc.vSpawnPos = m_pPlayer->Get_Position();
			tLightningDesc.vSpawnPos.m128_f32[1] += 1.5f;
			tLightningDesc.fLifeTime = 1.f;

			tLightningDesc.vDir = XMVectorSet(1.f, 0.f, 1.f, 0.f);
			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Lightning", L"Layer_Effect", &tLightningDesc)))
				return E_FAIL;

			tLightningDesc.vDir = XMVectorSet(1.f, 0.f, -1.f, 0.f);
			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Lightning", L"Layer_Effect", &tLightningDesc)))
				return E_FAIL;

			tLightningDesc.vDir = XMVectorSet(-1.f, 0.f, -1.f, 0.f);
			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Lightning", L"Layer_Effect", &tLightningDesc)))
				return E_FAIL;

			tLightningDesc.vDir = XMVectorSet(-1.f, 0.f, 1.f, 0.f);
			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Lightning", L"Layer_Effect", &tLightningDesc)))
				return E_FAIL;

			tLightningDesc.vDir = XMVectorSet(1.f, 0.f, 0.f, 0.f);
			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Lightning", L"Layer_Effect", &tLightningDesc)))
				return E_FAIL;

			tLightningDesc.vDir = XMVectorSet(-1.f, 0.f, 0.f, 0.f);
			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Lightning", L"Layer_Effect", &tLightningDesc)))
				return E_FAIL;

			tLightningDesc.vDir = XMVectorSet(0.f, 0.f, 1.f, 0.f);
			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Lightning", L"Layer_Effect", &tLightningDesc)))
				return E_FAIL;

			tLightningDesc.vDir = XMVectorSet(0.f, 0.f, -1.f, 0.f);
			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Lightning", L"Layer_Effect", &tLightningDesc)))
				return E_FAIL;*/
#pragma endregion

#pragma region 체인 라이트닝
				CChainLightning::CHAIN_LIGHTNING_DESC tDesc = {};
				tDesc.vPointA = m_pPlayer->Get_Position();
				tDesc.vPointA.m128_f32[1] += 0.5f;
				tDesc.vPointB = tDesc.vPointA;
				tDesc.vPointB.m128_f32[0] += 2.f;
				tDesc.vPointB.m128_f32[1] += 2.f;
				tDesc.vPointB.m128_f32[2] += 2.f;

				if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Eff_ChainLightning", L"Layer_Effect", &tDesc)))
					return E_FAIL;
#pragma endregion

		}
		else if(m_pGameInstance->Key_Down('T')) 
		{
			if (m_pGameInstance->Key_Pressing(VK_CONTROL) == TRUE) /* 좌표 텔레포트 */
			{
				_float4 vPos = {0.f, 0.f, 0.f, 1.f};
				cout << "=============================================" << endl;
				cout << "1. 안드리우스      :  953.0,  3.0f,  187.0" << endl;
				cout << "2. 성당 앞         :  171.0,  60.2,  257.2" << endl;
				cout << "3. 드발린 시작위치 : -101.0,   0.f,    1.0" << endl;
				cout << "4. 사원 시작위치   : -550.0, -43.0,  -38.0" << endl;
				cout << "5. 요새 시작위치   :  -63.0,   9.2,  -39.0" << endl;
				cout << "6. 퀘스트 시작위치 :    4.0,  -0.4,  -46.0" << endl;
				cout << "7. 직접 입력 " << endl;
				cout << "=============================================" << endl;

				_int iSelectNum = 0;
				cout << "Input : ";
				cin >> iSelectNum;

				if (iSelectNum != 0)
				{
					if (iSelectNum == 7)
					{
						cout << "Teleportation Position : ";
						cin >> vPos.x;
						cin >> vPos.y;
						cin >> vPos.z;
						vPos.w = 1.f;
					}
					else
					{
						switch (iSelectNum)
						{
						case 1: /* 안드리우스 */
							vPos = _float4(953.0f, 3.0f, 187.0f, 1.0f);
							break;
						case 2: /* 성당 앞 */
							vPos = _float4(171.3f, 60.2f, 257.2f, 1.f);
							break;
						case 3: /* 드발린 시작 위치 */
							vPos = _float4(-101.f, 0.f, 1.f, 1.0f);
							break;
						case 4: /* 사원 시작 위치 */
							vPos = _float4(-550.0f, -43.0f, -38.0, 1.0f);
							break;
						case 5: /* 요새 시작 위치 */
							vPos = _float4(-63.0f, 9.0f, -39.0, 1.0f);
							break;
						case 6: /* 퀘스트 시작 위치 */
							vPos = _float4(4.0f, -0.4f, -46.0, 1.0f);
							break;
						default:
							break;
						}
					}
					m_pPlayer->Get_PlayerInfo(m_iPlayerIdx).pController->Set_Position(vPos);
				}

			}
			else /* 현재 위치 출력 */
			{
				_vector vPos = m_pPlayer->Get_Position();
				cout << "Current Position : " << vPos.m128_f32[0] << ", " << vPos.m128_f32[1] << ", " << vPos.m128_f32[2] << endl;

				vPos.m128_f32[1] = 0.f;
				_vector vMondCenter = XMVectorSet(234, 0.f, 256, 1.f);
				_float	fLength = XMVectorGetX(XMVector3Length(vMondCenter - vPos));

				cout << fLength << endl;
			}
		}

		if (m_pGameInstance->Key_Down(VK_END))
			m_pPlayer->Get_PlayerInfo(m_iPlayerIdx).pController->Set_Position(_float4(234.0f, 68.0f, 256.0f, 1.0f));
		if (m_pGameInstance->Key_Down(VK_END) && m_pGameInstance->Key_Pressing(VK_LSHIFT))
			m_pPlayer->Get_PlayerInfo(m_iPlayerIdx).pController->Set_Position(_float4(953.0f, 3.0f, 187.0f, 1.0f));
		if (m_pGameInstance->Key_Pressing(VK_LSHIFT) && m_pGameInstance->Key_Down(VK_UP))
			m_pPlayer->Get_PlayerInfo(m_iPlayerIdx).pController->Set_Position(_float4(-101.f, 0.f, 1.f, 1.0f));
	}
	/* Effect */
	if (m_pGameInstance->Key_Down('0') && m_pGameInstance->Get_CurrentLevel() == LEVEL_WORLD)
	{
		OBJECT_DESC	ObjDesc = {};
		ObjDesc.vInitPos = XMVectorSet(953.0f, 0.f, 187.0f, 1.0f);
		ObjDesc.iObjectType = MONSTER_ANDRIUS;
		if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, TEXT("Prototype_GameObject_Andrius"), L"Layer_Boss", &ObjDesc)))
			return E_FAIL;
	}
	else if (m_pGameInstance->Key_Pressing(VK_LSHIFT) && m_pGameInstance->Key_Down('8') && m_pGameInstance->Get_CurrentLevel() == LEVEL_WORLD)
	{
		OBJECT_DESC		ObjDesc = {};
		ObjDesc.vInitPos = XMVectorSet(978.0f, 0.f, 192.0f, 1.0f);
		ObjDesc.iObjectType = MONSTER_BRUTE_AXE;
		if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, TEXT("Prototype_GameObject_Brute"), L"Layer_Monster", &ObjDesc)))
			return E_FAIL;
	}
	else if (m_pGameInstance->Key_Pressing(VK_LSHIFT) && m_pGameInstance->Key_Down('7') && m_pGameInstance->Get_CurrentLevel() == LEVEL_WORLD)
	{
		OBJECT_DESC		ObjDesc = {};
		ObjDesc.vInitPos = XMVectorSet(978.0f, 0.f, 192.0f, 1.0f);
		ObjDesc.iObjectType = MONSTER_ABYSS_MAGE_ELEC;
		//ObjDesc.iObjectType = MONSTER_ABYSS_MAGE_ICE;
		//ObjDesc.iObjectType = MONSTER_ABYSS_MAGE_WATER;
		//ObjDesc.iObjectType = MONSTER_ABYSS_MAGE_FIRE;
		if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Abyss_Mage"), L"Layer_Abyss_Mage", &ObjDesc)))
			return E_FAIL;
	}
#endif

	/* Level */
	if (m_pGameInstance->Key_Pressing(VK_SHIFT))
	{
		if (m_pGameInstance->Key_Down('V'))
		{
			CLevel* pLevel = CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_WORLD);

			if (nullptr == pLevel)
				return E_FAIL;

			m_pGameInstance->Reserve_Level(LEVEL_LOADING, pLevel);
		}

		if (m_pGameInstance->Key_Down('B'))
		{
			CLevel* pLevel = CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_TEMPLE_LION);

			if (nullptr == pLevel)
				return E_FAIL;

			m_pGameInstance->Reserve_Level(LEVEL_LOADING, pLevel);
		}

		if (m_pGameInstance->Key_Down('N'))
		{
			CLevel* pLevel = CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_STRONGHOLD_ABYSSMAGE);

			if (nullptr == pLevel)
				return E_FAIL;

			m_pGameInstance->Reserve_Level(LEVEL_LOADING, pLevel);
		}

		if (m_pGameInstance->Key_Down('M'))
		{
			CLevel* pLevel = CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_STORMTERROR);

			if (nullptr == pLevel)
				return E_FAIL;

			m_pGameInstance->Reserve_Level(LEVEL_LOADING, pLevel);
		}
	}

    return S_OK;
}

HRESULT CDebugCheat::LateTick(_float fTimeDelta)
{
    return S_OK;
}

HRESULT CDebugCheat::Render()
{
    return S_OK;
}

CDebugCheat* CDebugCheat::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDebugCheat* pInstance = new CDebugCheat(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CDebugCheat"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDebugCheat::Clone(void* pArg)
{
	CDebugCheat* pInstance = new CDebugCheat(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CDebugCheat"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDebugCheat::Free()
{
    __super::Free();
}
