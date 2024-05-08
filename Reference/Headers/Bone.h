#pragma once

#include "Base.h"

BEGIN(Engine)

class CBone final : public CBase
{
public:
	enum BONE_TYPE { BONE_NORMAL, BONE_PART_ROOT, BONE_END };

private:
	CBone();
	virtual ~CBone() = default;

private:
	BONE_TYPE			m_eBoneType = { };

	_char				m_szName[MAX_PATH] = { };

	_int				m_iParentBoneIndex = { };
	class CBone*		m_pParentBone = { nullptr }; /* Only Use Bone_Part */

	_float4x4			m_TransformationMatrix = { };
	_float4x4			m_CombindTransformationMatrix = { };

public:
	void				Set_BoneType(BONE_TYPE eType) { m_eBoneType = eType; }
	const _char*		Get_BoneName() const { return m_szName; }
	void				Set_ParentBone(class CBone* pBone) { m_pParentBone = pBone; Safe_AddRef(m_pParentBone); }
	void				Release_ParentBone() { Safe_Release(m_pParentBone); }

	const _float4x4		Get_TransformationMatrix() { return m_TransformationMatrix; }
	void				Set_TransformationMatrix(_fmatrix TransformationMatrix) { XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix); }

	const _float4x4*	Get_CombinedMatrix() const { return &m_CombindTransformationMatrix; }

public:
	void				Invalidate_CombinedTransformationMatrix(const vector<CBone*>& Bones);
	void				Invalidate_CombinedTransformationMatrix(const vector<CBone*>& Bones, _float fRate, _float fMargin);

public:
	HRESULT				Initialize(const BONE_DESC* pBoneDesc, BONE_TYPE eType = BONE_NORMAL);

public:
	static CBone*		Create(const BONE_DESC* pBoneDesc);
	CBone*				Clone();
	virtual void		Free() override;
};

END