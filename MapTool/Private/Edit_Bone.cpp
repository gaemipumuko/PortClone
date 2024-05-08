#include "Edit_Bone.h"

CEdit_Bone::CEdit_Bone()
{
}

void CEdit_Bone::Invalidate_CombinedTransformationMatrix(const vector<CEdit_Bone*>& Bones)
{
	if (m_eBoneType == BONE_PART_ROOT && m_iParentBoneIndex == -1)
	{
		XMStoreFloat4x4(&m_CombindTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix) *
			XMLoadFloat4x4(&m_pParentBone->m_CombindTransformationMatrix));
	}
	else if (m_iParentBoneIndex == -1)
	{
		m_CombindTransformationMatrix = m_TransformationMatrix;
	}
	else
	{
		XMStoreFloat4x4(&m_CombindTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix) *
			XMLoadFloat4x4(&Bones[m_iParentBoneIndex]->m_CombindTransformationMatrix));
	}
}

HRESULT CEdit_Bone::Initialize(const BONE_DESC* pBoneDesc, BONE_TYPE eType)
{
	/* Bone Type */
	m_eBoneType = eType;

	/* Bone Name */
	strcpy_s(m_szName, pBoneDesc->szName);

	/* Parent Bone Idx */
	m_iParentBoneIndex = pBoneDesc->iParentBoneIndex;

	/* Transform Matrix */
	m_TransformationMatrix = pBoneDesc->TransformationMatrix;
	XMStoreFloat4x4(&m_CombindTransformationMatrix, XMMatrixIdentity());

	return S_OK;
}

CEdit_Bone* CEdit_Bone::Create(const BONE_DESC* pBoneDesc)
{
	CEdit_Bone* pInstance = new CEdit_Bone();

	if (FAILED(pInstance->Initialize(pBoneDesc)))
	{
		MSG_BOX(TEXT("Failed to Create : CEdit_Bone"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CEdit_Bone* CEdit_Bone::Clone()
{
	return new CEdit_Bone(*this);
}

void CEdit_Bone::Free()
{
	__super::Free();

	Safe_Release(m_pParentBone);
}
