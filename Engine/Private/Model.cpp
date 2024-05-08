#include "Model.h"

#include "Bone.h"
#include "Mesh.h"
#include "Texture.h"
#include "Channel.h"
#include "Animation.h"
#include "Transform.h"

#include "EventCaller.h"
#include "GameInstance.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel& rhs)
	: CComponent(rhs)
	, m_strModelTag(rhs.m_strModelTag)
	, m_eModelType(rhs.m_eModelType)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_Meshes(rhs.m_Meshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Materials(rhs.m_Materials)
	, m_iCurrentAnimIndex(rhs.m_iCurrentAnimIndex)
	, m_iNumAnimations(rhs.m_iNumAnimations)
{
	for (auto& pPrototypeAnimation : rhs.m_Animations)
	{
		m_Animations.push_back(pPrototypeAnimation->Clone());
	}

	for (auto& pPrototypeBone : rhs.m_Bones)
	{
		m_Bones.push_back(pPrototypeBone->Clone());
	}

	for (auto& pMesh : m_Meshes)
		Safe_AddRef(pMesh);

	for (auto& Material : m_Materials)
		for (size_t i = 0; i < ::AI_TEXTURE_TYPE_MAX; i++)
			Safe_AddRef(Material.pMaterials[i]);
}

_char* CModel::Get_MeshName(_uint iIndex)
{
	return m_Meshes[iIndex]->Get_Name();
}

class CBone* CModel::Get_Bone(string strBoneName)
{
	for (auto& Bone : m_Bones)
	{
		if (!strcmp(Bone->Get_BoneName(), strBoneName.c_str()))
			return Bone;
	}

	return nullptr;
}

const _float4x4* CModel::Get_CombinedMatrix(string strBoneName)
{
	for (auto& Bone : m_Bones)
	{
		if (!strcmp(Bone->Get_BoneName(), strBoneName.c_str()))
			return Bone->Get_CombinedMatrix();
	}

	return nullptr;
}

const _float4x4* CModel::Get_CombinedMatrix(_uint iIndex)
{
	if (m_Bones.size() <= iIndex)
		return nullptr;

	return m_Bones[iIndex]->Get_CombinedMatrix();
}

const _char* CModel::Get_BoneName(_uint iIndex)
{
	if (m_Bones.size() <= iIndex)
		return nullptr;

	return m_Bones[iIndex]->Get_BoneName();
}

const _uint CModel::Get_NumBones()
{
	return m_Bones.size();
}

void CModel::Set_PartRootBone(string strRootNode, CBone* pBone)
{
	for (auto& Bone : m_Bones)
	{
		if (!strcmp(Bone->Get_BoneName(), strRootNode.c_str()))
		{
			Bone->Release_ParentBone();
			Bone->Set_BoneType(CBone::BONE_PART_ROOT);
			Bone->Set_ParentBone(pBone);
			return;
		}
	}
}

vector<string> CModel::Get_BoneTags()
{
	vector<string> BoneTags;

	for (auto& iter : m_Bones)
		BoneTags.push_back(iter->Get_BoneName());

	return BoneTags;
}

vector<string> CModel::Get_AnimationTags()
{
	vector<string> AnimationTags;

	for (auto& iter : m_Animations)
		AnimationTags.push_back(iter->Get_AnimationTag());

	return AnimationTags;
}

void CModel::Set_Animation(_uint iAnimIndex)
{
	if (m_iCurrentAnimIndex != iAnimIndex)
	{
		m_fConvertRatio = 0.f;
		m_iPrevAnimIndex = m_iCurrentAnimIndex;
		m_iCurrentAnimIndex = iAnimIndex;

		m_Animations[m_iCurrentAnimIndex]->Reset_Anim();
		m_Animations[m_iCurrentAnimIndex]->Reset_EventCaller();
		m_bChangeAnimation = TRUE;
	}
}

void CModel::Init_Animation(_uint iAnimIndex)
{
	m_iCurrentAnimIndex = iAnimIndex;
}

void CModel::ForceChange_Animation(_uint iAnimIndex)
{
	m_iPrevAnimIndex = iAnimIndex;
	m_iCurrentAnimIndex = iAnimIndex;

	m_Animations[m_iCurrentAnimIndex]->Reset_Anim();
	m_Animations[m_iCurrentAnimIndex]->Reset_EventCaller();
}

_float CModel::Get_AnimationCurrentDuration()
{
	return m_Animations[m_iCurrentAnimIndex]->Get_AnimationCurrentDuration();
}

_float CModel::Get_AnimationPrevDuration()
{
	return m_Animations[m_iPrevAnimIndex]->Get_AnimationCurrentDuration();
}

_float CModel::Get_AnimationDuration(_uint iAnimIndex)
{
	return m_Animations[iAnimIndex]->Get_AnimationDuration();
}

void CModel::Set_AnimationCurrentDuration(_float fDuration)
{
	m_Animations[m_iCurrentAnimIndex]->Set_AnimationCurrentDuration(fDuration);
}

KEYFRAME CModel::Get_InitKeyFrame(_uint iAnimIndex, string strBoneName)
{
	if (m_Animations.size() < iAnimIndex)
		return KEYFRAME();

	CChannel* pChannel = m_Animations[iAnimIndex]->Find_Channel(strBoneName);
	if (pChannel == nullptr)
		return KEYFRAME();

	return pChannel->Get_FirstFrame();
}

KEYFRAME CModel::Get_PrevKeyFrame(_uint iAnimIndex, string strBoneName)
{
	if (m_Animations.size() < iAnimIndex)
		return KEYFRAME();

	CChannel* pChannel = m_Animations[iAnimIndex]->Find_Channel(strBoneName);
	if (pChannel == nullptr)
		return KEYFRAME();

	return pChannel->Get_PrevFrame();
}

_vector CModel::Get_InitBoneTransform(string strBoneName, ROOTBONETRANSFORMTYPE eTransformType)
{
	// 뼈를 찾아봐
	CBone* pBone = Get_Bone(strBoneName);

	// 뼈가 없네?, 쓰레기 값을 반환해
	if (pBone == nullptr)
		return XMVectorSet(0.f, 0.f, 0.f, 0.f);

	_vector vScale = { }, vRotation = { }, vTranslation = { };
	XMMatrixDecompose(&vScale, &vRotation, &vTranslation,
		XMLoadFloat4x4(Get_Bone(strBoneName)->Get_CombinedMatrix()));

	switch (eTransformType)
	{
	case ROOTBONETRANSFORMTYPE::SCALE:
		return vScale;
	case ROOTBONETRANSFORMTYPE::ROTATION:
		return vRotation;
	case ROOTBONETRANSFORMTYPE::TRANSLATION:
		return vTranslation;
	default:
		return XMVectorSet(0.f, 0.f, 0.f, 1.f);
	}
}

_vector CModel::Get_CurrBoneTransform(string strBoneName, ROOTBONETRANSFORMTYPE eTransformType)
{
	// 뼈를 찾아봐
	CBone* pBone = Get_Bone(strBoneName);

	// 뼈가 없네?, 쓰레기 값을 반환해
	if (pBone == nullptr)
		return XMVectorSet(0.f, 0.f, 0.f, 0.f);

	_vector vScale = { }, vRotation = { }, vTranslation = { };
	XMMatrixDecompose(&vScale, &vRotation, &vTranslation,
		XMLoadFloat4x4(Get_Bone(strBoneName)->Get_CombinedMatrix()));

	switch (eTransformType)
	{
	case ROOTBONETRANSFORMTYPE::SCALE:
		return vScale;
	case ROOTBONETRANSFORMTYPE::ROTATION:
		return vRotation;
	case ROOTBONETRANSFORMTYPE::TRANSLATION:
		return vTranslation;
	default:
		return XMVectorSet(0.f, 0.f, 0.f, 1.f);
	}
}

_bool CModel::isFunctionTrue()
{
	return m_Animations[m_iCurrentAnimIndex]->isFunctionTrue();
}

HRESULT CModel::Ready_Bones(ifstream& inFile)
{
	_int iNumBones = 0;

	inFile.read(reinterpret_cast<char*>(&iNumBones), sizeof _uint);
	m_Bones.reserve(iNumBones);

	for (size_t idx = 0; idx < iNumBones; ++idx)
	{
		BONE_DESC tBoneDesc = { };

		inFile.read(reinterpret_cast<char*>(&tBoneDesc.szName),						sizeof (char[MAX_PATH]));
		inFile.read(reinterpret_cast<char*>(&tBoneDesc.iParentBoneIndex),			sizeof _int);

		inFile.read(reinterpret_cast<char*>(&tBoneDesc.TransformationMatrix.m[0]),	sizeof _float4);
		inFile.read(reinterpret_cast<char*>(&tBoneDesc.TransformationMatrix.m[1]),	sizeof _float4);
		inFile.read(reinterpret_cast<char*>(&tBoneDesc.TransformationMatrix.m[2]),	sizeof _float4);
		inFile.read(reinterpret_cast<char*>(&tBoneDesc.TransformationMatrix.m[3]),	sizeof _float4);

		CBone* pBone = CBone::Create(&tBoneDesc);
		if (nullptr == pBone)
			return E_FAIL;

		m_Bones.push_back(pBone);
	}

	return S_OK;
}

HRESULT CModel::Ready_Meshes(ifstream& inFile)
{
	inFile.read(reinterpret_cast<char*>(&m_iNumMeshes), sizeof _uint);
	m_Meshes.reserve(m_iNumMeshes);

	for (size_t idx = 0; idx < m_iNumMeshes; ++idx)
	{
		MESH_DESC tMeshDesc = { };

		inFile.read(reinterpret_cast<char*>(&tMeshDesc.szName),			sizeof (char[MAX_PATH]));
		inFile.read(reinterpret_cast<char*>(&tMeshDesc.iMaterialIndex), sizeof _uint);
		inFile.read(reinterpret_cast<char*>(&tMeshDesc.iNumVertices),	sizeof _uint);
		inFile.read(reinterpret_cast<char*>(&tMeshDesc.iNumIndices),	sizeof _uint);

		tMeshDesc.vPositions		= new XMFLOAT3[tMeshDesc.iNumVertices];
		tMeshDesc.vNormals			= new XMFLOAT3[tMeshDesc.iNumVertices];
		tMeshDesc.vTextureCoords	= new XMFLOAT3[tMeshDesc.iNumVertices];
		tMeshDesc.vTangents			= new XMFLOAT3[tMeshDesc.iNumVertices];

		inFile.read(reinterpret_cast<char*>(tMeshDesc.vPositions),		sizeof _float3 * tMeshDesc.iNumVertices);
		inFile.read(reinterpret_cast<char*>(tMeshDesc.vNormals),		sizeof _float3 * tMeshDesc.iNumVertices);
		inFile.read(reinterpret_cast<char*>(tMeshDesc.vTextureCoords),	sizeof _float3 * tMeshDesc.iNumVertices);
		inFile.read(reinterpret_cast<char*>(tMeshDesc.vTangents),		sizeof _float3 * tMeshDesc.iNumVertices);

		if (m_eModelType == MTYPE_ANIM)
		{
			inFile.read(reinterpret_cast<char*>(&tMeshDesc.iNumBones),	sizeof _uint);

			tMeshDesc.OffsetMatices = new _float4x4[tMeshDesc.iNumBones];
			tMeshDesc.BoneIndices	= new _uint[tMeshDesc.iNumBones];

			inFile.read(reinterpret_cast<char*>(tMeshDesc.OffsetMatices), sizeof _float4x4 * tMeshDesc.iNumBones);
			inFile.read(reinterpret_cast<char*>(tMeshDesc.BoneIndices),	  sizeof _uint	   * tMeshDesc.iNumBones);

			tMeshDesc.vBlendIndices = new XMUINT4[tMeshDesc.iNumVertices];
			tMeshDesc.vBlendWeights = new _float4[tMeshDesc.iNumVertices];

			inFile.read(reinterpret_cast<char*>(tMeshDesc.vBlendIndices),	sizeof XMUINT4 * tMeshDesc.iNumVertices);
			inFile.read(reinterpret_cast<char*>(tMeshDesc.vBlendWeights),	sizeof _float4 * tMeshDesc.iNumVertices);
		}

		tMeshDesc.pIndices			= new _ulong[tMeshDesc.iNumIndices];

		inFile.read(reinterpret_cast<char*>(tMeshDesc.pIndices), sizeof _ulong * tMeshDesc.iNumIndices);

		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, &tMeshDesc, m_Bones, XMLoadFloat4x4(&m_PivotMatrix));
		if (pMesh == nullptr)
			return E_FAIL;

		Safe_Delete_Array(tMeshDesc.vPositions);
		Safe_Delete_Array(tMeshDesc.vNormals);
		Safe_Delete_Array(tMeshDesc.vTextureCoords);
		Safe_Delete_Array(tMeshDesc.vTangents);

		if (m_eModelType == MTYPE_ANIM)
		{
			Safe_Delete_Array(tMeshDesc.OffsetMatices);
			Safe_Delete_Array(tMeshDesc.BoneIndices);

			Safe_Delete_Array(tMeshDesc.vBlendIndices);
			Safe_Delete_Array(tMeshDesc.vBlendWeights);
		}

		Safe_Delete_Array(tMeshDesc.pIndices);

		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(ifstream& inFile, const string& strModelFilePath)
{
	inFile.read(reinterpret_cast<char*>(&m_iNumMaterials), sizeof _uint);
	m_Meshes.reserve(m_iNumMeshes);

	_char		szDrive[MAX_PATH] = "";
	_char		szDirectory[MAX_PATH] = "";
	_char		szFileName[MAX_PATH] = "";
	_char		szExt[MAX_PATH] = "";
	
	_char		szTextureFilePath[MAX_PATH] = "";
	
	_splitpath_s(strModelFilePath.c_str(), szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);
	
	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		MODEL_MATERIAL			ModelMaterials = {};
	
		_uint iNumTexture = 0;
		inFile.read(reinterpret_cast<char*>(&iNumTexture), sizeof _uint);
	
		for (size_t j = 0; j < iNumTexture; j++)
		{
			_uint	iMaterialIdx = 0;
			char	szTexturePath[MAX_PATH];
	
			inFile.read(reinterpret_cast<char*>(&iMaterialIdx),		sizeof _uint);
			inFile.read(reinterpret_cast<char*>(&szTexturePath),	sizeof (char[MAX_PATH]));

			_splitpath_s(szTexturePath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);
	
			strcpy_s(szTextureFilePath, szDrive);
			strcat_s(szTextureFilePath, szDirectory);
			strcat_s(szTextureFilePath, szFileName);
			strcat_s(szTextureFilePath, szExt);

			wstring strPrototypeTag = L"Prototype_Component_Texture_";
			strPrototypeTag += ConvertCToWC(szFileName);

			/* Texture Find or Create */
			CTexture* pTexture = static_cast<CTexture*>(m_pGameInstance->Find_Component_Prototype(0, strPrototypeTag));
	
			if (pTexture == nullptr)
			{
				_tchar			szFullPath[MAX_PATH] = L"";
				MultiByteToWideChar(CP_ACP, 0, szTextureFilePath, static_cast<_int>(strlen(szTextureFilePath)), szFullPath, MAX_PATH);

				if (FAILED(m_pGameInstance->Add_Component_Prototype(0, strPrototypeTag,
					pTexture = CTexture::Create(m_pDevice, m_pContext, szFullPath))))
					return E_FAIL;
			}

			if (pTexture == nullptr)
				return E_FAIL;

			ModelMaterials.pMaterials[iMaterialIdx] = pTexture;
			Safe_AddRef(pTexture);
	
			if (nullptr == ModelMaterials.pMaterials[iMaterialIdx])
				return E_FAIL;
		}
	
		m_Materials.push_back(ModelMaterials);
	}

	return S_OK;
}

HRESULT CModel::Ready_Animations(ifstream& inFile)
{
	inFile.read(reinterpret_cast<char*>(&m_iNumAnimations), sizeof _uint);
	m_Animations.reserve(m_iNumAnimations);

	for (size_t idx = 0; idx < m_iNumAnimations; ++idx)
	{
		ANIM_DESC tAnimDesc = { };

		inFile.read(reinterpret_cast<char*>(&tAnimDesc.szName),				sizeof (char[MAX_PATH]));
		inFile.read(reinterpret_cast<char*>(&tAnimDesc.dDuration),			sizeof _double);
		inFile.read(reinterpret_cast<char*>(&tAnimDesc.dTicksPerSecond),	sizeof _double);
		inFile.read(reinterpret_cast<char*>(&tAnimDesc.iNumChannels),		sizeof _uint);

		tAnimDesc.pChannelDesc = new CH_DESC[tAnimDesc.iNumChannels];
		for (size_t chIdx = 0; chIdx < tAnimDesc.iNumChannels; ++chIdx)
		{
			inFile.read(reinterpret_cast<char*>(&tAnimDesc.pChannelDesc[chIdx].szName),			sizeof (char[MAX_PATH]));
			inFile.read(reinterpret_cast<char*>(&tAnimDesc.pChannelDesc[chIdx].iBoneIndex),		sizeof _uint);
			inFile.read(reinterpret_cast<char*>(&tAnimDesc.pChannelDesc[chIdx].iNumKeyFrames),	sizeof _uint);

			tAnimDesc.pChannelDesc[chIdx].pKeyFrames = new KEYFRAME[tAnimDesc.pChannelDesc[chIdx].iNumKeyFrames];
			for (size_t frameIdx = 0; frameIdx < tAnimDesc.pChannelDesc[chIdx].iNumKeyFrames; ++frameIdx)
			{
				inFile.read(reinterpret_cast<char*>(&tAnimDesc.pChannelDesc[chIdx].pKeyFrames[frameIdx].fTime), sizeof _float);
				inFile.read(reinterpret_cast<char*>(&tAnimDesc.pChannelDesc[chIdx].pKeyFrames[frameIdx].vScale), sizeof XMFLOAT4);
				inFile.read(reinterpret_cast<char*>(&tAnimDesc.pChannelDesc[chIdx].pKeyFrames[frameIdx].vRotation), sizeof XMFLOAT4);
				inFile.read(reinterpret_cast<char*>(&tAnimDesc.pChannelDesc[chIdx].pKeyFrames[frameIdx].vTranslation), sizeof XMFLOAT4);
			}
		}

		CAnimation* pAnimation = CAnimation::Create(&tAnimDesc, m_Bones);
		if (nullptr == pAnimation)
			return E_FAIL;

		for (size_t chIdx = 0; chIdx < tAnimDesc.iNumChannels; ++chIdx)
		{
			Safe_Delete_Array(tAnimDesc.pChannelDesc[chIdx].pKeyFrames);
		}
		
		Safe_Delete_Array(tAnimDesc.pChannelDesc);

		m_Animations.push_back(pAnimation);
	}

	return S_OK;
}

HRESULT CModel::Bind_BoneMatrices(_uint iMeshIndex, CShader* pShader, const _char* pConstantName)
{
	return m_Meshes[iMeshIndex]->Bind_BoneMatrices(pShader, m_Bones, pConstantName, XMLoadFloat4x4(&m_PivotMatrix));
}

HRESULT CModel::Bind_Material(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, TEXTURETYPE eMaterialType)
{
	_uint		iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	if (m_Materials[iMaterialIndex].pMaterials[eMaterialType] == nullptr)
		return E_NOINTERFACE;

	return m_Materials[iMaterialIndex].pMaterials[eMaterialType]->Bind_ShaderResource(pShader, pConstantName, 0);
}

void CModel::Invalidate_EventCaller()
{
	for (auto& Animation : m_Animations)
	{
		Animation->Invalidate_EventCaller(m_pGameInstance->Clone_EventCaller(m_strModelTag, Animation->Get_AnimationTag()));
	}
}

void CModel::Invalidate_CombinedTransformationMatrix()
{
	for (auto& pBone : m_Bones)
	{
		pBone->Invalidate_CombinedTransformationMatrix(m_Bones);
	}
}

void CModel::Invalidate_CombinedTransformationMatrix(_float fRate, _float fMargin)
{
	for (auto& pBone : m_Bones)
	{
		pBone->Invalidate_CombinedTransformationMatrix(m_Bones, fRate, fMargin);
	}
}

_float CModel::Play_Animation(_bool isLoop, class CTransform* pTransform, _float fTimeDelta)
{
	_float fCurrentAnimPosition = 0.f;

	if (m_bChangeAnimation)
	{
		m_fConvertRatio += fTimeDelta * m_fConvertSpeed;

		CModel::Change_Animation();

		if (m_fConvertRatio > 1.f)
		{
			m_bChangeAnimation = FALSE;
		}
	}
	else
		fCurrentAnimPosition = m_Animations[m_iCurrentAnimIndex]->Invalidate_TransformationMatrix(isLoop, pTransform, m_Bones, fTimeDelta);

	/* 모든 뼈들의 CombinedTransformationMatrix를 갱신한다. */
	for (auto& pBone : m_Bones)
	{
		pBone->Invalidate_CombinedTransformationMatrix(m_Bones);
	}

	return fCurrentAnimPosition;
}

_float CModel::Play_RootAnimation(_bool isLoop, class CTransform* pTransform, _float fTimeDelta, _vector& vRootTranslation, _bool bLerp)
{
	_float fCurrentAnimPosition = 0.f;

	if (m_bChangeAnimation)
	{
		m_fConvertRatio += fTimeDelta * m_fConvertSpeed;

		if(bLerp == TRUE)
			CModel::Change_Animation();

		if (m_fConvertRatio > 1.f)
		{
			m_bChangeAnimation = FALSE;
		}
	}
	else
			fCurrentAnimPosition = m_Animations[m_iCurrentAnimIndex]->Invalidate_TransformationMatrix(isLoop, pTransform, m_Bones, fTimeDelta);

	/* 모든 뼈들의 CombinedTransformationMatrix를 갱신한다. */
	for (auto& pBone : m_Bones)
	{
		pBone->Invalidate_CombinedTransformationMatrix(m_Bones);

		// 임시
		//// 회전 값을 제거한다.
		//_matrix boneMatrix = XMMatrixRotationQuaternion(Get_CurrBoneTransform("Bone_Root", ROOTBONETRANSFORMTYPE::ROTATION));
		//boneMatrix = XMMatrixIdentity();

		//// 업데이트된 행렬을 다시 뼈에 설정한다.
		////m_CombinedTransformationMatrix = boneMatrix;
	}

	if (Get_Bone(m_strRootBone) != nullptr)
		memcpy(&vRootTranslation, Get_Bone(m_strRootBone)->Get_TransformationMatrix().m[3], sizeof _float4);
	else
		vRootTranslation = { 0.f, 0.f, 0.f, 1.f };

	return fCurrentAnimPosition;
}

_float CModel::Play_PlayerRootAnimation(_bool isLoop, CTransform* pTransform, _float fTimeDelta, _vector& vRootTranslation)
{
	_float fCurrentAnimPosition = 0.f;

	if (m_bChangeAnimation)
	{
		m_fConvertRatio += fTimeDelta * m_fConvertSpeed;

		CModel::Change_Animation();

		if (m_fConvertRatio > 1.f)
		{
			m_bChangeAnimation = FALSE;
		}
	}
	else
		fCurrentAnimPosition = m_Animations[m_iCurrentAnimIndex]->Invalidate_TransformationMatrix(isLoop, pTransform, m_Bones, fTimeDelta);

	/* 모든 뼈들의 CombinedTransformationMatrix를 갱신한다. */
	for (auto& pBone : m_Bones)
	{
		pBone->Invalidate_CombinedTransformationMatrix(m_Bones);
	}

	if (Get_Bone(m_strPlayerRootBone) != nullptr)
		memcpy(&vRootTranslation, Get_Bone(m_strPlayerRootBone)->Get_TransformationMatrix().m[3], sizeof _float4);
	else
		vRootTranslation = { 0.f, 0.f, 0.f, 1.f };

	return fCurrentAnimPosition;
}

_float CModel::Play_PlayerRootAnimation_NoHeight(_bool isLoop, CTransform* pTransform, _float fTimeDelta, _vector& vRootTranslation)
{
	_float fCurrentAnimPosition = 0.f;

	if (m_bChangeAnimation)
	{
		m_fConvertRatio += fTimeDelta * m_fConvertSpeed;

		CModel::Change_Animation();

		if (m_fConvertRatio > 1.f)
		{
			m_bChangeAnimation = FALSE;
		}
	}
	else
		fCurrentAnimPosition = m_Animations[m_iCurrentAnimIndex]->Invalidate_TransformationMatrix(isLoop, pTransform, m_Bones, fTimeDelta);

	CBone* pRootBone = Get_Bone(m_strPlayerRootBone);
	if (pRootBone != nullptr)
	{
		_float4x4 RootMatrix = pRootBone->Get_TransformationMatrix();
		RootMatrix._42 = 1.1f;
		pRootBone->Set_TransformationMatrix(XMLoadFloat4x4(&RootMatrix));
	}

	/* 모든 뼈들의 CombinedTransformationMatrix를 갱신한다. */
	for (auto& pBone : m_Bones)
	{
		pBone->Invalidate_CombinedTransformationMatrix(m_Bones);
	}

	if (Get_Bone(m_strPlayerRootBone) != nullptr)
		memcpy(&vRootTranslation, Get_Bone(m_strPlayerRootBone)->Get_TransformationMatrix().m[3], sizeof _float4);
	else
		vRootTranslation = { 0.f, 0.f, 0.f, 1.f };

	return fCurrentAnimPosition;
}

_float CModel::Play_PlayerRootAnimation_NoHeight(_bool isLoop, CTransform* pTransform, _float fTimeDelta, _vector& vRootTranslation, _float fMargin)
{
	_float fCurrentAnimPosition = 0.f;

	if (m_bChangeAnimation)
	{
		m_fConvertRatio += fTimeDelta * m_fConvertSpeed;

		CModel::Change_Animation();

		if (m_fConvertRatio > 1.f)
		{
			m_bChangeAnimation = FALSE;
		}
	}
	else
		fCurrentAnimPosition = m_Animations[m_iCurrentAnimIndex]->Invalidate_TransformationMatrix(isLoop, pTransform, m_Bones, fTimeDelta);

	CBone* pRootBone = Get_Bone(m_strPlayerRootBone);
	if (pRootBone != nullptr)
	{
		_float4x4 RootMatrix = pRootBone->Get_TransformationMatrix();
		RootMatrix._42 = fMargin;
		pRootBone->Set_TransformationMatrix(XMLoadFloat4x4(&RootMatrix));
	}

	/* 모든 뼈들의 CombinedTransformationMatrix를 갱신한다. */
	for (auto& pBone : m_Bones)
	{
		pBone->Invalidate_CombinedTransformationMatrix(m_Bones);
	}

	if (Get_Bone(m_strPlayerRootBone) != nullptr)
		memcpy(&vRootTranslation, Get_Bone(m_strPlayerRootBone)->Get_TransformationMatrix().m[3], sizeof _float4);
	else
		vRootTranslation = { 0.f, 0.f, 0.f, 1.f };

	return fCurrentAnimPosition;
}

_float CModel::Play_BruteRootAnimation(_bool isLoop, CTransform* pTransform, _float fTimeDelta, _vector& vRootTranslation)
{
	_float fCurrentAnimPosition = 0.f;

	if (m_bChangeAnimation)
	{
		m_fConvertRatio += fTimeDelta * m_fConvertSpeed;

		CModel::Change_Animation();

		if (m_fConvertRatio > 1.f)
		{
			m_bChangeAnimation = FALSE;
		}
	}
	else
		fCurrentAnimPosition = m_Animations[m_iCurrentAnimIndex]->Invalidate_TransformationMatrix(isLoop, pTransform, m_Bones, fTimeDelta);

	/* 모든 뼈들의 CombinedTransformationMatrix를 갱신한다. */
	for (auto& pBone : m_Bones)
	{
		pBone->Invalidate_CombinedTransformationMatrix(m_Bones);
	}

	if (Get_Bone(m_strBruteRootBone) != nullptr)
	{
		memcpy(&vRootTranslation, Get_Bone(m_strBruteRootBone)->Get_TransformationMatrix().m[3], sizeof _float4);
		vRootTranslation *= 1.48f;
	}
	else
		vRootTranslation = { 0.f, 0.f, 0.f, 1.f };

	return fCurrentAnimPosition;
}

_float CModel::Play_EffigyRootAnimation(_bool isLoop, CTransform* pTransform, _float fTimeDelta, _vector& vRootTranslation)
{
	_float fCurrentAnimPosition = 0.f;

	if (m_bChangeAnimation)
	{
		m_fConvertRatio += fTimeDelta * m_fConvertSpeed;

		CModel::Change_Animation();

		if (m_fConvertRatio > 1.f)
		{
			m_bChangeAnimation = FALSE;
		}
	}
	else
		fCurrentAnimPosition = m_Animations[m_iCurrentAnimIndex]->Invalidate_TransformationMatrix(isLoop, pTransform, m_Bones, fTimeDelta);

	/* 모든 뼈들의 CombinedTransformationMatrix를 갱신한다. */
	for (auto& pBone : m_Bones)
	{
		pBone->Invalidate_CombinedTransformationMatrix(m_Bones);
	}

	if (Get_Bone(m_strEffigyRootBone) != nullptr)
	{
		memcpy(&vRootTranslation, Get_Bone(m_strEffigyRootBone)->Get_TransformationMatrix().m[3], sizeof _float4);
	}
	else
		vRootTranslation = { 0.f, 0.f, 0.f, 1.f };

	return fCurrentAnimPosition;
}

_float CModel::Play_Abyss_MageRootAnimation(_bool isLoop, CTransform* pTransform, _float fTimeDelta, _vector& vRootTranslation)
{
	_float fCurrentAnimPosition = 0.f;

	if (m_bChangeAnimation)
	{
		m_fConvertRatio += fTimeDelta * m_fConvertSpeed;

		CModel::Change_Animation();

		if (m_fConvertRatio > 1.f)
		{
			m_bChangeAnimation = FALSE;
		}
	}
	else
		fCurrentAnimPosition = m_Animations[m_iCurrentAnimIndex]->Invalidate_TransformationMatrix_Half(isLoop, pTransform, m_Bones, fTimeDelta);

	/* 모든 뼈들의 CombinedTransformationMatrix를 갱신한다. */
	for (auto& pBone : m_Bones)
	{
		pBone->Invalidate_CombinedTransformationMatrix(m_Bones);
	}

	if (Get_Bone(m_strPlayerRootBone) != nullptr)
		memcpy(&vRootTranslation, Get_Bone(m_strPlayerRootBone)->Get_TransformationMatrix().m[3], sizeof _float4);
	else
		vRootTranslation = { 0.f, 0.f, 0.f, 1.f };

	return fCurrentAnimPosition;
}

_float CModel::Play_Animation_Half(_bool isLoop, class CTransform* pTransform, _float fTimeDelta)
{
	_float fCurrentAnimPosition = 0.f;

	if (m_bChangeAnimation)
	{
		m_fConvertRatio += fTimeDelta * m_fConvertSpeed;

		CModel::Change_Animation();

		if (m_fConvertRatio > 1.f)
		{
			m_bChangeAnimation = FALSE;
		}
	}
	else
		fCurrentAnimPosition = m_Animations[m_iCurrentAnimIndex]->Invalidate_TransformationMatrix_Half(isLoop, pTransform, m_Bones, fTimeDelta);

	/* 모든 뼈들의 CombinedTransformationMatrix를 갱신한다. */
	for (auto& pBone : m_Bones)
	{
		pBone->Invalidate_CombinedTransformationMatrix(m_Bones);
	}

	return fCurrentAnimPosition;
}

void CModel::Change_Animation()
{
	_vector			vScale = { };
	_vector			vRotation = { };
	_vector			vTranslation = { };

	_vector			vSourScale, vDestScale;
	_vector			vSourRotation, vDestRotation;
	_vector			vSourTranslation, vDestTranslation;

	for (_uint iBoneIdx = 0; iBoneIdx < m_Bones.size(); ++iBoneIdx)
	{
		CChannel* pPrevBoneCH = m_Animations[m_iPrevAnimIndex]->Find_Channel(iBoneIdx);
		CChannel* pNextBoneCH = m_Animations[m_iCurrentAnimIndex]->Find_Channel(iBoneIdx);

		if (pPrevBoneCH == nullptr && pNextBoneCH == nullptr)
			continue;

		else if (pPrevBoneCH == nullptr)
		{
			XMMatrixDecompose(&vSourScale, &vSourRotation, &vSourTranslation, XMLoadFloat4x4(&m_Bones[iBoneIdx]->Get_TransformationMatrix()));

			vDestScale			= XMLoadFloat4(&pNextBoneCH->Get_FirstFrame().vScale);
			vScale				= XMVectorLerp(vSourScale, vDestScale, m_fConvertRatio);

			vDestRotation		= XMLoadFloat4(&pNextBoneCH->Get_FirstFrame().vRotation);
			vRotation			= XMQuaternionSlerp(vSourRotation, vDestRotation, m_fConvertRatio);

			vDestTranslation	= XMLoadFloat4(&pNextBoneCH->Get_FirstFrame().vTranslation);
			vTranslation		= XMVectorLerp(vSourTranslation, vDestTranslation, m_fConvertRatio);

			m_Bones[iBoneIdx]->Set_TransformationMatrix(XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation));
		}

		else if (pNextBoneCH == nullptr)
		{
			if (XMVector4Equal(XMLoadFloat4(&pPrevBoneCH->Get_PrevFrame().vScale), { 0.f,0.f,0.f,0.f }) ||
				XMVector4Equal(XMLoadFloat4(&pPrevBoneCH->Get_PrevFrame().vRotation), { 0.f,0.f,0.f,0.f }) ||
				XMVector4Equal(XMLoadFloat4(&pPrevBoneCH->Get_PrevFrame().vTranslation), { 0.f,0.f,0.f,0.f }))
			{
				XMMatrixDecompose(&vScale, &vRotation, &vTranslation, XMLoadFloat4x4(&m_Bones[iBoneIdx]->Get_TransformationMatrix()));
			}
			else
			{
				vScale			= XMLoadFloat4(&pPrevBoneCH->Get_PrevFrame().vScale);
				vRotation		= XMLoadFloat4(&pPrevBoneCH->Get_PrevFrame().vRotation);
				vTranslation	= XMLoadFloat4(&pPrevBoneCH->Get_PrevFrame().vTranslation);
			}

			m_Bones[iBoneIdx]->Set_TransformationMatrix(XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation));
		}

		else
		{
			if (XMVector4Equal(XMLoadFloat4(&pPrevBoneCH->Get_PrevFrame().vScale), { 0.f,0.f,0.f,0.f }) ||
				XMVector4Equal(XMLoadFloat4(&pPrevBoneCH->Get_PrevFrame().vRotation), { 0.f,0.f,0.f,0.f }) ||
				XMVector4Equal(XMLoadFloat4(&pPrevBoneCH->Get_PrevFrame().vTranslation), { 0.f,0.f,0.f,0.f }))
			{
				XMMatrixDecompose(&vSourScale, &vSourRotation, &vSourTranslation, XMLoadFloat4x4(&m_Bones[iBoneIdx]->Get_TransformationMatrix()));
			}
			else
			{
				vSourScale = XMLoadFloat4(&pPrevBoneCH->Get_PrevFrame().vScale);
				vSourRotation = XMLoadFloat4(&pPrevBoneCH->Get_PrevFrame().vRotation);
				vSourTranslation = XMLoadFloat4(&pPrevBoneCH->Get_PrevFrame().vTranslation);
			}

			vDestScale		= XMLoadFloat4(&pNextBoneCH->Get_FirstFrame().vScale);
			vScale			= XMVectorLerp(vSourScale, vDestScale, m_fConvertRatio);

			vDestRotation	= XMLoadFloat4(&pNextBoneCH->Get_FirstFrame().vRotation);
			vRotation		= XMQuaternionSlerp(vSourRotation, vDestRotation, m_fConvertRatio);

			vDestTranslation = XMLoadFloat4(&pNextBoneCH->Get_FirstFrame().vTranslation);
			vTranslation	= XMVectorLerp(vSourTranslation, vDestTranslation, m_fConvertRatio);

			m_Bones[iBoneIdx]->Set_TransformationMatrix(XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation));
		}
	}
}

void CModel::Reset_EventCaller()
{
	m_Animations[m_iCurrentAnimIndex]->Reset_EventCaller();
}

HRESULT CModel::Cooking_StaticModel(_float4x4 vWorldMatrix, const char* pMeshName, PxRigidStatic** ppRigidStatic)
{
	if (m_iNumMeshes <= 0)
		return E_FAIL;

	string szName07 = "Level_MdDungeon_Far07"; //ConvertWCToC(TEXT("Level_MdDungeon_Far07"));
	string szName09 = "Level_MdDungeon_Far09"; //ConvertWCToC(TEXT("Level_MdDungeon_Far09"));
	string szNameGrass = "Stages_SGrass"; //ConvertWCToC(TEXT("Stages_SGrass"));
	string szNameStone = "Area_MdProps_SdStone"; //ConvertWCToC(TEXT("Area_MdProps_SdStone"));
	string szNameWallGrass = "Stages_LYLvy"; //ConvertWCToC(TEXT("Stages_LYLvy"));

	for (_uint i = 0; i < m_iNumMeshes; i++) 
	{
		string strMeshName = m_Meshes[i]->Get_Name();

		string strName = strMeshName.substr(0, szName07.length());

		if (0 == strName.compare(szName07) || 0 == strName.compare(szName09))
			continue;

		strName = strMeshName.substr(0, szNameGrass.length());

		if (0 == strName.compare(szNameGrass))
			continue;

		strName = strMeshName.substr(0, szNameStone.length());

		if (0 == strName.compare(szNameStone))
			continue;

		strName = strMeshName.substr(0, szNameWallGrass.length());

		if (0 == strName.compare(szNameWallGrass))
			continue;

		if (FAILED(m_Meshes[i]->Cooking_StaticMesh(vWorldMatrix, pMeshName, ppRigidStatic)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CModel::Compute_Extents(_float3* pMin, _float3* pMax)
{
	if (nullptr == pMin || nullptr == pMax)
		return E_FAIL;

	for (_uint i = 0; i < m_iNumMeshes; i++) 
		m_Meshes[i]->Compute_VertexExtremes(pMin, pMax);

	return S_OK;
}

HRESULT CModel::Initialize_Prototype(const string& strModelFilePath)
{
	ifstream inFile(ConvertCToWC(strModelFilePath.c_str()), ios::binary | ios::in);
	if (inFile.is_open() == FALSE) {
		return E_FAIL;
	}

	_char	szFileName[MAX_PATH] = ("");
	_splitpath_s(strModelFilePath.c_str(), NULL, NULL, NULL, NULL, szFileName, MAX_PATH, NULL, NULL);

	m_strModelTag = ConvertCToWC(SplitBySeparator(szFileName, '.')[0]);

	/* Model Type */
	inFile.read(reinterpret_cast<char*>(&m_eModelType), sizeof _int);

	/* Pivot */
	inFile.read(reinterpret_cast<char*>(&m_PivotMatrix.m[0]), sizeof _float4);
	inFile.read(reinterpret_cast<char*>(&m_PivotMatrix.m[1]), sizeof _float4);
	inFile.read(reinterpret_cast<char*>(&m_PivotMatrix.m[2]), sizeof _float4);
	inFile.read(reinterpret_cast<char*>(&m_PivotMatrix.m[3]), sizeof _float4);

	if (m_eModelType == MTYPE_ANIM)
	{
		if (FAILED(Ready_Bones(inFile)))
		{
			inFile.close();
			return E_FAIL;
		}
	}

	if (FAILED(Ready_Meshes(inFile)))
	{
		inFile.close();
		return E_FAIL;
	}

	if (FAILED(Ready_Materials(inFile, strModelFilePath)))
	{
		inFile.close();
		return E_FAIL;
	}

	if (m_eModelType == MTYPE_ANIM)
	{
		if (FAILED(Ready_Animations(inFile)))
		{
			inFile.close();
			return E_FAIL;
		}
	}

	inFile.close();

	return S_OK;
}

HRESULT CModel::Initialize_Clone(void* pArg)
{
	if (m_eModelType == MTYPE_ANIM)
	{
		for (auto& Animation : m_Animations)
		{
			Animation->Bind_EventCaller(m_pGameInstance->Clone_EventCaller(m_strModelTag, Animation->Get_AnimationTag()));
		}
	}

	return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const string& strModelFilePath)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strModelFilePath)))
	{
		MSG_BOX(TEXT("Failed to Create : CModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);
	m_Bones.clear();

	for (auto& pMesh : m_Meshes)
		Safe_Release(pMesh);
	m_Meshes.clear();

	for (auto& Material : m_Materials)
	{
		for (size_t i = 0; i < ::AI_TEXTURE_TYPE_MAX; i++)
		{
			Safe_Release(Material.pMaterials[i]);
		}
	}
	m_Materials.clear();

	for (auto& Anim : m_Animations)
		Safe_Release(Anim);
	m_Animations.clear();
}