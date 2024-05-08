#include "..\Public\GameInstance.h"

#include "Graphic_Device.h"
#include "Timer_Manager.h"
#include "Collider_Manager.h"
#include "Event_Manager.h"
#include "Input_Manager.h"
#include "Interaction_Manager.h"
#include "Json_Manager.h"
#include "Random_Manager.h"
#include "Screen_Manager.h"
#include "Sound_Manager.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Component_Manager.h"
#include "Font_Manager.h"
#include "Light_Manager.h"
#include "Particle_Manager.h"
#include "AssetFX_Manager.h"
#include "StatusEffect_Manager.h"
#include "RenderTarget_Manager.h"
#include "PhysX_Manager.h"
#include "Frustum.h"
#include "ExecuteShader.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{
}

HRESULT CGameInstance::Clear_Manager()
{
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pEvent_Manager);
	Safe_Release(m_pInput_Manager);
	Safe_Release(m_pInteraction_Manager);
	Safe_Release(m_pJson_Manager);
	Safe_Release(m_pRandom_Manager);
	Safe_Release(m_pScreen_Manager);
	Safe_Release(m_pSound_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pCollider_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pParticle_Manager);
	Safe_Release(m_pAssetFX_Manager);
	Safe_Release(m_pStatusEffect_Manager);
	Safe_Release(m_pRenderTarget_Manager);
	Safe_Release(m_pPhysX_Manager);

	Safe_Release(m_pFrustum);
	Safe_Release(m_pExecuteShader);

	return S_OK;
}

HRESULT CGameInstance::Reserve_Level(_uint iLoadingLevel, CLevel* pLevel)
{
	if (nullptr == pLevel)
		return E_FAIL;

	m_bReserveLevel = true;
	m_iLoadingLevel = iLoadingLevel;
	m_pReserveLevel = pLevel;

	return S_OK;
}

HRESULT CGameInstance::Initialize_Engine(_uint iNumLevel, _uint iNumCollLayer, _uint iNumStatusLayer, HINSTANCE g_hInstance, const GRAPHIC_DESC& GraphicDesc,
	_Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext)
{
	/* Graphic_Device */
	m_pGraphic_Device = CGraphic_Device::Create(GraphicDesc.hWnd, GraphicDesc.isWindowed, GraphicDesc.iWinSizeX, GraphicDesc.iWinSizeY, ppDevice, ppContext);
	if (m_pGraphic_Device == nullptr)
		return E_FAIL;
	
	/* Collider */
	m_pCollider_Manager = CCollider_Manager::Create(iNumCollLayer);
	if (m_pCollider_Manager == nullptr)
		return E_FAIL;

	/* Timer */
	m_pTimer_Manager = CTimer_Manager::Create();
	if (m_pTimer_Manager == nullptr)
		return E_FAIL;

	/* Event */
	m_pEvent_Manager = CEvent_Manager::Create();
	if (m_pEvent_Manager == nullptr)
		return E_FAIL;

	/* Input */
	m_pInput_Manager = CInput_Manager::Create(g_hInstance, GraphicDesc.hWnd);
	if (m_pInput_Manager == nullptr)
		return E_FAIL;

	/* Interaction */
	m_pInteraction_Manager = CInteraction_Manager::Create();
	if (m_pInteraction_Manager == nullptr)
		return E_FAIL;

	/* Json */
	m_pJson_Manager = CJson_Manager::Create();
	if (m_pJson_Manager == nullptr)
		return E_FAIL;

	/* Random */
	m_pRandom_Manager = CRandom_Manager::Create();
	if (m_pRandom_Manager == nullptr)
		return E_FAIL;

	/* Screen */
	m_pScreen_Manager = CScreen_Manager::Create(GraphicDesc.hWnd);
	if (m_pScreen_Manager == nullptr)
		return E_FAIL;

	/* Sound */
	m_pSound_Manager = CSound_Manager::Create();
	if (m_pSound_Manager == nullptr)
		return E_FAIL;

	/* Level */
	m_pLevel_Manager = CLevel_Manager::Create(iNumLevel);
	if (m_pLevel_Manager == nullptr)
		return E_FAIL;

	/* Object */
	m_pObject_Manager = CObject_Manager::Create(iNumLevel);
	if (m_pObject_Manager == nullptr)
		return E_FAIL;

	/* Component */
	m_pComponent_Manager = CComponent_Manager::Create(iNumLevel);
	if (m_pComponent_Manager == nullptr)
		return E_FAIL;

	/* Font */
	m_pFont_Manager = CFont_Manager::Create(*ppDevice, *ppContext);
	if (m_pFont_Manager == nullptr)
		return E_FAIL;

	/* Light */
	m_pLight_Manager = CLight_Manager::Create(iNumLevel);
	if (m_pLight_Manager == nullptr)
		return E_FAIL;

	/* Particle */
	m_pParticle_Manager = CParticle_Manager::Create(*ppDevice, *ppContext);
	if (m_pParticle_Manager == nullptr)
		return E_FAIL;

	/* AssetFX */
	m_pAssetFX_Manager = CAssetFX_Manager::Create(*ppDevice, *ppContext);
	if (m_pAssetFX_Manager == nullptr)
		return E_FAIL;

	/* StatusEffect */
	m_pStatusEffect_Manager = CStatusEffect_Manager::Create(iNumStatusLayer);
	if (m_pAssetFX_Manager == nullptr)
		return E_FAIL;

	/* Frustum */
	m_pFrustum = CFrustum::Create();
	if (m_pFrustum == nullptr)
		return E_FAIL;

	/* PipeLine */
	m_pPipeLine = CPipeLine::Create(0.f, 0.f, static_cast<_float>(GraphicDesc.iWinSizeX), static_cast<_float>(GraphicDesc.iWinSizeY), 0.f, 1.f);
	if (m_pPipeLine == nullptr)
		return E_FAIL;

	/* ComputeShader */
	m_pExecuteShader = CExecuteShader::Create(*ppDevice, *ppContext);
	if (m_pExecuteShader == nullptr)
		return E_FAIL;

	/* RenderTarget_Manager */
	m_pRenderTarget_Manager = CRenderTarget_Manager::Create(*ppDevice, *ppContext);
	if (m_pRenderTarget_Manager == nullptr)
		return E_FAIL;

	/* PhysX_Manager */
	m_pPhysX_Manager = CPhysX_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pPhysX_Manager)
		return E_FAIL;

	return S_OK;
}

HRESULT CGameInstance::PreviousTick_Engine(_float fTimeDelta)
{
	if (m_pInput_Manager == nullptr ||
		m_pScreen_Manager == nullptr ||
		m_pLevel_Manager == nullptr ||
		m_pObject_Manager == nullptr ||
		m_pAssetFX_Manager == nullptr)
		return E_FAIL;

	if (true == m_bReserveLevel) 
	{
		m_bReserveLevel = false;

		if (FAILED(Open_Level(m_pLevel_Manager->Get_NumLevel() - 1, m_pReserveLevel)))
			return E_FAIL;
	}

	if (FAILED(m_pInput_Manager->PreviousTick(fTimeDelta)))
	{
		MSG_BOX(TEXT("Failed to PreviousTick : CInput_Manager"));
		return E_FAIL;
	}

	if (FAILED(m_pScreen_Manager->PreviousTick(fTimeDelta)))
	{
		MSG_BOX(TEXT("Failed to PreviousTick : CScreen_Manager"));
		return E_FAIL;
	}

	if (FAILED(m_pLevel_Manager->PreviousTick(fTimeDelta)))
	{
		MSG_BOX(TEXT("Failed to PreviousTick : CLevel_Manager"));
		return E_FAIL;
	}

	if (FAILED(m_pObject_Manager->PreviousTick(fTimeDelta)))
	{
		MSG_BOX(TEXT("Failed to PreviousTick : CObject_Manager"));
		return E_FAIL;
	}

	if (FAILED(m_pAssetFX_Manager->PreviousTick(fTimeDelta)))
	{
		MSG_BOX(TEXT("Failed to PreviousTick : CAssetFX_Manager"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CGameInstance::Tick_Engine(_float fTimeDelta)
{
	if (m_pLevel_Manager == nullptr ||
		m_pObject_Manager == nullptr ||
		m_pParticle_Manager == nullptr ||
		m_pAssetFX_Manager == nullptr ||
		m_pStatusEffect_Manager == nullptr ||
		m_pFrustum == nullptr ||
		m_pPipeLine == nullptr)
		return E_FAIL;

	if (FAILED(m_pLevel_Manager->Tick(fTimeDelta)))
	{
		MSG_BOX(TEXT("Failed to Tick : CLevel_Manager"));
		return E_FAIL;
	}

	if (FAILED(m_pObject_Manager->Tick(fTimeDelta)))
	{
		MSG_BOX(TEXT("Failed to Tick : CObject_Manager"));
		return E_FAIL;
	}

	if (FAILED(m_pParticle_Manager->Tick(fTimeDelta)))
	{
		MSG_BOX(TEXT("Failed to Tick : CParticle_Manager"));
		return E_FAIL;
	}

	if (FAILED(m_pAssetFX_Manager->Tick(fTimeDelta)))
	{
		MSG_BOX(TEXT("Failed to Tick : CAssetFX_Manager"));
		return E_FAIL;
	}
	
	if (FAILED(m_pStatusEffect_Manager->Tick(fTimeDelta)))
	{
		MSG_BOX(TEXT("Failed to Tick : CStatusEffect_Manager"));
		return E_FAIL;
	}

	if (FAILED(m_pPipeLine->Tick(fTimeDelta)))
	{
		MSG_BOX(TEXT("Failed to Tick : CPipeLine"));
		return E_FAIL;
	}

	if (FAILED(m_pFrustum->Tick(fTimeDelta)))
	{
		MSG_BOX(TEXT("Failed to Tick : CFrustum"));
		return E_FAIL;
	}

	if (FAILED(m_pPhysX_Manager->PhysX_Simulate(fTimeDelta)))
	{
		MSG_BOX(TEXT("Failed to Simulate : CPhysX_Manager"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CGameInstance::LateTick_Engine(_float fTimeDelta)
{
	if (m_pInput_Manager == nullptr ||
		m_pLevel_Manager == nullptr ||
		m_pObject_Manager == nullptr ||
		m_pAssetFX_Manager == nullptr)
		return E_FAIL;

	if (FAILED(m_pLevel_Manager->LateTick(fTimeDelta)))
	{
		MSG_BOX(TEXT("Failed to LateTick : CLevel_Manager"));
		return E_FAIL;
	}

	if (FAILED(m_pObject_Manager->LateTick(fTimeDelta)))
	{
		MSG_BOX(TEXT("Failed to LateTick : CObject_Manager"));
		return E_FAIL;
	}

	if (FAILED(m_pParticle_Manager->LateTick(fTimeDelta)))
	{
		MSG_BOX(TEXT("Failed to LateTick : CParticle_Manager"));
		return E_FAIL;
	}

	if (FAILED(m_pAssetFX_Manager->LateTick(fTimeDelta)))
	{
		MSG_BOX(TEXT("Failed to LateTick : CAssetFX_Manager"));
		return E_FAIL;
	}

	/* 모든 Tick 종료 이후에 실행되어야 함 */
	if (FAILED(m_pInput_Manager->LateTick(fTimeDelta)))
	{
		MSG_BOX(TEXT("Failed to LateTick : CInput_Manager"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CGameInstance::Release_Engine()
{
	CGameInstance::Get_Instance()->Clear_Manager();

	if (CGameInstance::Destroy_Instance())
	{
		MSG_BOX(TEXT("Failed to Destroy : CGameInstance"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CGameInstance::Clear_BackBuffer_View(_float4 vClearColor)
{
	if (m_pGraphic_Device == nullptr)
		return E_FAIL;

	return m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	if (m_pGraphic_Device == nullptr)
		return E_FAIL;

	return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	if (m_pGraphic_Device == nullptr)
		return E_FAIL;

	return m_pGraphic_Device->Present();
}

void CGameInstance::Set_PlayVideo(_bool bPlayVideo)
{
	return m_pGraphic_Device->Set_PlayVideo(bPlayVideo);
}

HRESULT CGameInstance::Add_Timer(const wstring & strTimerTag)
{
	if (m_pTimer_Manager == nullptr)
		return E_FAIL;

	return m_pTimer_Manager->Add_Timer(strTimerTag);
}

_float CGameInstance::Compute_TimeDelta(const wstring & strTimerTag)
{
	if (m_pTimer_Manager == nullptr)
		return 0.f;

	return m_pTimer_Manager->Compute_TimeDelta(strTimerTag);
}

void CGameInstance::Set_IsCollision(_uint iLayer, _int iObjectID, _bool bCollision)
{
	if (m_pCollider_Manager == nullptr)
		return;

	m_pCollider_Manager->Set_IsCollision(iLayer, iObjectID, bCollision);
}

HRESULT CGameInstance::Add_Collider_Object(_uint iLayer, OUT _int* pCreateObjectID, CCollider* pCollider, CGameObject* pGameObject)
{
	if (m_pCollider_Manager == nullptr)
		return E_FAIL;

	return m_pCollider_Manager->Add_Collider_Object(iLayer, pCreateObjectID, pCollider, pGameObject);
}

HRESULT CGameInstance::Release_Collider_Object(_uint iLayer, _int iObjectID)
{
	if (m_pCollider_Manager == nullptr)
		return E_FAIL;

	return m_pCollider_Manager->Release_Collider_Object(iLayer, iObjectID);
}

CGameObject* CGameInstance::Find_ObjectID(_uint iLayer, _int iObjectID)
{
	if (m_pCollider_Manager == nullptr)
		return nullptr;

	return m_pCollider_Manager->Find_ObjectID(iLayer, iObjectID);
}

CGameObject* CGameInstance::Find_AllCollision_Object(_uint iLayer, _int iObjectID)
{
	if (m_pCollider_Manager == nullptr)
		return nullptr;

	return m_pCollider_Manager->Find_AllCollision_Object(iLayer, iObjectID);
}

vector<class CGameObject*> CGameInstance::Find_Collision_Objects(_uint iLayer, _int iObjectID, _uint iFindLayer)
{
	if (m_pCollider_Manager == nullptr)
		return vector<class CGameObject*>();

	return m_pCollider_Manager->Find_Collision_Objects(iLayer, iObjectID, iFindLayer);
}

vector<class CGameObject*> CGameInstance::Find_Collision_Objects(CCollider* pCollider, _uint iFindLayer)
{
	if (m_pCollider_Manager == nullptr)
		return vector<class CGameObject*>();

	return m_pCollider_Manager->Find_Collision_Objects(pCollider, iFindLayer);
}

vector<string> CGameInstance::Get_EventCallerTags()
{
	if (m_pEvent_Manager == nullptr)
		return vector<string> { };

	return m_pEvent_Manager->Get_EventCallerTags();
}

vector<string> CGameInstance::Get_EventSounds(const wstring& strModelTag, const string& strAnimationTag)
{
	if (m_pEvent_Manager == nullptr)
		return vector<string> { };

	return m_pEvent_Manager->Get_EventSounds(strModelTag, strAnimationTag);
}

vector<string> CGameInstance::Get_EventParticles(const wstring& strModelTag, const string& strAnimationTag)
{
	if (m_pEvent_Manager == nullptr)
		return vector<string> { };

	return m_pEvent_Manager->Get_EventParticles(strModelTag, strAnimationTag);
}

vector<string> CGameInstance::Get_EventFunctions(const wstring& strModelTag, const string& strAnimationTag)
{
	if (m_pEvent_Manager == nullptr)
		return vector<string> { };

	return m_pEvent_Manager->Get_EventFunctions(strModelTag, strAnimationTag);
}

HRESULT CGameInstance::Create_EventCaller(const wstring& strModelTag, CModel* pModelCom)
{
	if (m_pEvent_Manager == nullptr)
		return E_FAIL;

	return m_pEvent_Manager->Create_EventCaller(strModelTag, pModelCom);
}

HRESULT CGameInstance::Delete_EventCaller(const wstring& strModelTag)
{
	if (m_pEvent_Manager == nullptr)
		return E_FAIL;

	return m_pEvent_Manager->Delete_EventCaller(strModelTag);
}

CEventCaller* CGameInstance::Clone_EventCaller(const wstring& strModelTag, const string& strAnimationTag)
{
	if (m_pEvent_Manager == nullptr)
		return nullptr;

	return m_pEvent_Manager->Clone_EventCaller(strModelTag, strAnimationTag);
}

HRESULT CGameInstance::Save_EventDataFile(const string& strEventDataFilePath)
{
	if (m_pEvent_Manager == nullptr)
		return E_FAIL;

	return m_pEvent_Manager->Save_EventDataFile(strEventDataFilePath);
}

HRESULT CGameInstance::Load_EventDataFile(const string& strEventDataFilePath)
{
	if (m_pEvent_Manager == nullptr)
		return E_FAIL;

	return m_pEvent_Manager->Load_EventDataFile(strEventDataFilePath);
}

HRESULT CGameInstance::Append_SoundEvent(const wstring& strModelTag, const string& strAnimationTag, wstring strSoundTag, _float fDuration)
{
	if (m_pEvent_Manager == nullptr)
		return E_FAIL;

	return m_pEvent_Manager->Append_SoundEvent(strModelTag, strAnimationTag, strSoundTag, fDuration);
}

HRESULT CGameInstance::Append_ParticleEvent(const wstring& strModelTag, const string& strAnimationTag, _int iShaderPass, _int iParticleIndex, const wstring& strParticleTag, const string& strBoneTag, _float fDuration, _float fEventFrame)
{
	if (m_pEvent_Manager == nullptr)
		return E_FAIL;

	return m_pEvent_Manager->Append_ParticleEvent(strModelTag, strAnimationTag, iShaderPass, iParticleIndex, strParticleTag, strBoneTag, fDuration, fEventFrame);
}

HRESULT CGameInstance::Append_FunctionEvent(const wstring& strModelTag, const string& strAnimationTag, wstring strFunctionTag, _float fDuration)
{
	if (m_pEvent_Manager == nullptr)
		return E_FAIL;

	return m_pEvent_Manager->Append_FunctionEvent(strModelTag, strAnimationTag, strFunctionTag, fDuration);
}

HRESULT CGameInstance::Delete_SoundEvent(const wstring& strModelTag, const string& strAnimationTag, _uint iIndex)
{
	if (m_pEvent_Manager == nullptr)
		return E_FAIL;

	return m_pEvent_Manager->Delete_SoundEvent(strModelTag, strAnimationTag, iIndex);
}

HRESULT CGameInstance::Delete_FunctionEvent(const wstring& strModelTag, const string& strAnimationTag, _uint iIndex)
{
	if (m_pEvent_Manager == nullptr)
		return E_FAIL;

	return m_pEvent_Manager->Delete_FunctionEvent(strModelTag, strAnimationTag, iIndex);
}

_bool CGameInstance::Key_Pressing(_int iKey)
{
	if (m_pInput_Manager == nullptr)
		return FALSE;

	return m_pInput_Manager->Key_Pressing(iKey);
}

_bool CGameInstance::Key_Down(_int iKey)
{
	if (m_pInput_Manager == nullptr)
		return FALSE;

	return m_pInput_Manager->Key_Down(iKey);
}

_bool CGameInstance::Key_Up(_int iKey)
{
	if (m_pInput_Manager == nullptr)
		return FALSE;

	return m_pInput_Manager->Key_Up(iKey);
}

_long CGameInstance::Mouse_Move(MOUSEMOVESTATE eMouseMoveID)
{
	if (m_pInput_Manager == nullptr)
		return 0l;

	return m_pInput_Manager->Mouse_Move(eMouseMoveID);
}

void CGameInstance::Add_InteractionCount(wstring strType, _int iCount)
{
	if (m_pInteraction_Manager == nullptr)
		return;

	m_pInteraction_Manager->Add_InteractionCount(strType, iCount);
}

void CGameInstance::Register_Interaction(wstring strType)
{
	if (m_pInteraction_Manager == nullptr)
		return;

	m_pInteraction_Manager->Register_Interaction(strType);
}

void CGameInstance::Unregister_Interaction(wstring strType)
{
	if (m_pInteraction_Manager == nullptr)
		return;

	m_pInteraction_Manager->Unregister_Interaction(strType);
}

_bool CGameInstance::isInteraction(wstring strType)
{
	if (m_pInteraction_Manager == nullptr)
		return FALSE;

	return m_pInteraction_Manager->isInteraction(strType);
}

_int CGameInstance::Get_InteractionCount(wstring strType)
{
	if (m_pInteraction_Manager == nullptr)
		return 0;

	return m_pInteraction_Manager->Get_InteractionCount(strType);
}

void CGameInstance::Set_InteractionCount(wstring strType, _int iCount)
{
	if (m_pInteraction_Manager == nullptr)
		return;

	return m_pInteraction_Manager->Set_InteractionCount(strType, iCount);
}

HRESULT CGameInstance::Load_JsonFile(const wstring& strJsonTag, const string& strJsonFilePath)
{
	if (m_pJson_Manager == nullptr)
		return E_FAIL;

	return m_pJson_Manager->Load_JsonFile(strJsonTag, strJsonFilePath);
}

_int CGameInstance::Find_JsonIntValue(const wstring& strJsonTag, vector<string> vFindTags)
{
	if (m_pJson_Manager == nullptr)
		return 0;

	return m_pJson_Manager->Find_JsonIntValue(strJsonTag, vFindTags);
}

_float CGameInstance::Find_JsonFloatValue(const wstring& strJsonTag, vector<string> vFindTags)
{
	if (m_pJson_Manager == nullptr)
		return 0.f;

	return m_pJson_Manager->Find_JsonFloatValue(strJsonTag, vFindTags);
}

wstring	CGameInstance::Find_JsonStringValue(const wstring& strJsonTag, vector<string> vFindTags)
{
	if (m_pJson_Manager == nullptr)
		return L"";

	return m_pJson_Manager->Find_JsonStringValue(strJsonTag, vFindTags);
}

list<_int> CGameInstance::Find_JsonIntArrayValues(const wstring& strJsonTag, vector<string> vFindTags)
{
	if (m_pJson_Manager == nullptr)
		return list<_int>();

	return m_pJson_Manager->Find_JsonIntArrayValues(strJsonTag, vFindTags);
}

list<wstring> CGameInstance::Find_JsonStringArrayValues(const wstring& strJsonTag, vector<string> vFindTags)
{
	if (m_pJson_Manager == nullptr)
		return list<wstring>();

	return m_pJson_Manager->Find_JsonStringArrayValues(strJsonTag, vFindTags);
}

_int CGameInstance::Get_Int_Random(_int iStart, _int iEnd)
{
	if (m_pRandom_Manager == nullptr)
		return 0;

	return m_pRandom_Manager->Get_Int_Random(iStart, iEnd);
}

_float CGameInstance::Get_Float_Random(_float fStart, _float fEnd)
{
	if (m_pRandom_Manager == nullptr)
		return 0;

	return m_pRandom_Manager->Get_Float_Random(fStart, fEnd);
}

_vector CGameInstance::Get_RandomPos_InCircle(_vector vCenterPos, _float fRadius, _vector vSelfPos)
{
	if (m_pRandom_Manager == nullptr)
		return XMVectorSet(0.f, 0.f, 0.f, 0.f);

	return m_pRandom_Manager->Get_RandomPos_InCircle(vCenterPos, fRadius, vSelfPos);
}

_int CGameInstance::Get_ClipCount()
{
	if (m_pScreen_Manager == nullptr)
		return 0;

	return m_pScreen_Manager->Get_ClipCount();
}

void CGameInstance::Fix_Mouse()
{
	if (m_pScreen_Manager == nullptr)
		return;

	return m_pScreen_Manager->Fix_Mouse();
}

void CGameInstance::FreeFix_Mouse()
{
	if (m_pScreen_Manager == nullptr)
		return;

	return m_pScreen_Manager->FreeFix_Mouse();
}

void CGameInstance::Clip_Mouse()
{
	if (m_pScreen_Manager == nullptr)
		return;

	return m_pScreen_Manager->Clip_Mouse();
}

void CGameInstance::FreeClip_Mouse()
{
	if (m_pScreen_Manager == nullptr)
		return;

	return m_pScreen_Manager->FreeClip_Mouse();
}

vector<string> CGameInstance::Get_SoundTags()
{
	if (m_pSound_Manager == nullptr)
		return vector<string> { };

	return m_pSound_Manager->Get_SoundTags();
}

CHANNELID CGameInstance::Get_NextChannel()
{
	if (m_pSound_Manager == nullptr)
		return MAXCHANNEL;

	return m_pSound_Manager->Get_NextChannel();
}

void CGameInstance::PlayBGM(wstring strSoundKey, float fVolume)
{
	if (m_pSound_Manager == nullptr)
		return;

	return m_pSound_Manager->PlayBGM(strSoundKey, fVolume);
}

void CGameInstance::PlayBGM(wstring strSoundKey, float fVolume, CHANNELID eChannel)
{
	if (m_pSound_Manager == nullptr)
		return;

	return m_pSound_Manager->PlayBGM(strSoundKey, fVolume, eChannel);
}

CHANNELID CGameInstance::PlayReturnSound(wstring strSoundKey, float fVolume)
{
	if (m_pSound_Manager == nullptr)
		return MAXCHANNEL;

	return m_pSound_Manager->PlaySoundW(strSoundKey, fVolume);
}

void CGameInstance::PlaySoundW(wstring strSoundKey, float fVolume, CHANNELID eChannel)
{
	if (m_pSound_Manager == nullptr)
		return;

	return m_pSound_Manager->PlaySoundW(strSoundKey, fVolume, eChannel);
}

void CGameInstance::StopAll()
{
	if (m_pSound_Manager == nullptr)
		return;

	return m_pSound_Manager->StopAll();
}

void CGameInstance::StopSound(CHANNELID eID)
{
	if (m_pSound_Manager == nullptr)
		return;

	return m_pSound_Manager->StopSound(eID);
}

void CGameInstance::SetChannelVolume(CHANNELID eID, float fVolume)
{
	if (m_pSound_Manager == nullptr)
		return;

	return m_pSound_Manager->SetChannelVolume(eID, fVolume);
}

HRESULT CGameInstance::Open_Level(_uint iLevel, CLevel* pLevel)
{
	if (m_pLevel_Manager == nullptr)
		return E_FAIL;

	return m_pLevel_Manager->Open_Level(iLevel, pLevel);
}

HRESULT CGameInstance::Render_Level()
{
	if (m_pLevel_Manager == nullptr)
		return E_FAIL;

	return m_pLevel_Manager->Render();
}

_uint CGameInstance::Get_CurrentLevel()
{
	if (m_pLevel_Manager == nullptr)
		return E_FAIL;

	return m_pLevel_Manager->Get_CurrentLevel();
}

CLevel* CGameInstance::Get_CurrentLevelObject()
{
	if (m_pLevel_Manager == nullptr)
		return nullptr;

	return m_pLevel_Manager->Get_CurrentLevelObject();
}

HRESULT CGameInstance::Add_Object_Prototype(const wstring& strPrototypeTag, CGameObject* pPrototype, _bool bDuplication)
{
	if (m_pObject_Manager == nullptr)
		return E_FAIL;

	return m_pObject_Manager->Add_Object_Prototype(strPrototypeTag, pPrototype, bDuplication);
}

HRESULT CGameInstance::Add_Object_Layer(_uint iLevel, const wstring& strPrototypeTag, const wstring& strLayerTag, void* pArg, class CGameObject** ppObject)
{
	if (m_pObject_Manager == nullptr)
		return E_FAIL;

	return m_pObject_Manager->Add_Object_Layer(iLevel, strPrototypeTag, strLayerTag, pArg, ppObject);
}

CGameObject* CGameInstance::Find_Object(_uint iLevel, const wstring& strLayerTag, _uint iIndex)
{
	if (m_pObject_Manager == nullptr)
		return nullptr;

	return m_pObject_Manager->Find_Object(iLevel, strLayerTag, iIndex);
}

CGameObject* CGameInstance::Find_Object_Prototype(const wstring& strPrototypeTag)
{
	if (m_pObject_Manager == nullptr)
		return nullptr;

	return m_pObject_Manager->Find_Object_Prototype(strPrototypeTag);
}

CComponent* CGameInstance::Find_Layer_Component(_uint iLevel, const wstring& strLayerTag, const wstring& strComponentTag, _uint iIndex)
{
	if (m_pObject_Manager == nullptr)
		return nullptr;

	return m_pObject_Manager->Find_Layer_Component(iLevel, strLayerTag, strComponentTag, iIndex);
}

HRESULT CGameInstance::Release_Object(_uint iLevel, const wstring& strLayerTag, CGameObject* pGameObject)
{
	if (m_pObject_Manager == nullptr)
		return E_FAIL;

	return m_pObject_Manager->Release_Object(iLevel, strLayerTag, pGameObject);
}

HRESULT CGameInstance::Release_Object_Layer(_uint iLevel, const wstring& strLayerTag)
{
	if (m_pObject_Manager == nullptr)
		return E_FAIL;

	return m_pObject_Manager->Release_Object_Layer(iLevel, strLayerTag);
}

list<class CGameObject*>* CGameInstance::Find_Objects(_uint iLevel, const wstring& strLayerTag)
{
	if (m_pObject_Manager == nullptr)
		return nullptr;

	return m_pObject_Manager->Find_Objects(iLevel, strLayerTag);
}

HRESULT CGameInstance::Add_Component_Prototype(_uint iLevel, const wstring& strPrototypeTag, CComponent* pPrototype, _bool bDuplication)
{
	if (m_pComponent_Manager == nullptr)
		return E_FAIL;

	return m_pComponent_Manager->Add_Component_Prototype(iLevel, strPrototypeTag, pPrototype, bDuplication);
}

HRESULT CGameInstance::Release_Component_Prototype(_uint iLevel, const wstring& strPrototypeTag)
{
	if (m_pComponent_Manager == nullptr)
		return E_FAIL;

	return m_pComponent_Manager->Release_Component_Prototype(iLevel, strPrototypeTag);
}

CComponent* CGameInstance::Find_Component_Prototype(_uint iLevel, const wstring& strPrototypeTag)
{
	if (m_pComponent_Manager == nullptr)
		return nullptr;

	return m_pComponent_Manager->Find_Component_Prototype(iLevel, strPrototypeTag);
}

CComponent* CGameInstance::Clone_Component_Prototype(_uint iLevel, const wstring& strPrototypeTag, void* pArg)
{
	if (m_pComponent_Manager == nullptr)
		return nullptr;

	return m_pComponent_Manager->Clone_Component_Prototype(iLevel, strPrototypeTag, pArg);
}

HRESULT CGameInstance::Add_Font(const wstring& strFontTag, const wstring& strFontFilePath)
{
	if (m_pFont_Manager == nullptr)
		return E_FAIL;

	return m_pFont_Manager->Add_Font(strFontTag, strFontFilePath);
}

HRESULT CGameInstance::Render_Font(const wstring& strFontTag, const wstring& strText, const _float2& vPosition, _float fScale, _fvector vColor, _float fRotation, const _float2& vOrigin, _bool bAlign)
{
	if (m_pFont_Manager == nullptr)
		return E_FAIL;

	return m_pFont_Manager->Render(strFontTag, strText, vPosition, fScale, vColor, fRotation, vOrigin, bAlign);
}

HRESULT CGameInstance::Add_Light(_uint iLevelIndex, const LIGHT_DESC& LightDesc)
{
	if (m_pLight_Manager == nullptr)
		return E_FAIL;

	return m_pLight_Manager->Add_Light(iLevelIndex, LightDesc);
}

const LIGHT_DESC* CGameInstance::Find_LightDesc(_uint iLevelIndex, _uint iIndex) const
{
	if (m_pLight_Manager == nullptr)
		return nullptr;

	return m_pLight_Manager->Find_LightDesc(iLevelIndex, iIndex);
}

HRESULT CGameInstance::Update_LightPosition(_uint iLevelIndex, _uint iIndex, const _fvector& vLightPosition)
{
	if (m_pLight_Manager == nullptr)
		return E_FAIL;

	return m_pLight_Manager->Update_LightPosition(iLevelIndex, iIndex, vLightPosition);
}

HRESULT CGameInstance::Render_Lights(_uint iLevelIndex, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	if (m_pLight_Manager == nullptr)
		return E_FAIL;

	return m_pLight_Manager->Render(iLevelIndex, pShader, pVIBuffer);
}

vector<wstring> CGameInstance::Get_ParticleTags()
{
	if (m_pParticle_Manager == nullptr)
		return vector<wstring>();

	return m_pParticle_Manager->Get_ParticleTags();
}

PARTICLE_INFO CGameInstance::Get_ParticleInfo(const wstring& strPrototypeTag)
{
	if (m_pParticle_Manager == nullptr)
		return PARTICLE_INFO();

	return m_pParticle_Manager->Get_ParticleInfo(strPrototypeTag);
}

HRESULT CGameInstance::Prepare_ParticleManager(CTexture* pTexture, CRenderer* pRenderer)
{
	if (m_pParticle_Manager == nullptr)
		return E_FAIL;

	return m_pParticle_Manager->Prepare_ParticleManager(pTexture, pRenderer);
}

HRESULT CGameInstance::Play_Particle(const wstring& strPrototypeTag, const wstring& strParticleTag, _int iShaderPass, _int iParticleIndex, const _float4x4* pParentBone, class CTransform* pParentMatrix, _float fDuration)
{
	if (m_pParticle_Manager == nullptr)
		return E_FAIL;

	return m_pParticle_Manager->Play_Particle(strPrototypeTag, strParticleTag, iShaderPass, iParticleIndex, pParentBone, pParentMatrix, fDuration);
}

HRESULT CGameInstance::Pause_Particle(const wstring& strParticleTag, _bool bPause)
{
	if (m_pParticle_Manager == nullptr)
		return E_FAIL;

	return m_pParticle_Manager->Pause_Particle(strParticleTag, bPause);
}

HRESULT CGameInstance::Stop_Particle(const wstring& strParticleTag, _float fDelay)
{
	if (m_pParticle_Manager == nullptr)
		return E_FAIL;

	return m_pParticle_Manager->Stop_Particle(strParticleTag, fDelay);
}

HRESULT CGameInstance::Create_ParticlePrototype(const wstring& strParticleTag, PARTICLE_INFO tValue)
{
	if (m_pParticle_Manager == nullptr)
		return E_FAIL;

	return m_pParticle_Manager->Create_ParticlePrototype(strParticleTag, tValue);
}

HRESULT CGameInstance::Delete_ParticlePrototype(const wstring& strParticleTag)
{
	if (m_pParticle_Manager == nullptr)
		return E_FAIL;

	return m_pParticle_Manager->Delete_ParticlePrototype(strParticleTag);
}

HRESULT CGameInstance::Save_Particles(const wstring& strFilePath)
{
	if (m_pParticle_Manager == nullptr)
		return E_FAIL;

	return m_pParticle_Manager->Save_Particles(strFilePath);
}

HRESULT CGameInstance::Load_Particles(const wstring& strFilePath)
{
	if (m_pParticle_Manager == nullptr)
		return E_FAIL;

	return m_pParticle_Manager->Load_Particles(strFilePath);
}

HRESULT CGameInstance::Ready_FXPrototype(const wstring& strPrototypeTag, CAssetFX* pAssetFX)
{
	if (m_pAssetFX_Manager == nullptr)
		return E_FAIL;

	return m_pAssetFX_Manager->Ready_FXPrototype(strPrototypeTag, pAssetFX);
}

HRESULT CGameInstance::Play_AssetFX(const wstring& strPrototypeTag, const wstring& strEffectTag, void* pArgs)
{
	if (m_pAssetFX_Manager == nullptr)
		return E_FAIL;

	return m_pAssetFX_Manager->Play_AssetFX(strPrototypeTag, strEffectTag, pArgs);
}

HRESULT CGameInstance::Stop_AssetFX(const wstring& strEffectTag, _float fDelay)
{
	if (m_pAssetFX_Manager == nullptr)
		return E_FAIL;

	return m_pAssetFX_Manager->Stop_AssetFX(strEffectTag, fDelay);
}

HRESULT CGameInstance::Add_StatusEffect(_uint iLayer, wstring strObjectID, _int iCount, _float fDuration, _bool bCountCompareMax)
{
	if (m_pStatusEffect_Manager == nullptr)
		return E_FAIL;

	return m_pStatusEffect_Manager->Add_StatusEffect(iLayer, strObjectID, iCount, fDuration, bCountCompareMax);
}

HRESULT CGameInstance::Release_StatusEffect(_uint iLayer, wstring strObjectID)
{
	if (m_pStatusEffect_Manager == nullptr)
		return E_FAIL;

	return m_pStatusEffect_Manager->Release_StatusEffect(iLayer, strObjectID);
}

HRESULT CGameInstance::Add_Count_StatusEffect(_uint iLayer, wstring strObjectID, _int iCount)
{
	if (m_pStatusEffect_Manager == nullptr)
		return E_FAIL;

	return m_pStatusEffect_Manager->Add_Count_StatusEffect(iLayer, strObjectID, iCount);
}

_int CGameInstance::Get_Count_StatusEffect(_uint iLayer, wstring strObjectID)
{
	if (m_pStatusEffect_Manager == nullptr)
		return -1;

	return m_pStatusEffect_Manager->Get_Count_StatusEffect(iLayer, strObjectID);
}

_bool CGameInstance::IsProgress_StatusEffect(_uint iLayer, wstring strObjectID)
{
	if (m_pStatusEffect_Manager == nullptr)
		return FALSE;

	return m_pStatusEffect_Manager->IsProgress_StatusEffect(iLayer, strObjectID);
}

HRESULT CGameInstance::Create_RenderTarget(const wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float& fValue)
{
	if (m_pRenderTarget_Manager == nullptr)
		return E_FAIL;

	return m_pRenderTarget_Manager->Create_RenderTarget(strTargetTag, iWidth, iHeight, ePixelFormat, fValue);
}

HRESULT CGameInstance::Create_RenderTarget(const wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vColor)
{
	if (m_pRenderTarget_Manager == nullptr)
		return E_FAIL;

	return m_pRenderTarget_Manager->Create_RenderTarget(strTargetTag, iWidth, iHeight, ePixelFormat, vColor);
}

HRESULT CGameInstance::Create_RenderTarget(const wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vColor, _int iArraySize)
{
	if (m_pRenderTarget_Manager == nullptr)
		return E_FAIL;

	return m_pRenderTarget_Manager->Create_RenderTarget(strTargetTag, iWidth, iHeight, ePixelFormat, vColor, iArraySize);
}

HRESULT CGameInstance::Bind_RenderTarget(CShader* pShader, _char* pConstantName, const wstring& strTargetTag)
{
	if (m_pRenderTarget_Manager == nullptr)
		return E_FAIL;

	return m_pRenderTarget_Manager->Bind_RenderTarget(pShader, pConstantName, strTargetTag);
}

HRESULT CGameInstance::Add_MultiRenderTarget(const wstring& strMRTTag, const wstring& strTargetTag)
{
	if (m_pRenderTarget_Manager == nullptr)
		return E_FAIL;

	return m_pRenderTarget_Manager->Add_MultiRenderTarget(strMRTTag, strTargetTag);
}

HRESULT CGameInstance::Begin_MultiRenderTarget(const wstring& strMRTTag, ID3D11DepthStencilView* pDepthStencilView, _bool bBackBuffer, _bool bClearBuffer)
{
	if (m_pRenderTarget_Manager == nullptr)
		return E_FAIL;

	return m_pRenderTarget_Manager->Begin_MultiRenderTarget(strMRTTag, pDepthStencilView, bBackBuffer, bClearBuffer);
}

HRESULT CGameInstance::Begin_NonDepthMultiRenderTarget(const wstring& strMRTTag, _bool bBackBuffer, _bool bClearBuffer)
{
	if (m_pRenderTarget_Manager == nullptr)
		return E_FAIL;

	return m_pRenderTarget_Manager->Begin_NonDepthMultiRenderTarget(strMRTTag, bBackBuffer, bClearBuffer);
}

HRESULT CGameInstance::End_MultiRenderTarget()
{
	if (m_pRenderTarget_Manager == nullptr)
		return E_FAIL;

	return m_pRenderTarget_Manager->End_MultiRenderTarget();
}

CRenderTarget* CGameInstance::Find_RenderTarget(const wstring& strTargetTag)
{
	if (m_pRenderTarget_Manager == nullptr)
		return nullptr;

	return m_pRenderTarget_Manager->Find_RenderTarget(strTargetTag);
}

ID3D11ShaderResourceView* CGameInstance::Find_ShaderResourceView(const wstring& strTargetTag)
{
	if (m_pRenderTarget_Manager == nullptr)
		return nullptr;

	return m_pRenderTarget_Manager->Find_ShaderResourceView(strTargetTag);
}

#ifdef _DEBUG
HRESULT CGameInstance::Ready_RTDebug(const wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	if (m_pRenderTarget_Manager == nullptr)
		return E_FAIL;

	return m_pRenderTarget_Manager->Ready_Debug(strTargetTag, fX, fY, fSizeX, fSizeY);
}

HRESULT CGameInstance::Render_RTDebug(const wstring& strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	if (m_pRenderTarget_Manager == nullptr)
		return E_FAIL;

	return m_pRenderTarget_Manager->Render_Debug(strMRTTag, pShader, pVIBuffer);
}
#endif

HRESULT CGameInstance::Init_Scene()
{
	if (nullptr == m_pPhysX_Manager)
		return E_FAIL;

	return m_pPhysX_Manager->Init_Scene();
}

HRESULT CGameInstance::PhysX_Simulate(_float fTimeDelta)
{
	if (nullptr == m_pPhysX_Manager)
		return E_FAIL;

	return m_pPhysX_Manager->PhysX_Simulate(fTimeDelta);
}

HRESULT CGameInstance::Create_StaticMesh(_uint iNumVertices, _float3* pVertices, _uint iIndices, _ulong* pIndices, _float4x4 vWorldMatrix, PxRigidStatic** ppRigidStatic)
{
	if (nullptr == m_pPhysX_Manager)
		return E_FAIL;

	return m_pPhysX_Manager->Create_StaticMesh(iNumVertices, pVertices, iIndices, pIndices, vWorldMatrix, ppRigidStatic);
}

PxController* CGameInstance::Create_Controller(_float fHeight, _float fRadius, _float fStepOffset, _float4 vPosition, PxFilterData tControllerFilterData)
{
	if (nullptr == m_pPhysX_Manager)
		return nullptr;

	return m_pPhysX_Manager->Create_Controller(fHeight, fRadius, fStepOffset, vPosition, tControllerFilterData);
}

_bool CGameInstance::RayCast_Position(_float3 vOrigin, _float3 vUnitDir, _float fMaxDistance, _float3* pHitPoint, PxQueryFilterData tQueryFilterData)
{
	if (nullptr == m_pPhysX_Manager)
		return false;

	return m_pPhysX_Manager->RayCast_Position(vOrigin, vUnitDir, fMaxDistance, pHitPoint, tQueryFilterData);
}

_float3* CGameInstance::Get_WorldFrustum()
{
	if (m_pFrustum == nullptr)
		return FALSE;

	return m_pFrustum->Get_WorldFrustum();
}

_bool CGameInstance::PointInFrustum_InWorld(_fvector vWorldPos, _float fRange)
{
	if (m_pFrustum == nullptr)
		return FALSE;

	return m_pFrustum->PointInFrustum_InWorld(vWorldPos, fRange);
}

_bool CGameInstance::PointInFrustum_InLocal(_fvector vLocalPos, _float fRange)
{
	if (m_pFrustum == nullptr)
		return FALSE;

	return m_pFrustum->PointInFrustum_InLocal(vLocalPos, fRange);
}

void CGameInstance::Transform_LocalSpace(_fmatrix WorldMatrixInv)
{
	if (nullptr == m_pFrustum)
		return;

	return m_pFrustum->Transform_LocalSpace(WorldMatrixInv);
}

_float4x4 CGameInstance::Get_ViewPortMatrix()
{
	if (m_pPipeLine == nullptr)
		return _float4x4{ };

	return m_pPipeLine->Get_ViewPortMatrix();
}

_matrix CGameInstance::Get_Transform(CPipeLine::TRANSFORMSTATE eTransformState) const
{
	if (m_pPipeLine == nullptr)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_Transform(eTransformState);
}

_float4x4 CGameInstance::Get_Transform_Float4x4(CPipeLine::TRANSFORMSTATE eTransformState) const
{
	if (m_pPipeLine == nullptr)
		return _float4x4();

	return m_pPipeLine->Get_Transform_Float4x4(eTransformState);
}

void CGameInstance::Set_Transform(CPipeLine::TRANSFORMSTATE eTransformState, _fmatrix TransformMatrix)
{
	if (m_pPipeLine == nullptr)
		return;

	m_pPipeLine->Set_Transform(eTransformState, TransformMatrix);
}

void CGameInstance::Set_Transform(CPipeLine::TRANSFORMSTATE eTransformState, const _float4x4& TransformMatrix)
{
	if (m_pPipeLine == nullptr)
		return;

	m_pPipeLine->Set_Transform(eTransformState, TransformMatrix);
}

_matrix CGameInstance::Get_Transform_Inverse(CPipeLine::TRANSFORMSTATE eTransformState) const
{
	if (m_pPipeLine == nullptr)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_Transform_Inverse(eTransformState);
}

_float4x4 CGameInstance::Get_Transform_Inverse_Float4x4(CPipeLine::TRANSFORMSTATE eTransformState) const
{
	if (m_pPipeLine == nullptr)
		return _float4x4();

	return m_pPipeLine->Get_Transform_Inverse_Float4x4(eTransformState);
}

_float4 CGameInstance::Get_CamRight() const
{
	if (m_pPipeLine == nullptr)
		return _float4(0.f, 0.f, 0.f, 0.f);

	return m_pPipeLine->Get_CamRight();
}

_float4 CGameInstance::Get_CamLook() const
{
	if (m_pPipeLine == nullptr)
		return _float4(0.f, 0.f, 0.f, 0.f);

	return m_pPipeLine->Get_CamLook();
}

_float4 CGameInstance::Get_CamPosition() const
{
	if (m_pPipeLine == nullptr)
		return _float4(0.f, 0.f, 0.f, 0.f);

	return m_pPipeLine->Get_CamPosition();
}

void CGameInstance::Set_InvenBlur(_bool bInvenBlur)
{
	if (m_pExecuteShader == nullptr)
		return;

	return m_pExecuteShader->Set_InvenBlur(bInvenBlur);
}

HRESULT CGameInstance::Update_LUTIndex(_uint iIndex)
{
	if (m_pExecuteShader == nullptr)
		return E_FAIL;

	return m_pExecuteShader->Update_LUTIndex(iIndex);
}

HRESULT CGameInstance::Update_BloomCB(_float fBloomScale, _float fBloomThreshold)
{
	if (m_pExecuteShader == nullptr)
		return E_FAIL;

	return m_pExecuteShader->Update_BloomCB(fBloomScale, fBloomThreshold);
}
HRESULT CGameInstance::Update_ColorGradeCB(_float3 vColorFilter, _float fSaturation, _float fExposure, _float fContrast)
{
	if (m_pExecuteShader == nullptr)
		return E_FAIL;

	return m_pExecuteShader->Update_ColorGradeCB(vColorFilter, fSaturation, fExposure, fContrast);
}

HRESULT CGameInstance::Render_Scene(ID3D11ShaderResourceView* pSceneSRV)
{
	if (m_pExecuteShader == nullptr)
		return E_FAIL;

	return m_pExecuteShader->Render_Scene(pSceneSRV);
}

HRESULT CGameInstance::Render_Effect(ID3D11ShaderResourceView* pSceneSRV)
{
	if (m_pExecuteShader == nullptr)
		return E_FAIL;

	return m_pExecuteShader->Render_Effect(pSceneSRV);
}

void CGameInstance::Clear(_uint iLevel)
{
	if (m_pObject_Manager == nullptr ||
		m_pComponent_Manager == nullptr ||
		m_pLight_Manager == nullptr ||
		m_pParticle_Manager == nullptr)
		return;

	m_pObject_Manager->Clear(iLevel);
	m_pComponent_Manager->Clear(iLevel);
	m_pLight_Manager->Clear(iLevel);
	m_pParticle_Manager->Clear();
}

void CGameInstance::Free()
{
	__super::Free();

	Safe_Release(m_pPipeLine);
	Safe_Release(m_pGraphic_Device);
}
