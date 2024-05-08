#pragma once

#include "Tool_Defines.h"
#include "Base.h"

BEGIN(Tool)

class CEdit_Channel final : public CBase
{
private:
	CEdit_Channel();
	virtual ~CEdit_Channel() = default;

private:
	_char					m_szName[MAX_PATH] = "";

	_uint					m_iBoneIndex = { 0 };
	KEYFRAME				m_ePrevFrame = { };

	_uint					m_iNumKeyFrames = { 0 };
	vector<KEYFRAME>		m_KeyFrames;

public:
	KEYFRAME			Get_PrevFrame() { return m_ePrevFrame; }
	KEYFRAME			Get_FirstFrame() { return m_KeyFrames[0]; }
	_uint				Get_BoneIndex() { return m_iBoneIndex; }

public:
	void				Invalidate_TransformationMatrix(const vector<class CEdit_Bone*>& Bones, _uint* pCurrentKeyFrameIndex, _float fCurrentAnimPosition);

public:
	HRESULT				Initialize(const CH_DESC* pChDesc, const vector<class CEdit_Bone*>& Bones);

public:
	static CEdit_Channel* Create(const CH_DESC* pChDesc, const vector<class CEdit_Bone*>& Bones);
	virtual void		  Free() override;
};

END
