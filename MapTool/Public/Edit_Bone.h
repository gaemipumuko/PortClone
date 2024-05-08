#pragma once

#include "Tool_Defines.h"
#include "Base.h"

BEGIN(Tool)

class CEdit_Bone final : public CBase
{
public:
	enum BONE_TYPE { BONE_NORMAL, BONE_PART_ROOT, BONE_END };

private:
	CEdit_Bone();
	virtual ~CEdit_Bone() = default;

private:
	BONE_TYPE			m_eBoneType = { };

	_char				m_szName[MAX_PATH] = { };

	_int				m_iParentBoneIndex = { };
	class CEdit_Bone*	m_pParentBone = { nullptr }; /* Only Use Bone_Part */

	_float4x4			m_TransformationMatrix = { };
	_float4x4			m_CombindTransformationMatrix = { };

public:
	void				Set_BoneType(BONE_TYPE eType) { m_eBoneType = eType; }
	const _char*		Get_BoneName() const { return m_szName; }
	void				Set_ParentBone(class CEdit_Bone* pBone) { m_pParentBone = pBone; Safe_AddRef(m_pParentBone); }

	const _float4x4		Get_TransformationMatrix() { return m_TransformationMatrix; }
	void				Set_TransformationMatrix(_fmatrix TransformationMatrix) { XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix); }

	const _float4x4*	Get_CombinedMatrix() const { return &m_CombindTransformationMatrix; }

public:
	void				Invalidate_CombinedTransformationMatrix(const vector<CEdit_Bone*>& Bones);

public:
	HRESULT				Initialize(const BONE_DESC* pBoneDesc, BONE_TYPE eType = BONE_NORMAL);

public:
	static CEdit_Bone* Create(const BONE_DESC* pBoneDesc);
	CEdit_Bone*		   Clone();
	virtual void	   Free() override;
};

END
