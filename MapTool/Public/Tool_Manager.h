#pragma once

#include "Tool_Defines.h"
#include "Base.h"
#include "Edit_Spawner.h"

BEGIN(Engine)
class CGameInstance;
class CTexture;
class CGameObject;
class CVIBuffer_Terrain;
END

BEGIN(Tool)

class CTool_Manager final : public CBase
{
public:
	enum TERRAIN_TEXTURE { TERRAIN_DIFFUSE, TERRAIN_NORMAL, TERRAIN_MASK, TERRAIN_END };

private:
	CTool_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTool_Manager() = default;

public:
	HRESULT Initialize(HWND hWnd);
	void Tick(_float fTimeDelta);
	HRESULT Render_Begin();
	HRESULT Render_End();

private:
	void Draw_Common();
	void Draw_WorkSpace();
	void Draw_Gizmo();
	void Draw_Grid();

private:
	void Show_Inspector();
	void Show_TerrainEditor();
	void Show_InstancingEditor();
	void Show_WaveEditor();
	void Show_DungeonEditor();
	void Show_NavigationEditor();

private:
	_float2 Compute_ScreenPoint();
	HRESULT Create_GameObject();
	HRESULT Load_GameObjects();
	HRESULT Save_GameObjects();
	void Move_AllObjects(_float3 vDir);
	HRESULT Loading_Path(const wstring& strDataPath, vector<wstring>* pPath);
	HRESULT Compute_DepthPicking(_float2 vScreenPoint, _float3* pPickPos);
	HRESULT Create_Staging();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };
	HWND m_hWnd;

private: /* 데이터 파일 */
	vector<wstring> m_ProtoTypeTag;

private: /* 공통 데이터 변수 */
	wstring m_LayerTag_GameObjects = TEXT("Layer_GameObjects");
	wstring m_LayerTag_Terrain = TEXT("Layer_Terrain");
	wstring m_LayerTag_WaterInWorld = TEXT("Layer_WaterInWorld");
	wstring m_LayerTag_WaterInTemple = TEXT("Layer_WaterInTemple");
	wstring m_LayerTag_WaterInStrongHold = TEXT("Layer_WaterInStrongHold");
	wstring m_LayerTag_WaterInQuest = TEXT("Layer_WaterInQuest");
	wstring m_LayerTag_WaterInDvalin = TEXT("Layer_WaterInDvalin");
	wstring m_LayerTag_Temple = TEXT("Layer_Temple");
	wstring m_LayerTag_StrongHold = TEXT("Layer_StrongHold");
	wstring m_LayerTag_Quest = TEXT("Layer_Quest");
	wstring m_LayerTag_Dvalin = TEXT("Layer_Dvalin");
	wstring m_LayerTag_WorldSpawner = TEXT("Layer_WorldSpawner");
	wstring m_LayerTag_TempleSpawner = TEXT("Layer_TempleSpawner");
	wstring m_LayerTag_StrongHoldSpawner = TEXT("Layer_StrongHoldSpawner");
	wstring m_LayerTag_QuestSpawner = TEXT("Layer_QuestSpawner");
	wstring m_LayerTag_DvalinSpawner = TEXT("Layer_DvalinSpawner");

private:/* Common */
	ID3D11Texture2D* m_pDepthCopyTexture = { nullptr };
	_float m_fScreenPoint[2] = {};
	_float m_fPickingPoint[3] = {};

	_bool m_bWorldLevel = false;
	_bool m_bTempleLevel = false;
	_bool m_bStrongHoldLevel = false;
	_bool m_bQuestLevel = false;
	_bool m_bDvalinLevel = false;

private: /* 속성 에디터 */
	const _char** m_ppItem_Model;
	const _char* m_CurrentObjectItem;
	_uint m_iCurrentObjectIndex = 0;

	const _char* m_pItemType[2] =
	{
		"None",
		"Interaction"
	};
	const _char* m_pCurrentItemType = m_pItemType[0];
	_uint m_iCurrentItemTypeIndex = 0;

	_bool m_bDelete = false;

private: /* 터레인 에디터 */
	_bool m_bBrushMode = false;
	_int m_iTerrainScale[2] = {};
	_float m_fBrushForce = 0.05f;
	_float m_fBrushSize = 10.0f;
	_float m_fStrength = 1.0f;
	_float m_fMaxHeight = 10.0f;
	_bool m_bPainting = false;
	_bool m_bMasking = false;

	_bool m_bGrassMask = false;
	_bool m_bRoadMask = false;
	_bool m_bStoneMask = false;

private: /* 인스턴싱 에디터 */
	const _char* m_pInstancingType[25] =
	{
		"Lush Grass",
		"Normal Grass",
		"Silver Grass",
		"Pink Three Leaf Grass",
		"Blue One Leaf Grass",
		"Pink Long Grass",
		"Blue Flower Grass",
		"Toran Grass",
		//"Lush Leaf Grass",
		"Yellow Flower1",
		"Yellow Blue Red Flower",
		"Yellow Flower2",
		"Red Flower",
		"Low Yellow Blue Red Flower",
		"Yellow Blue Flower",
		"Blue Red Flower",
		"Yellow Blue White Flower",
		"Blue Flower1",
		"Puple Flower",
		"Red Napal Flower",
		"Orange Flower1",
		"Orange Flower2",
		"Pink Flower",
		"Blue Flower2",
		"SunFlower",
		"Yellow Flower3"
	};
	const _char* m_pCurrentInstanceType = m_pInstancingType[0];
	_uint m_iCurrentInstanceTypeIndex = 0;
	_bool m_bRandomRotationY = false;
	_bool m_bInstancing = false;
	_bool m_bInstancingBrush = false;
	_bool m_bAll = false;
	_float m_fInstanceTerm = 1.0f;

private: /* 물 에디터 */
	_int m_iWaterSize[2] = {};
	_float m_fGridSpacing = 1.0f;
	_float m_fWaterSpawnPos[3] = {};
	_float m_fWaterUVScale[3] = {};
	_float m_fWaterScale = 0.0f;
	_float m_fNoise[3] = {};
	_float m_fWaterSpeed[2] = {};

private: /* 던전 에디터 */
	_float m_fSpawnerPosition[3] = {};
	_float m_fSpawnerRange = 0.0f;

	const _char* m_pMonsterType[15] =
	{
		"Trveler -> X",
		"BARBARA -> X",
		"DILUC -> X",
		"GANYU -> X",
		"ZHONGLI -> X",
		"PAIMON -> X",
		"KATHERYNE -> X",
		"CHUCHU -> X",
		"ANDRIUS -> X",
		"SLIME_WATER",
		"SLIME_ICE",
		"HILI_NORMAL",
		"MONSTER_BRUTE_SHIELD -> X",
		"MONSTER_BRUTE_AXE",
		"MONSTER_EFFIGY_FIRE -> X"
	};
	const _char* m_pCurrentMonsterType = m_pMonsterType[0];
	_uint m_iCurrentMonsterTypeIndex = 0;

	_float m_fMonsterSpawnPos[3] = {};
	_int m_iSpawnerIndex = -1;
	CEdit_Spawner::MONSTER_INFO* m_pSelectMonsterInfo = { nullptr };
	_int m_iMonsterIndex = -1;

private: /* 네비 메시 에디터*/
	_float3 m_vPoints[3] = {};
	_int m_iPointIndex = 0;

private: /* 기즈모 */
	CGameObject* m_pSelectObject = { nullptr };
	_bool m_bIsUsingGizmo = false;
	_int m_iGizmoMode = 1;
	_int m_iOperationMode = 7;
	_bool m_bActiveSnap = true;
	_float m_fSnapValue[3] = { 1.0f, 1.0f, 1.0f };


public:
	static CTool_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd);
	virtual void Free() override;
};

END
