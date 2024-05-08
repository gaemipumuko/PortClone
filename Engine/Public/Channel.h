#pragma once

#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

private:
	_char					m_szName[MAX_PATH] = "";

	_uint					m_iBoneIndex = { 0 };
	KEYFRAME				m_ePrevFrame = { };

	_uint					m_iNumKeyFrames = { 0 };
	vector<KEYFRAME>		m_KeyFrames;

public:
	KEYFRAME			Get_PrevFrame() { return m_ePrevFrame; }
	KEYFRAME			Get_FirstFrame() { return m_KeyFrames[0]; }

	string				Get_BoneTag() { return m_szName; }
	_uint				Get_BoneIndex() { return m_iBoneIndex; }

public:
	void				Invalidate_TransformationMatrix(const vector<class CBone*>& Bones, _uint* pCurrentKeyFrameIndex, _float fCurrentAnimPosition);

public:
	HRESULT				Initialize(const CH_DESC* pChDesc, const vector<class CBone*>& Bones);

public:
	static CChannel*	Create(const CH_DESC* pChDesc, const vector<class CBone*>& Bones);
	virtual void		Free() override;
};

END