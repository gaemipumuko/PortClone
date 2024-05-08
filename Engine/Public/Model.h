#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum MODEL_TYPE { MTYPE_NONANIM, MTYPE_ANIM, MTYPE_END };

private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

private:
	wstring						m_strModelTag = { }; /* Event Caller에 필요한 이름 */

	MODEL_TYPE					m_eModelType = { MTYPE_END };
	_float4x4					m_PivotMatrix = { }; /* 모델의 초기 상태를 결정 */
								
	string						m_strRootBone = { "Bone_Root" }; /* 이동 값을 가지는 Root Node */
	string						m_strPlayerRootBone = { "Bip001" }; 
	string						m_strBruteRootBone = { "Bone001" };
	string						m_strEffigyRootBone = { "Bone001" };

	vector<class CBone*>		m_Bones;
								
	_uint						m_iNumMeshes = { };
	vector<class CMesh*>		m_Meshes;
								
	_uint						m_iNumMaterials = { };
	vector<MODEL_MATERIAL>		m_Materials;

	_bool						m_bChangeAnimation = { FALSE };
	_uint						m_iNumAnimations = { };
	_float						m_fConvertRatio = { };
	_float						m_fConvertSpeed = { 5.f };
	_uint						m_iPrevAnimIndex = { };
	_uint						m_iCurrentAnimIndex = { };
	vector<class CAnimation*>	m_Animations;

public:
	void				Set_PlayerRootBone(_int iRootIndex) { m_strPlayerRootBone = Get_BoneTags()[iRootIndex]; }
	void				Set_PlayerRootBone(string strRootBone) { m_strPlayerRootBone = strRootBone; }

public:
	MODEL_TYPE			Get_ModelType() { return m_eModelType; }
	_char*				Get_MeshName(_uint iIndex);
	_uint				Get_NumMeshes() { return m_iNumMeshes; }

	class CBone*		Get_Bone(string strBoneName);
	const _float4x4*	Get_CombinedMatrix(string strBoneName);
	const _float4x4*	Get_CombinedMatrix(_uint iIndex);
	const _char*		Get_BoneName(_uint iIndex);
	const _uint			Get_NumBones();
	void				Set_PartRootBone(string strRootNode, CBone* pBone);

	_float4x4			Get_PivotMatrix() { return m_PivotMatrix; }
	void				Set_PivotMatrix(_fmatrix PivotMatrix) { XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix); }

	_float				Get_ConvertRatio() { return m_fConvertRatio; }
	_bool				Get_ChangeAnimation() { return m_bChangeAnimation; }

	vector<string>		Get_BoneTags();
	vector<string>		Get_AnimationTags();

	_uint				Get_CurrentAnimIndex() { return m_iCurrentAnimIndex; }

	void	Set_Animation(_uint iAnimIndex);
	void	Init_Animation(_uint iAnimIndex);
	void	ForceChange_Animation(_uint iAnimIndex);

	_uint	Get_AnimationCount() { return static_cast<_uint>(m_Animations.size()); }
	_float	Get_AnimationCurrentDuration();
	_float	Get_AnimationPrevDuration();
	_float	Get_AnimationDuration(_uint iAnimIndex);

	void	Set_AnimationCurrentDuration(_float fDuration);
	
	KEYFRAME	Get_InitKeyFrame(_uint iAnimIndex, string strBoneName);
	KEYFRAME	Get_PrevKeyFrame(_uint iAnimIndex, string strBoneName);

	_vector Get_InitBoneTransform(string strBoneName, ROOTBONETRANSFORMTYPE eTransformType);
	_vector Get_CurrBoneTransform(string strBoneName, ROOTBONETRANSFORMTYPE eTransformType);

public:
	void	Set_ConvertSpeed(_float fConvertSpeed) { m_fConvertSpeed = fConvertSpeed; }

public:
	_bool	isFunctionTrue();

public:
	HRESULT Ready_Bones(ifstream& inFile);
	HRESULT Ready_Meshes(ifstream& inFile);
	HRESULT Ready_Materials(ifstream& inFile, const string& strModelFilePath);
	HRESULT Ready_Animations(ifstream& inFile);

	HRESULT Bind_BoneMatrices(_uint iMeshIndex, class CShader* pShader, const _char* pConstantName);
	HRESULT Bind_Material(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, TEXTURETYPE eMaterialType);

	void	Invalidate_EventCaller();
	void	Invalidate_CombinedTransformationMatrix();
	void	Invalidate_CombinedTransformationMatrix(_float fRate, _float fMargin);

	_float	Play_Animation(_bool isLoop, class CTransform* pTransform, _float fTimeDelta);
	_float	Play_RootAnimation(_bool isLoop, class CTransform* pTransform, _float fTimeDelta, _vector& vRootTranslation, _bool bLerp = TRUE);
	_float	Play_PlayerRootAnimation(_bool isLoop, class CTransform* pTransform, _float fTimeDelta, _vector& vRootTranslation);
	_float	Play_PlayerRootAnimation_NoHeight(_bool isLoop, class CTransform* pTransform, _float fTimeDelta, _vector& vRootTranslation);
	_float	Play_PlayerRootAnimation_NoHeight(_bool isLoop, class CTransform* pTransform, _float fTimeDelta, _vector& vRootTranslation, _float fMargin);
	_float	Play_BruteRootAnimation(_bool isLoop, class CTransform* pTransform, _float fTimeDelta, _vector& vRootTranslation);
	_float	Play_EffigyRootAnimation(_bool isLoop, class CTransform* pTransform, _float fTimeDelta, _vector& vRootTranslation);
	_float	Play_Abyss_MageRootAnimation(_bool isLoop, class CTransform* pTransform, _float fTimeDelta, _vector& vRootTranslation);
	_float	Play_Animation_Half(_bool isLoop, class CTransform* pTransform, _float fTimeDelta); /* Play Frame Half */
	
	void	Change_Animation();

	void	Reset_EventCaller();

public:
	HRESULT Cooking_StaticModel(_float4x4 vWorldMatrix, const char* pMeshName = "None", PxRigidStatic** ppRigidStatic = nullptr);
	HRESULT Compute_Extents(_float3* pMin, _float3* pMax);

public:
	virtual HRESULT Initialize_Prototype(const string& strModelFilePath);
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual HRESULT Render(_uint iMeshIndex);

public:
	static	CModel*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const string& strModelFilePath);
	virtual CComponent*		Clone(void* pArg) override;
	virtual void			Free() override;
};

END