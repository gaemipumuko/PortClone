#pragma once

#include "Tool_Defines.h"
#include "Component.h"

BEGIN(Engine)
class CShader;
END

BEGIN(Tool)

class CEdit_Model final : public CComponent
{
public:
	enum MODEL_TYPE { MTYPE_NONANIM, MTYPE_ANIM, MTYPE_END };

private:
	CEdit_Model(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEdit_Model(const CEdit_Model& rhs);
	virtual ~CEdit_Model() = default;

private:
	MODEL_TYPE					m_eModelType = { MTYPE_END };
	_float4x4					m_PivotMatrix = { }; /* 모델의 초기 상태를 결정 */

	vector<class CEdit_Bone*>		m_Bones;

	_uint						m_iNumMeshes = { };
	vector<class CEdit_Mesh*>		m_Meshes;

	_uint						m_iNumMaterials = { };
	vector<MODEL_MATERIAL>		m_Materials;

	_bool						m_bChangeAnimation = { FALSE };
	_uint						m_iNumAnimations = { };
	_float						m_fConvertRatio = { };
	_uint						m_iPrevAnimIndex = { };
	_uint						m_iCurrentAnimIndex = { };
	vector<class CEdit_Animation*>	m_Animations;

public:
	_char* Get_MeshName(_uint iIndex);
	_uint				Get_NumMeshes() { return m_iNumMeshes; }

	class CEdit_Bone* Get_Bone(string strBoneName);
	const _float4x4* Get_CombinedMatrix(string strBoneName);
	void				Set_PartRootBone(string strRootNode, CEdit_Bone* pBone);

	_float4x4			Get_PivotMatrix() { return m_PivotMatrix; }

	vector<string>		Get_BoneTags();
	vector<string>		Get_AnimationTags();

	void	Set_Animation(_uint iAnimIndex);
	void	Init_Animation(_uint iAnimIndex);

public:
	HRESULT Ready_Bones(ifstream& inFile);
	HRESULT Ready_Meshes(ifstream& inFile);
	HRESULT Ready_Materials(ifstream& inFile, const string& strModelFilePath);
	HRESULT Ready_Animations(ifstream& inFile);

	HRESULT Bind_BoneMatrices(_uint iMeshIndex, class CShader* pShader, const _char* pConstantName);
	HRESULT Bind_Material(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, TEXTURETYPE eMaterialType);

	void	Invalidate_CombinedTransformationMatrix();

	_float	Play_Animation(_bool isLoop, _float fTimeDelta);
	_float	Play_Animation_Half(_bool isLoop, _float fTimeDelta); /* Play Frame Half */
	void	Change_Animation();

public:
	virtual HRESULT Initialize_Prototype(const string& strModelFilePath);
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual HRESULT Render(_uint iMeshIndex);

public:
	static	CEdit_Model*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const string& strModelFilePath);
	virtual CComponent*		Clone(void* pArg) override;
	virtual void			Free() override;
};

END
