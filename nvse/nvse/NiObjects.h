#pragma once

#include "NiNodes.h"
#include "GameData.h"
#include "GameObjects.h"
#include <iterator>


// UNMODIFIED OBSE FILE. I need NiNode class definition.

/***										type
 *	NiAVObject
 *		NiDynamicEffect
 *			NiLight
 *				NiDirectionalLight			01
 *				NiPointLight				02
 *					NiSpotLight				03
 *				NiAmbientLight				00
 *			NiTextureEffect					04
 *		NiNode
 *			SceneGraph
 *			BSTempNodeManager
 *			BSTempNode
 *			BSCellNode
 *			BSClearZNode
 *			BSFadeNode
 *			BSScissorNode
 *			BSTimingNode
 *			BSFaceGenNiNode
 *			NiBillboardNode
 *			NiSwitchNode
 *				NiLODNode
 *					NiBSLODNode
 *			NiSortAdjustNode
 *			NiBSPNode
 *			ShadowSceneNode
 *		NiCamera
 *			BSCubeMapCamera					RTTI data incorrect
 *			NiScreenSpaceCamera
 *		NiGeometry
 *			NiLines
 *			NiTriBasedGeom
 *				NiTriShape
 *					BSScissorTriShape
 *					NiScreenElements
 *					NiScreenGeometry
 *					TallGrassTriShape
 *				NiTriStrips
 *					TallGrassTriStrips
 *			NiParticles
 *				NiParticleSystem
 *					NiMeshParticleSystem
 *				NiParticleMeshes
 */

class NiCamera;
class bhkNiCollisionObject;
class NiLODData;
class ShadowSceneLight;	// 220, derives from NiRefObject
class BSImageSpaceShader;
class NiScreenPolygon;
class NiScreenTexture;
class NiPSysModifier;
class NiRenderer;
class NiGeometryData;
// 0C


struct NiUpdateData
{
	float		timePassed;			// 00
	bool		updateControllers;	// 04
	bool		isMultiThreaded;	// 05
	UInt8		byte06;				// 06
	bool		updateGeomorphs;	// 07
	bool		updateShadowScene;	// 08
	UInt8		pad09;				// 09
	UInt8		pad0A;				// 0A
	UInt8		pad0B;				// 0B

	NiUpdateData() {ZeroMemory(this, sizeof(NiUpdateData));}
	
};


// 09C
class NiAVObject : public NiObjectNET
{
public:
	NiAVObject();
	~NiAVObject();

	/*8C*/virtual void		UpdateControllers(const NiUpdateData& updParams);
	/*90*/virtual void		ApplyTransform(NiMatrix33* arg1, NiVector3* arg2, bool arg3);
	/*94*/virtual void		Unk_25(UInt32 arg);
	/*98*/virtual NiAVObject* GetObject_(const NiFixedString& arName);
	/*9C*/virtual NiAVObject* GetObjectByName(NiFixedString* objName);	//NiFixedString
	/*A0*/virtual void		SetSelectiveUpdateFlags(UInt8* bSelectiveUpdate, UInt32 bSelectiveUpdateTransform, UInt8* bRigid);
	/*A4*/virtual void		UpdateDownwardPass(const NiUpdateData& updParams, UInt32 flags);
	/*A8*/virtual void		UpdateSelectedDownwardPass(const NiUpdateData& updParams, UInt32 flags);
	/*AC*/virtual void		UpdateRigidDownwardPass(const NiUpdateData& updParams, UInt32 flags);
	/*B0*/virtual void		Unk_2C(/*const GeometryProperties& properties*/);
	/*B4*/virtual void		Unk_2D(UInt32 arg);
	/*B8*/virtual void		UpdateWorldData(const NiUpdateData& updParams);
	/*BC*/virtual void		UpdateWorldBound();
	/*C0*/virtual void		UpdateTransformAndBounds(const NiUpdateData& updParams);
	/*C4*/virtual void		PreAttachUpdate(NiNode* newParent, const NiUpdateData& updParams);
	/*C8*/virtual void		PreAttachUpdateProperties(NiNode* newParent);
	/*CC*/virtual void		DetachParent(UInt32 arg);
	/*D0*/virtual void		UpdateUpwardPassParent();
	/*D4*/virtual void		OnVisible(NiCullingProcess* culling);
	/*D8*/virtual void		PurgeRendererData(void* renderer);	//NiDX9Renderer

	enum
	{
		kFlag_AppCulled =					1 << 0,	// originally named m_bAppCulled but they appear to have used bitfields
		kFlag_SelUpdate =					1 << 1,
		kFlag_SelUpdateTransforms =			1 << 2,
		kFlag_SelUpdatePropControllers =	1 << 3,
		kFlag_SelUpdateRigid =				1 << 4,
	};

	NiNode* m_parent;								// 18
	bhkNiCollisionObject* m_collisionObject;		// 1C
	NiBound* m_kWorldBound;							// 20
	DList<NiProperty>		m_propertyList;			// 24
	UInt32					m_flags;				// 30
	NiTransform				m_transformLocal;		// 34
	NiTransform				m_transformWorld;		// 68
	inline NiVector3& LocalTranslate() { return m_transformLocal.translate; }
	inline NiVector3& WorldTranslate() { return m_transformWorld.translate; }
	inline NiMatrix33& WorldRotate() { return m_transformWorld.rotate; }
	inline NiMatrix33& LocalRotate() { return m_transformLocal.rotate; }
	void Dump(UInt32 level, const char * indent);
};
STATIC_ASSERT(sizeof(NiAVObject) == 0x9C);


// 0C
class NiCollisionObject : public NiObject
{
public:
	/*8C*/virtual void		Attach(NiAVObject* obj);
	/*90*/virtual void		Unk_24(UInt32 arg);
	/*94*/virtual void		Unk_25(void);
	/*98*/virtual void		Unk_26(UInt32 arg);
	/*9C*/virtual void		Unk_27(UInt32 version, UInt32 arg1);

	NiNode* linkedNode;	// 08
};



#if 0

// DC+
class NiDynamicEffect : public NiAVObject
{
public:
	NiDynamicEffect();
	~NiDynamicEffect();

	virtual UInt32	GetEffectType(void);

	UInt8	enable;		// 0AC
	UInt8	pad0AD[3];	// 0AD
	UInt32	unk0B0;		// 0B0 - init'd to monotonically increasing number (uid?)
	UInt32	unk0B4;		// 0B4
	UInt32	unk0B8;		// 0B8 - init'd to 1

	NiTPointerList <NiNode *>	affectedNodes;		// 0BC
	NiTPointerList <NiNode *>	unaffectedNodes;	// 0CC
};

// 108
class NiLight : public NiDynamicEffect
{
public:
	NiLight();
	~NiLight();

	float	m_fDimmer;	// 0DC
	NiColor	m_kAmb;		// 0E0
	NiColor	m_kDiff;	// 0EC
	NiColor	m_kSpec;	// 0F8
	UInt32	unk104;		// 104
};

// 114
class NiDirectionalLight : public NiLight
{
public:
	NiDirectionalLight();
	~NiDirectionalLight();

	// UpdateTransform ticks unk0B8 and copies m_direction

	enum { kType = 1 };

	NiVector3	m_direction;	// 108
};

// 114
class NiPointLight : public NiLight
{
public:
	NiPointLight();
	~NiPointLight();

	// UpdateTransform ticks unk0B8

	enum { kType = 2 };

	float	m_fAtten0;	// 108
	float	m_fAtten1;	// 10C
	float	m_fAtten2;	// 110
};

// 128
class NiSpotLight : public NiPointLight
{
public:
	NiSpotLight();
	~NiSpotLight();

	// UpdateTransform ticks unk0B8 and copies m_kWorldDir

	enum { kType = 3 };

	NiVector3	m_kWorldDir;		// 114
	float		m_fSpotAngle;		// 120
	float		m_fSpotExponent;	// 124
};

// 108
class NiAmbientLight : public NiLight
{
public:
	NiAmbientLight();
	~NiAmbientLight();

	enum { kType = 0 };
};

// 174
class NiTextureEffect : public NiDynamicEffect
{
public:
	NiTextureEffect();
	~NiTextureEffect();

	enum { kType = 4 };

	enum
	{
		kTextureMode_ProjectedLight = 0,
		kTextureMode_ProjectedShadow,
		kTextureMode_EnvironmentMap,
		kTextureMode_FogMap,
	};

	enum
	{
		kCoordMode_WorldParallel = 0,
		kCoordMode_WorldPerspective,
		kCoordMode_SphereMap,
		kCoordMode_SpecularCubeMap,
		kCoordMode_DiffuseCubeMap,
	};

	NiMatrix33	m_kModelProjMat;	// 0DC
	NiVector3	m_kModelProjTrans;	// 100
	NiMatrix33	m_kWorldProjMat;	// 10C
	NiVector3	m_kWorldProjTrans;	// 130
	NiObject	* m_texture;		// 13C
	UInt32		m_eFilter;			// 140 - see NiTexturingProperty::kFilterMode_* without the shift
	UInt32		m_eClamp;			// 144 - see NiTexturingProperty::kClampMode_* without the shift
	UInt32		m_eTextureMode;		// 148
	UInt32		m_eCoordMode;		// 14C
	UInt8		m_bPlaneEnable;		// 150
	UInt8		pad151[3];			// 151
	NiPlane		m_kModelPlane;		// 154
	NiPlane		m_kWorldPlane;		// 164
};

#endif

// AC
class NiNode : public NiAVObject
{
public:
	NiNode();
	~NiNode();

	/*DC*/virtual void		AddObject(NiAVObject* object, bool firstFree);
	/*E0*/virtual void		AddObjectAt(UInt32 index, NiAVObject* object);
	/*E4*/virtual void		RemoveObject2(NiAVObject* toRemove, NiAVObject** arg2);
	/*E8*/virtual void		RemoveObject(NiAVObject* toRemove);		//	Calls RemoveObject2 with arg2 as ptr to NULL
	/*EC*/virtual void		RemoveNthObject2(UInt32 index, NiAVObject** arg2);
	/*F0*/virtual void		RemoveNthObject(UInt32 index);			//	Calls RemoveNthObject2 with arg2 as ptr to NULL
	/*F4*/virtual void		ReplaceNthObject2(UInt32 index, NiAVObject* replaceWith, NiAVObject** arg3);
	/*F8*/virtual void		ReplaceNthObject(UInt32 index, NiAVObject* replaceWith);	//	Calls ReplaceNthObject2 with arg3 as ptr to NULL
	/*FC*/virtual void		UpdateUpwardPass();

	NiTArray <NiAVObject *>		m_children;	// 09C

	NiAVObject* GetBlock(const char* blockName);
	NiNode* GetNode(const char* nodeName);
	void __fastcall ToggleCollision(UInt8 flag);
	void ResetCollision();

};	// 0AC
STATIC_ASSERT(sizeof(NiNode) == 0xAC);

class BSFadeNode : public NiNode
{
public:
	enum FadeType
	{
		kFade_Object = 1,
		kFade_Item = 2,
		kFade_Actor = 3,
		kFade_Unknown6 = 6,
		kFade_Unknown7 = 7,
		kFade_Unknown8 = 8,
		kFade_LODFadeOutMax = 0xA,
	};

	float			nearDistSqr;	// AC
	float			farDistSqr;		// B0
	float			lastFade;		// B4
	float			currentFade;	// B8	[0.0, 1.0]; Used for fade-in/out
	float			boundRadius;	// BC
	float			timeSinceUpdate;// C0
	UInt32			fadeType;		// C4
	UInt32			frameCounter;	// C8
	TESObjectREFR* linkedObj;		// CC
	UInt32			unkD0[5];		// D0

	__forceinline static BSFadeNode* Create() { return ThisStdCall<BSFadeNode*>(0xB4E150, CdeclCall<void*>(0xAA13E0, sizeof(BSFadeNode))); }

	void __fastcall SetVisible(bool visible);
};
static_assert(sizeof(BSFadeNode) == 0xE4);

class NiCamera : public NiAVObject
{
public:
	float			worldToCam[4][4];	// 09C
	NiFrustum		frustum;			// 0DC
	float			minNearPlaneDist;	// 0F8
	float			maxFarNearRatio;	// 0FC
	NiViewport		viewPort;			// 100
	float			LODAdjust;			// 110

	__forceinline static NiCamera* Create() { return CdeclCall<NiCamera*>(0xA71430); }

	//bool __fastcall WorldToScreen(const NiVector3& worldPos, NiPoint2& scrPos);
};
static_assert(sizeof(NiCamera) == 0x114);

class BSSceneGraph : public NiNode {
public:
	BSSceneGraph();
	~BSSceneGraph();

	/*100*/virtual double	CalculateNoLODFarDist();
	/*104*/virtual void		SetViewDistanceBasedOnFrameRate(float afTime);
};
STATIC_ASSERT(sizeof(BSSceneGraph) == 0xAC);

//C0
class SceneGraph : public BSSceneGraph {
public:
	SceneGraph();
	~SceneGraph();

	NiCamera*			camera;			// AC
	void*				pVisArray;			// B0
	NiCullingProcess*	cullingProc;		// B4
	UInt32				isMinFarPlaneDist;	// B8 The farplane is set to 20480.0 when the flag is true. Probably used for interiors.
	float				cameraFOV;			// BC
};
STATIC_ASSERT(sizeof(SceneGraph) == 0xC0);


#if 0

// F0
//class SceneGraph : public NiNode
//{
//public:
//	SceneGraph();
//	~SceneGraph();
//
//	NiCamera			* camera;			// 0DC
//	UInt32				unk0E0;				// 0E0
//	NiCullingProcess	* cullingProcess;	// 0E4
//	UInt32				unk0E8;				// 0E8
//	float				cameraFOV;			// 0EC
//};
//
//STATIC_ASSERT(sizeof(SceneGraph) == 0x0F0);

// E0
class BSTempNodeManager : public NiNode
{
public:
	BSTempNodeManager();
	~BSTempNodeManager();

	// overrides Unk_18 and related to scan children for BSTempNodes
	// removes them? tests against unk0DC

	float	unk0DC;	// 0DC
};

// E0
class BSTempNode : public NiNode
{
public:
	BSTempNode();
	~BSTempNode();

	float	unk0DC;	// 0DC
};

// BSCellNode - in RTTI but unref'd

// E8
class BSClearZNode : public NiNode
{
public:
	BSClearZNode();
	~BSClearZNode();

	// in Draw, scan for BSShaderAccumulator on the renderer, set 21E0 to 0 before calling parent Draw, reset to 1 afterwards

	UInt8	unk0DC;	// 0DC
	UInt8	unk0DD;	// 0DD
	float	unk0E0;	// 0E0 - init'd to 283840
	float	unk0E4;	// 0E4 - init'd to 100
};



// EC
class BSScissorNode : public NiNode
{
public:
	BSScissorNode();
	~BSScissorNode();

	// mess about with renderer in Draw (presumably to set scissor mode)

	UInt32	unk0DC;	// 0DC
	UInt32	unk0E0;	// 0E0
	UInt32	unk0E4;	// 0E4
	UInt32	unk0E8;	// 0E8
};

// BSTimingNode - unref'd

class Actor;

// 118
class BSFaceGenNiNode : public NiNode
{
public:
	BSFaceGenNiNode();
	~BSFaceGenNiNode();

	virtual NiObject *	GetAnimationData(void);
	virtual void		SetAnimationData(NiObject * obj);
	virtual float *		GetRotation(float * dst);
	virtual void		SetRotation(float * src);	// ticks bRotatedLastUpdate
	virtual bool		GetAnimationUpdate(void);
	virtual void		SetAnimationUpdate(bool arg);
	virtual bool		GetApplyRotationToParent(void);
	virtual void		SetApplyRotationToParent(bool arg);
	virtual bool		GetFixedNormals(void);
	virtual void		SetFixedNormals(bool arg);
	virtual void		Unk_31(void * arg);	// scans children, "could not find bone for the head node"

	// spAnimationData->Unk15(idx) returns emotion weighting (max 0x0C)
	// spAnimationData->Unk17(idx) returns movement weighting (max 0x10)
	// spAnimationData->Unk1A(idx) returns phoneme weighting (max 0x0F)

	NiObject	* spAnimationData;		// 0DC
	float		m_rotation[9];			// 0E0 - init'd to identity mtx
	UInt8		bForceBaseMorph;		// 104
	UInt8		bFixedNormals;			// 105
	UInt8		bAnimationUpdate;		// 106
	UInt8		bRotatedLastUpdate;		// 107
	UInt8		bApplyRotationToParent;	// 108
	UInt8		pad109[3];				// 109
	float		fLastTime;				// 10C - init'd to -1
	UInt8		bUsingLoResHead;		// 110
	UInt8		bIAmPlayerCharacter;	// 111
	UInt8		bIAmInDialouge;			// 112
	UInt8		pad113;					// 113
	Actor		* pActor;				// 114
};

// E4
class NiBillboardNode : public NiNode
{
public:
	NiBillboardNode();
	~NiBillboardNode();

	enum
	{
		kMode_AlwaysFaceCamera =	0,
		kMode_RotateAboutUp,
		kMode_RigidFaceCamera,
		kMode_AlwaysFaceCenter,
		kMode_RigidFaceCenter,
		kMode_BSRotateAboutUp,
		kMode_Mask =				7,

		kFlag_Unk3 =				1 << 3,
	};

	// update functions set kFlag_Unk3

	UInt16	flags;				// 0DC
	UInt8	pad0DE[2];			// 0DE
	float	m_lastUpdateTime;	// 0E0
};

// FC
class NiSwitchNode : public NiNode
{
public:
	NiSwitchNode();
	~NiSwitchNode();

	// only updates/displays/etc child with index m_iIndex
	// keeps track of the last time each child was updated to save work

	enum
	{
		kUpdateOnlyActive =	1 << 0,
	};

	UInt16				flags;			// 0DC
	UInt8				pad0DE[2];		// 0DE
	UInt32				m_iIndex;		// 0E0 - active child idx
	float				lastUpdateTime;	// 0E4
	UInt32				curUpdateIdx;	// 0E8
	NiTArray <UInt32>	childUpdateIdx;	// 0EC - last update idx for each child
};

// 104
class NiLODNode : public NiSwitchNode
{
public:
	NiLODNode();
	~NiLODNode();

	NiLODData	* lodData;	// 0FC
	UInt8		unk100;		// 100
	UInt8		pad101[3];	// 101
};

// NiBSLODNode - unref'd

// E0
class NiSortAdjustNode : public NiNode
{
public:
	NiSortAdjustNode();
	~NiSortAdjustNode();

	UInt32	unk0DC;	// 0DC
};

// FC
class NiBSPNode : public NiNode
{
public:
	NiBSPNode();
	~NiBSPNode();

	NiPlane	m_kModelPlane;	// 0DC
	NiPlane	m_kWorldPlane;	// 0EC
};

// 130
class ShadowSceneNode : public NiNode
{
public:
	ShadowSceneNode();
	~ShadowSceneNode();

	NiObject			* unk0DC;	// 0DC
	UInt16				unk0E0;		// 0E0
	UInt8				pad0E2[2];	// 0E2
	NiTPointerList <ShadowSceneLight *>	lights;		// 0E4
	NiTPointerList <ShadowSceneLight *>	lights2;	// 0F4
	UInt32				pad104;		// 104
	UInt32				unk108;		// 108
	UInt32				unk10C;		// 10C
	ShadowSceneLight	* unk110;	// 110
	ShadowSceneLight	* unk114;	// 114
	UInt32				unk118;		// 118
	UInt8				unk11C;		// 11C
	UInt8				pad11D[3];	// 11D
	NiObject			* unk120;	// 120
	UInt32				unk124;		// 124
	UInt32				unk128;		// 128
	UInt8				unk12C;		// 12C
	UInt8				pad12D[3];	// 12D
};

// 124
class NiCamera : public NiAVObject
{
public:
	NiCamera();
	~NiCamera();

	UInt32		unk0AC[(0xEC - 0xAC) >> 2];	// 0AC
	NiFrustum	m_kViewFrustum;				// 0EC
	float		m_fMinNearPlaneDist;		// 108
	float		m_fMaxFarNearRatio;			// 10C
	NiViewport	m_kPort;					// 110
	float		unk120;						// 120
};

// 150
class BSCubeMapCamera : public NiCamera
{
public:
	BSCubeMapCamera();
	~BSCubeMapCamera();

	// 1C
	class Str14C
	{
	public:
		Str14C();
		~Str14C();

		NiTPointerList <BSImageSpaceShader *>	shaders;	// 00
		NiObject	* unk010;	// 10
		NiObject	* unk014;	// 14
		UInt32		unk018;		// 18
	};

	UInt32		unk124;			// 124
	NiObject	* unk128[6];	// 128
	NiObject	* unk140;		// 140
	UInt32		pad144;			// 144
	NiObject	* unk148;		// 148
	Str14C		* unk14C;		// 14C
};

// 144
class NiScreenSpaceCamera : public NiCamera
{
public:
	NiScreenSpaceCamera();
	~NiScreenSpaceCamera();

	NiTArray <NiScreenPolygon *>	polygons;	// 124
	NiTArray <NiScreenTexture *>	textures;	// 134
};

// C0
class NiGeometry : public NiAVObject
{
public:
	NiGeometry();
	~NiGeometry();

	virtual void	Render(NiRenderer * arg);
	virtual void	Unk_22(NiRenderer * arg);
	virtual void	SetGeomData(NiObject * obj);
	virtual void	Unk_24(void);		// geomData->Unk_15()
	virtual void	Unk_25(NiRenderer * arg);

	NiPropertyState			* unk0AC;	// 0AC
	NiDynamicEffectState	* unk0B0;	// 0B0
	NiGeometryData			* geomData;	// 0B4
	NiObject				* skinData;	// 0B8
	NiObject				* shader;	// 0BC
};

// C0
class NiLines : public NiGeometry
{
public:
	NiLines();
	~NiLines();
};

// C0
class NiTriBasedGeom : public NiGeometry
{
public:
	NiTriBasedGeom();
	~NiTriBasedGeom();

	// use this API for SUPER MAX SPEED
	virtual void	GetTri(UInt32 idx, NiVector3 ** a, NiVector3 ** b, NiVector3 ** c) = 0;
};

// C0
class NiTriShape : public NiTriBasedGeom
{
public:
	NiTriShape();
	~NiTriShape();
};

// D0
class BSScissorTriShape : public NiTriShape
{
public:
	BSScissorTriShape();
	~BSScissorTriShape();

	UInt32	unk0C0;	// 0C0
	UInt32	unk0C4;	// 0C4
	UInt32	unk0C8;	// 0C8
	UInt32	unk0CC;	// 0CC
};

// C0
class NiScreenElements : public NiTriShape
{
public:
	NiScreenElements();
	~NiScreenElements();
};

// C0
class NiScreenGeometry : public NiTriShape
{
public:
	NiScreenGeometry();
	~NiScreenGeometry();
};

// D4
class TallGrassTriShape : public NiTriShape
{
public:
	TallGrassTriShape();
	~TallGrassTriShape();

	UInt16		instanceCount;	// 0C0
	UInt8		pad0C2[2];		// 0C2
	NiSphere	pLocalBound;	// 0C4
};

// C0
class NiTriStrips : public NiTriBasedGeom
{
public:
	NiTriStrips();
	~NiTriStrips();
};

// D4
class TallGrassTriStrips : public NiTriStrips
{
public:
	TallGrassTriStrips();
	~TallGrassTriStrips();

	virtual void		SetLocalBound(float x, float y, float z, float radius);
	virtual void		SetInstanceCount(UInt16 count);
	virtual NiSphere *	GetLocalBound(void);

	UInt16		instanceCount;	// 0C0
	UInt8		pad0C2[2];		// 0C2
	NiSphere	pLocalBound;	// 0C4
};

// C0
class NiParticles : public NiGeometry
{
public:
	NiParticles();
	~NiParticles();
};

// F0
class NiParticleSystem : public NiParticles
{
public:
	NiParticleSystem();
	~NiParticleSystem();

	virtual void	Unk_26(float arg);
	virtual void	Update(float time);

	UInt8	inWorldSpace;		// 0C0
	UInt8	pad0C1[3];			// 0C1
	NiTPointerList <NiPSysModifier *>	modifiers;		// 0C4
	NiTPointerMap <NiPSysModifier *>	modifierMap;	// 0D4 - wrong, should be NiTStringPointerMap <NiPSysModifier *>
	UInt8	unk0E4;				// 0E4
	UInt8	pad0E5[3];			// 0E5
	float	systemUpdateTime;	// 0E8
	UInt8	unk0EC;				// 0EC
	UInt8	pad0ED[3];			// 0ED
};

// F8
class NiMeshParticleSystem : public NiParticleSystem
{
public:
	NiMeshParticleSystem();
	~NiMeshParticleSystem();

	float	transformUpdateTime;	// 0F0
	UInt8	unk0F4;					// 0F4
	UInt8	unk0F5[3];				// 0F5
};

// C8
class NiParticleMeshes : public NiParticles
{
public:
	NiParticleMeshes();
	~NiParticleMeshes();

	float	transformUpdateTime;	// 0C0
	UInt8	unk0C4;					// 0C4
	UInt32	unk0C5[3];				// 0C5
};

#endif
