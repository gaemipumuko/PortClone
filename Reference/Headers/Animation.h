#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

private:
	class CEventCaller*			m_pEventCaller = { nullptr };
	_float4x4*					m_WorldMatrix = { };
	_float4x4					m_CombinedMatrix = { };

public:
	HRESULT						Reset_EventCaller();
	HRESULT						Bind_EventCaller(class CEventCaller* pEventCaller);
	HRESULT						Invalidate_EventCaller(class CEventCaller* pEventCaller);

private:
	_char						m_szName[MAX_PATH] = { };

	_float						m_Duration = { };
	_float						m_fTickPerSecond = { };
	_float						m_CurrentAnimPosition = { };

	_uint						m_iNumChannels = { };
	vector<class CChannel*>		m_Channels;

	_bool						m_bFinished = { FALSE };
	vector<_uint>				m_CurrentKeyFrameIndices;

public:
	string				Get_AnimationTag() { return m_szName; }
	_float				Get_AnimationDuration() { return m_Duration; }
	_float				Get_AnimationCurrentDuration() { return m_CurrentAnimPosition; }

	void				Set_AnimationCurrentDuration(_float fDuration) { Reset_Anim(); m_CurrentAnimPosition = fDuration; }

public:
	void				Reset_Anim();

public:
	_bool				isFunctionTrue();

public:
	class CChannel*		Find_Channel(_uint iBoneIdx);
	class CChannel*		Find_Channel(string strBoneTag);

	_float				Invalidate_TransformationMatrix(_bool isLoop, class CTransform* pTransform, const vector<class CBone*>& Bones, _float fTimeDelta);
	_float				Invalidate_TransformationMatrix(_bool isLoop, class CTransform* pTransform, const vector<class CBone*>& Bones, _float fDuration, _float fTimeDelta);
	_float				Invalidate_TransformationMatrix_Half(_bool isLoop, class CTransform* pTransform, const vector<class CBone*>& Bones, _float fTimeDelta);

public:
	HRESULT				Initialize(const ANIM_DESC* pAnimDesc, const vector<class CBone*>& Bones);

public:
	static CAnimation*	Create(const ANIM_DESC* pAnimDesc, const vector<class CBone*>& Bones);
	CAnimation*			Clone();
	virtual void		Free() override;
};

END