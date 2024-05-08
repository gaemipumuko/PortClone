#pragma once

namespace Engine
{
	/* Graphic_Device */
	typedef struct tagGraphic_Desc
	{
		enum WINMODE	{ MODE_FULL, MODE_WIN, MODE_END };
		HWND			hWnd;
		unsigned int	iWinSizeX, iWinSizeY;
		WINMODE			isWindowed;
	} GRAPHIC_DESC;

	/* Model */
	typedef struct tagModelMaterial
	{
		class CTexture* pMaterials[AI_TEXTURE_TYPE_MAX];
	} MODEL_MATERIAL;

	/* Bone */
	typedef struct tagBoneDesc
	{
		char		szName[MAX_PATH] = { };
		int			iParentBoneIndex = { };
		XMFLOAT4X4	TransformationMatrix = { };
	} BONE_DESC;

	/* Mesh */
	typedef struct tagMeshDesc
	{
		char				szName[MAX_PATH];
		unsigned int		iMaterialIndex;

		unsigned int		iNumVertices;
		unsigned int		iNumIndices;
		unsigned int		iNumBones;

		XMFLOAT3*			vPositions;
		XMFLOAT3*			vNormals;
		XMFLOAT3*			vTextureCoords;
		XMFLOAT3*			vTangents;
		XMUINT4*			vBlendIndices;
		XMFLOAT4*			vBlendWeights;

		XMFLOAT4X4*			OffsetMatices;
		unsigned int*		BoneIndices;

		unsigned long*		pIndices;
	} MESH_DESC;

	/* Channel */
	typedef struct tagKeyFrame
	{
		float			fTime;
		XMFLOAT4		vScale;
		XMFLOAT4		vRotation;
		XMFLOAT4		vTranslation;
	}KEYFRAME;

	typedef struct tagChannelDesc
	{
		char				szName[MAX_PATH];
		unsigned int		iBoneIndex;
		unsigned int		iNumKeyFrames;
		KEYFRAME*			pKeyFrames;
	} CH_DESC;

	/* Anim */
	typedef struct tagAnimDesc
	{
		char				szName[MAX_PATH];
		double				dDuration;
		double				dTicksPerSecond;
		unsigned int		iNumChannels;

		CH_DESC* pChannelDesc;
	} ANIM_DESC;

	/* Light */
	typedef struct tagLightDesc
	{
		enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_END };

		TYPE eLightType = TYPE_END;
		XMFLOAT4 vDirection;
		XMFLOAT4 vPosition;
		float fRange;
		
		XMFLOAT4 vDiffuse;
		XMFLOAT4 vAmbient;
		XMFLOAT4 vSpecular;
	} LIGHT_DESC;

	/* Particle */
	typedef struct tagParticleInfo
	{
		/* DIV : 발산, CONV : 응집 */
		enum PARTICLE_TYPE {
			PARTICLE_SPHERE_DIV, PARTICLE_SPHERE_CONV,
			PARTICLE_END
		};

		enum PARTICLE_OPTION {
			PARTICLE_OPTION_GRAVITY,
			PARTICLE_OPTION_ROTATION,
			PARTICLE_OPTION_ACCEL,
			PARTICLE_OPTION_COMBINED_TARGET,
			PARTICLE_OPTION_END
		};

		int			iMaxParticle = { };
		
		bool		bFade = { FALSE };
		bool		bFrameAnimation = { };
		float		fFrameAnimCycle = { };

		XMINT2		iNumParticleRowCol = { };
		XMINT2		iCurrParticleRowCol = { };

		int			iParticleEmitRate = { };			/* 한 번에 생성되는 입자 개수 */
		float		fParticleCreateTime = { };			/* 입자 생성 주기 */

		XMVECTOR	vParticleColor = { 0.f, 0.f, 0.f, 0.f };	/* ZeroVector : Random */

		/* 생성 기준점 */
		bool			bTrail = { FALSE };
		PARTICLE_TYPE	eType;
#pragma region Add Option
		unsigned int	iOption = { 0 };

		float		fGravAcc = { 9.8f }; /* 중력 가속도 */
		XMFLOAT3	vRotationXYZ = {};
		float		fAccel = { 0.f };
#pragma endregion

		float		fAngle = {0.f};

		XMFLOAT4	vParticlePosition = { };	/* 위치 벡터 */
		XMFLOAT4	vCorrectionVector = { };	/* 보정 벡터 */

		XMFLOAT4	vTrailPosition = { };		/* 트레일 기준 벡터 */
		XMFLOAT3	vDirectionMin = { -1.f, -1.f, -1.f };
		XMFLOAT3	vDirectionMax = { 1.f, 1.f, 1.f };

		XMFLOAT2	vParticleSize = { };
		XMFLOAT2	vParticleSpeed = { };
		XMFLOAT2	vParticleLifeTime = { };
	} PARTICLE_INFO;

	/* Vertex */
	typedef struct ENGINE_DLL tagVertex_Position
	{
		XMFLOAT3		vPosition;

		static const unsigned int					iNumElements = { 1 };
		static const D3D11_INPUT_ELEMENT_DESC		VertexElements[iNumElements];
	} VTXPOS;

	typedef struct ENGINE_DLL tagVertex_Position_Texcoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexcoord;

		static const unsigned int					iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC		VertexElements[iNumElements];
	} VTXPOSTEX;

	typedef struct ENGINE_DLL tagVertex_Effect
	{
		XMFLOAT4		vPosition;
		XMFLOAT2		vTexcoord;

		static const unsigned int iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC	VertexElements[iNumElements];

	}VTXEFFECT;

	typedef struct ENGINE_DLL tagVertex_Point
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vPSize;

		static const unsigned int					iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC		VertexElements[iNumElements];
	}VTXPOINT;

	typedef struct ENGINE_DLL tagVertex_Position_Normal_Texcoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;

		static const unsigned int					iNumElements = { 3 };
		static const D3D11_INPUT_ELEMENT_DESC		VertexElements[iNumElements];
	} VTXNORTEX;

	typedef struct ENGINE_DLL tagVertex_Position_Cube_Texcoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vTexcoord;

		static const unsigned int iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC	VertexElements[iNumElements];
	}VTXCUBETEX;

	typedef struct ENGINE_DLL tagVertex_Mesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;

		static const unsigned int iNumElements = 4;
		static const D3D11_INPUT_ELEMENT_DESC	VertexElements[iNumElements];
	}VTXMESH;

	typedef struct ENGINE_DLL tagVertex_Anim_Mesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;

		XMUINT4			vBlendIndices;
		XMFLOAT4		vBlendWeights;

		static const unsigned int iNumElements = 6;
		static const D3D11_INPUT_ELEMENT_DESC	VertexElements[iNumElements];

	}VTXANIMMESH;

	typedef struct ENGINE_DLL tagVertex_Instance
	{
		XMFLOAT4		vRight;
		XMFLOAT4		vUp;
		XMFLOAT4		vLook;
		XMFLOAT4		vPosition;
	}VTXINSTANCE;

	typedef struct ENGINE_DLL tagVertex_Rect_Instance
	{
		static const unsigned int iNumElements = 6;
		static const D3D11_INPUT_ELEMENT_DESC	VertexElements[iNumElements];
	}VTXRECT_INSTANCE;

	typedef struct ENGINE_DLL tagVertex_Point_Instance
	{
		static const unsigned int iNumElements = 6;
		static const D3D11_INPUT_ELEMENT_DESC	VertexElements[iNumElements];
	}VTXPOINT_INSTANCE;

	typedef struct ENGINE_DLL tagVertex_Mesh_Instance 
	{
		static const unsigned int iNumElements = 8;
		static const D3D11_INPUT_ELEMENT_DESC	VertexElements[iNumElements];
	}VTXMESH_INSTANCE;
}

