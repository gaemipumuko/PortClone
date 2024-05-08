#pragma once

#include "Animation_Defines.h"
#include "Level.h"

BEGIN(AnimationTool)

class CLevel_Editor final : public CLevel
{
private:
	CLevel_Editor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Editor() = default;

private:
	class CCamera_Main*	m_pCamera = { nullptr };
	class CMannequin*	m_pMannequin = { nullptr };

private:
	HRESULT	Ready_Level();

	HRESULT	Ready_Lights();
	HRESULT	Ready_ParticleManager();
	HRESULT	Ready_Layer_Camera(const wstring& strLayerTag);
	HRESULT	Ready_Layer_Mannequin(const wstring& strLayerTag);

private: /* For. Material */
	_float					m_fCurrAnimDuration = { };
	_float					m_fMaxAnimDuration = { };

	char					m_szMaterialModelPath[MAX_PATH] = { "../../Reference/Resources/Model/Body/*.*" };
	_int					m_iSelectMaterialModelIndex = { 0 };
	map<string, wstring>	m_MaterialModelTags;

	HRESULT		Clear_MaterialModel();
	HRESULT		Load_MaterialModel(const wstring& strModelFolderPath, const wstring& strFolderTag = L"");

private: /* For. Data */
	char					m_szEventDataFilePath[MAX_PATH] = { "../../Reference/Resources/Data/Event/Event.dat" };
	_int					m_iSelectEventCallerIndex = { 0 };

private: /* For. Animation */
	_float					m_fPlayAnimSpeed = { 0.f };

private: /* For. Sound */
	_int					m_iSelectSoundIndex = { };
	_int					m_iSelectSoundEventIndex = { };

	HRESULT		Append_SoundEvent();
	HRESULT		Delete_SoundEvent();

private: /* For. Particle */
	_int					m_iSelectShaderPass = { };
	_int					m_iSelectTextureIndex = { };

	_int					m_iSelectBoneIndex = { };
	_int					m_iSelectParticleIndex = { };
	_int					m_iSelectParticleEventIndex = { };

	_float					m_fParticleEventDuration = { -1.f };

private: /* For. Function */
	char					m_szFunctionTag[MAX_PATH] = { "" };
	_int					m_iSelectFunctionEventIndex = { };

private: /* For. Camera */
	_bool					m_bCameraMove = { FALSE };

private: /* ImGui Window */
	void	Main_Window();
	void	Sub_Window();
	void	ConsoleLog_Window();

	/* Material Tab */
	void	MainWin_Material();
	void	MaterialTab_Model();
	void	MaterialTab_Animation();

	/* Event Tab */
	void	MainWin_Event();
	void	EventTab_Data();
	void	EventTab_Sound();
	void	EventTab_Effect();
	void	EventTab_Particle();
	void	EventTab_Function();
	void	EventTab_Translation();

	/* Camera Tab */
	void	SubWin_Camera();
	void	SubWin_PlayAnim();

public:
	virtual HRESULT Initialize() override;
	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CLevel_Editor*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void			Free() override;
};

END