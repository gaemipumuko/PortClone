#pragma once

#include "Base.h"
#include "AssetFX.h"
#include "PipeLine.h"

#include "Component_Manager.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)

private:
	CGameInstance();
	virtual ~CGameInstance() = default;

private:
	_uint	m_iLoadingLevel = 0;
	_bool	m_bReserveLevel = false;
	_float4	m_vReservePosition = { };
	class CLevel* m_pReserveLevel = { nullptr };

public:
	_float4	Get_ReservePosition() { return m_vReservePosition; }
	void	Set_ReservePosition(_float4	vReservePosition) { m_vReservePosition = vReservePosition; }
	void	Reset_ReservePosition() { m_vReservePosition = { }; }

private: /* Managers */
	class CGraphic_Device*			m_pGraphic_Device = { nullptr };
	class CTimer_Manager*			m_pTimer_Manager = { nullptr };
	class CCollider_Manager*		m_pCollider_Manager = { nullptr };
	class CEvent_Manager*			m_pEvent_Manager = { nullptr };
	class CInput_Manager*			m_pInput_Manager = { nullptr };
	class CInteraction_Manager*		m_pInteraction_Manager = { nullptr };
	class CJson_Manager*			m_pJson_Manager = { nullptr };
	class CRandom_Manager*			m_pRandom_Manager = { nullptr };
	class CSound_Manager*			m_pSound_Manager = { nullptr };
	class CScreen_Manager*			m_pScreen_Manager = { nullptr };
	class CLevel_Manager*			m_pLevel_Manager = { nullptr };
	class CObject_Manager*			m_pObject_Manager = { nullptr };
	class CComponent_Manager*		m_pComponent_Manager = { nullptr };
	class CFont_Manager*			m_pFont_Manager = { nullptr };
	class CLight_Manager*			m_pLight_Manager = { nullptr };
	class CParticle_Manager*		m_pParticle_Manager = { nullptr };
	class CAssetFX_Manager*			m_pAssetFX_Manager = { nullptr };
	class CStatusEffect_Manager*	m_pStatusEffect_Manager = { nullptr };
	class CRenderTarget_Manager*	m_pRenderTarget_Manager = { nullptr };
	class CPhysX_Manager*			m_pPhysX_Manager = { nullptr };

	class CFrustum*					m_pFrustum = { nullptr };
	class CPipeLine*				m_pPipeLine = { nullptr };
	class CExecuteShader*			m_pExecuteShader = { nullptr };

	HRESULT Clear_Manager();

public:
	HRESULT Reserve_Level(_uint iLoadingLevel, class CLevel* pLevel);

public: /* For.GameInstance */
	HRESULT Initialize_Engine(_uint iNumLevel, _uint iNumCollLayer, _uint iNumStatusLayer, HINSTANCE g_hInstance, const GRAPHIC_DESC& GraphicDesc,
		_Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);
	HRESULT PreviousTick_Engine(_float fTimeDelta);
	HRESULT Tick_Engine(_float fTimeDelta);
	HRESULT LateTick_Engine(_float fTimeDelta);
	static HRESULT Release_Engine();

public: /* For.Graphic_Device*/
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();
	void Set_PlayVideo(_bool bPlayVideo);

public: /* For.Timer_Manager */
	HRESULT	Add_Timer(const wstring& strTimerTag);
	_float	Compute_TimeDelta(const wstring& strTimerTag);

public: /* For. Collider_Manager */
	void				Set_IsCollision(_uint iLayer, _int iObjectID, _bool bCollision);

	HRESULT				Add_Collider_Object(_uint iLayer, OUT _int* pCreateObjectID, class CCollider* pCollider, class CGameObject* pGameObject);
	HRESULT				Release_Collider_Object(_uint iLayer, _int iObjectID);

	class CGameObject* Find_ObjectID(_uint iLayer, _int iObjectID);
	class CGameObject* Find_AllCollision_Object(_uint iLayer, _int iObjectID);
	vector<class CGameObject*>	Find_Collision_Objects(_uint iLayer, _int iObjectID, _uint iFindLayer);
	vector<class CGameObject*>	Find_Collision_Objects(class CCollider* pCollider, _uint iFindLayer);

public: /* For.Event_Manager */
	vector<string>	Get_EventCallerTags();
	vector<string>	Get_EventSounds(const wstring& strModelTag, const string& strAnimationTag);
	vector<string>	Get_EventParticles(const wstring& strModelTag, const string& strAnimationTag);
	vector<string>	Get_EventFunctions(const wstring& strModelTag, const string& strAnimationTag);

	HRESULT	Create_EventCaller(const wstring& strModelTag, CModel* pModelCom);
	HRESULT	Delete_EventCaller(const wstring& strModelTag);

	class CEventCaller*	Clone_EventCaller(const wstring& strModelTag, const string& strAnimationTag);

	HRESULT	Save_EventDataFile(const string& strEventDataFilePath);
	HRESULT	Load_EventDataFile(const string& strEventDataFilePath);

	HRESULT	Append_SoundEvent(const wstring& strModelTag, const string& strAnimationTag, wstring strSoundTag, _float fDuration = 0.f);
	HRESULT	Append_ParticleEvent(const wstring& strModelTag, const string& strAnimationTag,
		_int iShaderPass, _int iParticleIndex, const wstring& strParticleTag, const string& strBoneTag, _float fDuration, _float fEventFrame = 0.f);
	HRESULT	Append_FunctionEvent(const wstring& strModelTag, const string& strAnimationTag, wstring strFunctionTag, _float fDuration = 0.f);

	HRESULT Delete_SoundEvent(const wstring& strModelTag, const string& strAnimationTag, _uint iIndex);
	HRESULT Delete_FunctionEvent(const wstring& strModelTag, const string& strAnimationTag, _uint iIndex);

public: /* For.Input_Manager */
	_bool Key_Pressing(_int iKey);
	_bool Key_Down(_int iKey);
	_bool Key_Up(_int iKey);

	_long Mouse_Move(MOUSEMOVESTATE eMouseMoveID);

public: /* For.Interaction_Manager */
	void		Add_InteractionCount(wstring strType, _int iCount = 1);
	void		Register_Interaction(wstring strType);
	void		Unregister_Interaction(wstring strType);

	_bool		isInteraction(wstring strType);

	_int		Get_InteractionCount(wstring strType);
	void		Set_InteractionCount(wstring strType, _int iCount = 0);


public: /* For.Json_Manager */
	HRESULT	Load_JsonFile(const wstring& strJsonTag, const string& strJsonFilePath);

	_int		Find_JsonIntValue(const wstring& strJsonTag, vector<string> vFindTags);
	_float		Find_JsonFloatValue(const wstring& strJsonTag, vector<string> vFindTags);
	wstring		Find_JsonStringValue(const wstring& strJsonTag, vector<string> vFindTags);
	list<_int>		Find_JsonIntArrayValues(const wstring& strJsonTag, vector<string> vFindTags);
	list<wstring>	Find_JsonStringArrayValues(const wstring& strJsonTag, vector<string> vFindTags);

public: /* For.Random_Manager */
	_int	Get_Int_Random(_int iStart, _int iEnd);
	_float	Get_Float_Random(_float fStart, _float fEnd);
	_vector	Get_RandomPos_InCircle(_vector vCenterPos, _float fRadius, _vector vSelfPos);

public: /* For.Screen_Manager */
	_int	Get_ClipCount();

	void	Fix_Mouse();
	void	FreeFix_Mouse();

	void	Clip_Mouse();
	void	FreeClip_Mouse();

public: /* For.Sound_Manager */
	vector<string>	Get_SoundTags();
	CHANNELID		Get_NextChannel();

	void		PlayBGM(wstring strSoundKey, float fVolume = 1.f);
	void		PlayBGM(wstring strSoundKey, float fVolume, CHANNELID eChannel);

	CHANNELID	PlayReturnSound(wstring strSoundKey, float fVolume = 1.f);
	void		PlaySoundW(wstring strSoundKey, float fVolume = 1.f, CHANNELID eChannel = CHANNELID::MAXCHANNEL);

	void StopAll();
	void StopSound(CHANNELID eID);

	void SetChannelVolume(CHANNELID eID, float fVolume);

public: /* For.Level_Manager */
	HRESULT			Open_Level(_uint iLevel, class CLevel* pLevel);
	HRESULT			Render_Level();
	_uint			Get_CurrentLevel();
	class CLevel*	Get_CurrentLevelObject();

public: /* For.Object_Manager */
	HRESULT	Add_Object_Prototype(const wstring& strPrototypeTag, class CGameObject* pPrototype, _bool bDuplication = FALSE);
	HRESULT	Add_Object_Layer(_uint iLevel, const wstring& strPrototypeTag, const wstring& strLayerTag, void* pArg = nullptr, class CGameObject** ppObject = nullptr);

	class CGameObject* Find_Object(_uint iLevel, const wstring& strLayerTag, _uint iIndex = 0);
	class CGameObject* Find_Object_Prototype(const wstring& strPrototypeTag);

	class CComponent* Find_Layer_Component(_uint iLevel, const wstring& strLayerTag, const wstring& strComponentTag, _uint iIndex = 0);

	HRESULT	Release_Object(_uint iLevel, const wstring& strLayerTag, class CGameObject* pGameObject);
	HRESULT	Release_Object_Layer(_uint iLevel, const wstring& strLayerTag);

	list<class CGameObject*>* Find_Objects(_uint iLevel, const wstring& strLayerTag);
	
public: /* For.Component_Manager */
	HRESULT				Add_Component_Prototype(_uint iLevel, const wstring& strPrototypeTag, class CComponent* pPrototype, _bool bDuplication = FALSE);
	HRESULT				Release_Component_Prototype(_uint iLevel, const wstring& strPrototypeTag);
	class CComponent*	Find_Component_Prototype(_uint iLevel, const wstring& strPrototypeTag);
	class CComponent*	Clone_Component_Prototype(_uint iLevel, const wstring& strPrototypeTag, void* pArg = nullptr);

public: /* For.Font_Manager */
	HRESULT	Add_Font(const wstring& strFontTag, const wstring& strFontFilePath);
	HRESULT	Render_Font(const wstring& strFontTag, const wstring& strText, const _float2& vPosition, _float fScale = 1.f,
		_fvector vColor = Colors::White, _float fRotation = 0.f, const _float2& vOrigin = _float2(0.f, 0.f), _bool bAlign = TRUE);
	
public: /* For.Light_Manager */
	HRESULT				Add_Light(_uint iLevelIndex, const LIGHT_DESC& LightDesc);
	const LIGHT_DESC*	Find_LightDesc(_uint iLevelIndex, _uint iIndex) const;
	HRESULT				Update_LightPosition(_uint iLevelIndex, _uint iIndex, const _fvector& vLightPosition);
	HRESULT				Render_Lights(_uint iLevelIndex, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

public: /* For.Particle_Manager */
	vector<wstring>		Get_ParticleTags();
	PARTICLE_INFO		Get_ParticleInfo(const wstring& strPrototypeTag);

public:
	HRESULT Prepare_ParticleManager(CTexture* pTexture, CRenderer* pRenderer);

	HRESULT	Play_Particle(const wstring& strPrototypeTag, const wstring& strParticleTag, _int iShaderPass, _int iParticleIndex,
		const _float4x4* pParentBone = nullptr, class CTransform* pParentMatrix = nullptr, _float fDuration = -1.f);
	HRESULT	Pause_Particle(const wstring& strParticleTag, _bool bPause);
	HRESULT	Stop_Particle(const wstring& strParticleTag, _float fDelay = -1.f);

public:
	HRESULT	Create_ParticlePrototype(const wstring& strParticleTag, PARTICLE_INFO tValue);
	HRESULT	Delete_ParticlePrototype(const wstring& strParticleTag);

	HRESULT	Save_Particles(const wstring& strFilePath);
	HRESULT	Load_Particles(const wstring& strFilePath);

public:	/* For.AssetFX_Manager */
	HRESULT	Ready_FXPrototype(const wstring& strPrototypeTag, class CAssetFX* pAssetFX);
	HRESULT	Play_AssetFX(const wstring& strPrototypeTag, const wstring& strEffectTag, void* pArgs);
	HRESULT	Stop_AssetFX(const wstring& strEffectTag, _float fDelay = -1.f);

public: /* For.StatusEffect_Manager */
	HRESULT Add_StatusEffect(_uint iLayer, wstring strObjectID, _int iCount = 0, _float fDuration = 0.f, _bool bCountCompareMax = TRUE);
	HRESULT Release_StatusEffect(_uint iLayer, wstring strObjectID);
	HRESULT Add_Count_StatusEffect(_uint iLayer, wstring strObjectID, _int iCount);
	_int	Get_Count_StatusEffect(_uint iLayer, wstring strObjectID);
	_bool	IsProgress_StatusEffect(_uint iLayer, wstring strObjectID);

public: /* For.RenderTarget_Manager */
	HRESULT Create_RenderTarget(const wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float& fValue);
	HRESULT Create_RenderTarget(const wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vColor);
	HRESULT Create_RenderTarget(const wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vColor, _int iArraySize);
	HRESULT Bind_RenderTarget(class CShader* pShader, _char* pConstantName, const wstring& strTargetTag);

	HRESULT Add_MultiRenderTarget(const wstring& strMRTTag, const wstring& strTargetTag);
	HRESULT Begin_MultiRenderTarget(const wstring& strMRTTag, ID3D11DepthStencilView* pDepthStencilView = nullptr, _bool bBackBuffer = FALSE, _bool bClearBuffer = TRUE);
	HRESULT Begin_NonDepthMultiRenderTarget(const wstring& strMRTTag, _bool bBackBuffer = FALSE, _bool bClearBuffer = TRUE);
	HRESULT End_MultiRenderTarget();

	class CRenderTarget*		Find_RenderTarget(const wstring& strTargetTag);
	ID3D11ShaderResourceView*	Find_ShaderResourceView(const wstring& strTargetTag);

#ifdef _DEBUG
	HRESULT Ready_RTDebug(const wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_RTDebug(const wstring& strMTRTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif

public: /* For.PhysX_Manager */
	HRESULT Init_Scene();
	HRESULT PhysX_Simulate(_float fTimeDelta);
	HRESULT Create_StaticMesh(_uint iNumVertices, _float3* pVertices, _uint iIndices, _ulong* pIndices, _float4x4 vWorldMatrix, PxRigidStatic** ppRigidStatic = nullptr);
	PxController* Create_Controller(_float fHeight, _float fRadius, _float fStepOffset, _float4 vPosition, PxFilterData tControllerFilterData);
	_bool RayCast_Position(_float3 vOrigin, _float3 vUnitDir, _float fMaxDistance, _float3* pHitPoint, PxQueryFilterData tQueryFilterData);

public: /* For.Frustum */
	_float3*	Get_WorldFrustum();

	_bool		PointInFrustum_InWorld(_fvector vWorldPos, _float fRange = 0.f);
	_bool		PointInFrustum_InLocal(_fvector vLocalPos, _float fRange = 0.f);
	void		Transform_LocalSpace(_fmatrix WorldMatrixInv);

public: /* For.PipeLine */
	_float4x4	Get_ViewPortMatrix();

	_matrix		Get_Transform(CPipeLine::TRANSFORMSTATE eTransformState) const;
	_float4x4	Get_Transform_Float4x4(CPipeLine::TRANSFORMSTATE eTransformState) const;

	void		Set_Transform(CPipeLine::TRANSFORMSTATE eTransformState, _fmatrix TransformMatrix);
	void		Set_Transform(CPipeLine::TRANSFORMSTATE eTransformState, const _float4x4& TransformMatrix);

	_matrix		Get_Transform_Inverse(CPipeLine::TRANSFORMSTATE eTransformState) const;
	_float4x4	Get_Transform_Inverse_Float4x4(CPipeLine::TRANSFORMSTATE eTransformState) const;

	_float4		Get_CamRight() const;
	_float4		Get_CamLook() const;
	_float4		Get_CamPosition() const;

	
public:	/* For. ExecuteShader */
	void		Set_InvenBlur(_bool bInvenBlur);

	HRESULT		Update_LUTIndex(_uint iIndex);
	HRESULT		Update_BloomCB(_float fBloomScale, _float fBloomThreshold);
	HRESULT		Update_ColorGradeCB(_float3 vColorFilter, _float fSaturation, _float fExposure, _float fContrast);

	HRESULT		Render_Scene(ID3D11ShaderResourceView* pSceneSRV);
	HRESULT		Render_Effect(ID3D11ShaderResourceView* pSceneSRV);

public:
	void			Clear(_uint iLevel);
	virtual void	Free();
};

END