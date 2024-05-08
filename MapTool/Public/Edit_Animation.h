#pragma once

#include "Tool_Defines.h"
#include "Base.h"

BEGIN(Tool)

class CEdit_Animation final : public CBase
{
private:
	CEdit_Animation();
	CEdit_Animation(const CEdit_Animation& rhs);
	virtual ~CEdit_Animation() = default;

private:
	_char						m_szName[MAX_PATH] = { };

	_float						m_Duration = { };
	_float						m_fTickPerSecond = { };
	_float						m_CurrentAnimPosition = { };

	_uint								m_iNumChannels = { };
	vector<class CEdit_Channel*>		m_Channels;

	_bool						m_bFinished = { FALSE };
	vector<_uint>				m_CurrentKeyFrameIndices;

public:
	string				Get_AnimationTag() { return m_szName; }

public:
	void				Reset_Anim()
	{
		m_bFinished = FALSE;
		m_CurrentAnimPosition = 0.f;
		fill(m_CurrentKeyFrameIndices.begin(), m_CurrentKeyFrameIndices.end(), 0);
	}

public:
	class CEdit_Channel*		Find_Channel(_uint iBoneIdx);
	_float						Invalidate_TransformationMatrix(_bool isLoop, const vector<class CEdit_Bone*>& Bones, _float fTimeDelta);
	_float						Invalidate_TransformationMatrix_Half(_bool isLoop, const vector<class CEdit_Bone*>& Bones, _float fTimeDelta);

public:
	HRESULT				Initialize(const ANIM_DESC* pAnimDesc, const vector<class CEdit_Bone*>& Bones);

public:
	static CEdit_Animation* Create(const ANIM_DESC* pAnimDesc, const vector<class CEdit_Bone*>& Bones);
	CEdit_Animation*		Clone();
	virtual void			Free() override;
};

END
