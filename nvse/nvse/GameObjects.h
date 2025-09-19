#pragma once

#include "GameForms.h"
#include "GameBSExtraData.h"
#include "GameExtraData.h"
#include "GameProcess.h"
#include "NiPoint.h"


class TESObjectCELL;
struct ScriptEventList;
class ActiveEffect;
class NiNode;
class Animation;
class MagicCaster;

typedef ExtraContainerChanges::EntryData ContChangesEntry;
typedef ExtraContainerChanges::EntryDataList ContChangesEntryList;
typedef ExtraContainerChanges::ExtendDataList ContChangesExtraList;
struct BipedAnim
{
	enum eOptionalBoneType
	{
		kOptionalBone_Bip01Head = 0,
		kOptionalBone_Weapon = 1,
		kOptionalBone_Bip01LForeTwist = 2,
		kOptionalBone_Bip01Spine2 = 3,
		kOptionalBone_Bip01Neck1 = 4,
	};

	enum SlotIDs
	{
		kSlot_Head = 0,
		kSlot_Hair,
		kSlot_UpperBody,
		kSlot_LeftHand,
		kSlot_RightHand,
		kSlot_Weapon,
		kSlot_PipBoy,
		kSlot_Backpack,
		kSlot_Necklace,
		kSlot_Headband,
		kSlot_Hat,
		kSlot_Eyeglasses,
		kSlot_Nosering,
		kSlot_Earrings,
		kSlot_Mask,
		kSlot_Choker,
		kSlot_MouthObject,
		kSlot_BodyAddon1,
		kSlot_BodyAddon2,
		kSlot_BodyAddon3
	};

	// 08
	struct OptionalBone
	{
		bool	exists;
		NiNode* bone;
	};

	// 10
	struct Data
	{
		union									// 00 can be a modelled form (Armor or Weapon) or a Race if not equipped
		{
			TESForm* item;
			TESObjectARMO* armor;
			TESObjectWEAP* weapon;
			TESRace* race;
		};
		TESModelTextureSwap* modelTexture;	// 04 texture or model for said form
		NiAVObject* object;		// 08 NiNode for the modelled form
		UInt8					hasSkinnedGeom;	// 0C
		UInt8					pad0D[3];		// 0D
	};

	NiNode* bip01;			// 000 receive Bip01 node, then optionally Bip01Head, Weapon, Bip01LForeTwist, Bip01Spine2, Bip01Neck1
	OptionalBone		bones[5];		// 004
	Data				slotData[20];	// 02C indexed by the EquipSlot
	Data				unk016C[20];	// 16C indexed by the EquipSlot
	UInt32				unk2AC;			// 2AC
	Character* character;		// 2B0
};
static_assert(sizeof(BipedAnim) == 0x2B4);


// 008
class TESChildCell
{
public:
	TESChildCell();
	~TESChildCell();

	// no virtual destructor
	virtual TESObjectCELL *	GetPersistentCell(void);		// 000

//	void	** vtbl;	// 00
};



// 68
class TESObjectREFR : public TESForm
{
public:
	MEMBER_FN_PREFIX(TESObjectREFR);

	TESObjectREFR();
	~TESObjectREFR();

	/*138*/virtual bool		GetStartingPosition(NiVector3* outPos, NiVector3* outRot, TESForm** outWrldOrCell, TESForm* defaultWrldOrCell);
	/*13C*/virtual void		SayTopic(Sound* sound, TESTopic* topic, TESObjectREFR* target, bool dontUseNiNode, bool notVoice, bool useLipFile, UInt8 unused, bool subtitles);
	/*140*/virtual void		Unk_50(void);
	/*144*/virtual void		DamageObject(float damage, bool allowDestroyed);
	/*148*/virtual bool		GetCastsShadows();
	/*14C*/virtual void		SetCastsShadows(bool doSet);
	/*150*/virtual void		Unk_54(void);
	/*154*/virtual void		Unk_55(void);
	/*158*/virtual void		Unk_56(void);
	/*15C*/virtual bool		IsObstacle();
	/*160*/virtual bool		BaseIsQuestItem();
	/*164*/virtual void		Unk_59(void);
	/*168*/virtual void		Unk_5A(void);
	/*16C*/virtual void		Unk_5B(void);
	/*170*/virtual void		Unk_5C(void);
	/*174*/virtual void		Unk_5D(void);
	/*178*/virtual void		RefreshDynamicLight();
	/*17C*/virtual TESObjectREFR* RemoveItem(TESForm* toRemove, BaseExtraList* extraList, UInt32 quantity, bool keepOwner, bool drop, TESObjectREFR* destRef, UInt32 unk6, UInt32 unk7, bool unk8, bool unk9);
	/*180*/virtual void		Unk_60(void);
	/*184*/virtual bool		LoadEquipedItem3D(TESForm* item, UInt32 count, ExtraDataList* xData, bool lockEquip);
	/*188*/virtual void		Unk_62(void);
	/*18C*/virtual void		Unk_63(void);
	/*190*/virtual void		AddItem(TESForm* item, ExtraDataList* xDataList, UInt32 quantity);
	/*194*/virtual void		Unk_65(void);
	/*198*/virtual MagicCaster* GetMagicCaster();
	/*19C*/virtual MagicTarget* GetMagicTarget();
	/*1A0*/virtual bool		GetIsChildSize(bool checkHeight);		// 068 Actor: GetIsChildSize
	/*1A4*/virtual UInt32	GetActorUnk0148();			// result can be interchanged with baseForm, so TESForm* ?
	/*1A8*/virtual void		SetActorUnk0148(UInt32 arg1);
	/*1AC*/virtual BSFaceGenNiNode* GetFaceGenNodeBiped(UInt32 arg1);
	/*1B0*/virtual BSFaceGenNiNode* GetFaceGenNodeSkinned(UInt32 arg1);
	/*1B4*/virtual BSFaceGenNiNode* CallGetFaceGenNodeSkinned(UInt32 arg1);	// REFR: calls 006C
	/*1B8*/virtual BSFaceGenAnimationData* CreateFaceAnimationData(UInt32 arg1);
	/*1BC*/virtual void		Unk_6F(void);
	/*1C0*/virtual bool		Unload3D();
	/*1C4*/virtual void		AnimateNiNode();
	/*1C8*/virtual NiNode* GenerateNiNode(bool arg1);
	/*1CC*/virtual void		Set3D(NiNode* niNode, bool unloadArt);
	/*1D0*/virtual NiNode* GetNiNode_v();
	/*1D4*/virtual void		Unk_75(void);
	/*1D8*/virtual NiVector3* GetMinBounds(NiVector3* outBounds);
	/*1DC*/virtual NiVector3* GetMaxBounds(NiVector3* outBounds);
	/*1E0*/virtual void		  UpdateAnimation();
	/*1E4*/virtual AnimData* GetAnimData();			// 0079
	/*1E8*/virtual BipedAnim* GetBipedAnim();	// 007A	Character only
	/*1EC*/virtual BipedAnim* CallGetBipedAnim();
	/*1F0*/virtual void		SetBipedAnim(BipedAnim* bipedAnims);
	/*1F4*/virtual NiVector3* GetPos();
	/*1F8*/virtual void		Unk_7E(UInt32 arg0);
	/*1FC*/virtual void		Unk_7F(void);
	/*200*/virtual void		Unk_80(UInt32 arg0);
	/*204*/virtual void		Unk_81(UInt32 arg0);
	/*208*/virtual void		Unk_82(void);
	/*20C*/virtual NiNode* GetProjectileNode();
	/*210*/virtual void		SetProjectileNode(NiNode* node);
	/*214*/virtual UInt32	GetSitSleepState();
	/*218*/virtual bool		IsCharacter();			// return false for Actor and Creature, true for character and PlayerCharacter
	/*21C*/virtual bool		IsCreature();
	/*220*/virtual bool		IsExplosion();
	/*224*/virtual bool		IsProjectile() const;
	/*228*/virtual void		SetParentCell(TESObjectCELL* cell);			// SetParentCell (Interior only ?)
	/*22C*/virtual bool		HasHealth(bool arg0);	// HasHealth (baseForm health > 0 or Flags bit23 set)
	/*230*/virtual bool		GetHasKnockedState();
	/*234*/virtual bool		GetIsParalyzed();
	/*238*/virtual void		Unk_8E(void);
	/*23C*/virtual void		Unk_8F(void);
	/*240*/virtual void		MoveToHighProcess();

	enum
	{
		kFlags_Unk00000002 = 0x00000002,
		kFlags_Deleted = 0x00000020,		// refr removed from .esp or savegame
		kFlags_Taken = kFlags_Deleted | kFlags_Unk00000002,
		kFlags_Persistent = 0x00000400,		//shared bit with kFormFlags_QuestItem
		kFlags_Temporary = 0x00004000,
		kFlags_IgnoreFriendlyHits = 0x00100000,
		kFlags_Destroyed = 0x00800000,

		kChanged_Inventory = 0x08000000,
	};

	struct RenderState
	{
		TESObjectREFR* currWaterRef;		// 00
		UInt32			underwaterCount;	// 04	0-0x13 when fully-underwater; exterior only
		float			waterLevel;			// 08
		float			revealDistance;		// 0C
		UInt32			flags;				// 10
		NiNode* rootNode;			// 14
		void* phantom;			// 18	bhkPhantom* Used with trigger volume
	};

	struct EditorData {
		UInt32	unk00;	// 00
	};
	// 0C

#ifdef EDITOR
	EditorData	editorData;			// +04
#endif

	TESChildCell	childCell;				// 018

	UInt32			unk1C;					// 01C

	TESForm			* baseForm;				// 020
	
	NiVector3		rotation;		// 024 - either public or accessed via simple inline accessor common to all child classes
	NiVector3		position;		// 030 - seems to be private
	float			scale;					// 03C 

	TESObjectCELL	* parentCell;			// 040
	ExtraDataList	extraDataList;			// 044
	RenderState		* renderState;			// 064	- (05C in FOSE)

	ExtraScript* GetExtraScript() const;
	ScriptEventList *	GetEventList() const;
	__forceinline NiNode* GetRefNiNode() const { return renderState ? renderState->rootNode : nullptr; }
	bool IsTaken() const { return (flags & kFlags_Taken) != 0; }
	bool IsPersistent() const { return (flags & kFlags_Persistent) != 0; }
	bool IsTemporary() { return (flags & kFlags_Temporary) ? true : false; }
	bool IsDeleted() const { return (flags & kFlags_Deleted) ? true : false; }
	bool GetDisabled() const;
	bool Update3D();
	bool Update3D_v1c();	// Less worse version as used by some modders
	TESContainer* GetContainer();
	bool IsMapMarker();
	bool GetInventoryItems(InventoryItemsMap &invItems);
	ExtraDroppedItemList* GetDroppedItems();
	ExtraContainerChanges::EntryDataList* GetContainerChangesList() const;
	double GetHeadingAngle(const TESObjectREFR* to) const;
	float GetHeadingAngle_f(const TESObjectREFR* to) const;
	
	void __fastcall ToggleCollision(bool toggle);
	BSBound* GetBoundingBox() const;

	bool __fastcall GetInSameCellOrWorld(TESObjectREFR* target) const;
	float __vectorcall GetDistance(TESObjectREFR* target) const;
	static TESObjectREFR* Create(bool bTemp = false);

	MEMBER_FN_PREFIX(TESObjectREFR);
#if RUNTIME
	DEFINE_MEMBER_FN(Activate, bool, 0x00573170, TESObjectREFR*, UInt32, UInt32, UInt32);	// Usage Activate(actionRef, 0, 0, 1); found inside Cmd_Activate_Execute as the last call (190 bytes)
	DEFINE_MEMBER_FN(Set3D, void, 0x0094EB40, NiNode*, bool);	// Usage Set3D(niNode, true); virtual func 0073
#endif
	bhkCharacterController* GetCharacterController() const;
};
float GetHeadingAngleX_f(TESObjectREFR* out,TESObjectREFR* to);

TESForm* GetPermanentBaseForm(TESObjectREFR* thisObj);	// For LevelledForm, find real baseForm, not temporary one.

// Taken from JIP LN NVSE.
float __vectorcall GetDistance3D(const TESObjectREFR* ref1, const TESObjectREFR* ref2);

STATIC_ASSERT(offsetof(TESObjectREFR, baseForm) == 0x020);
STATIC_ASSERT(offsetof(TESObjectREFR, extraDataList) == 0x044);
STATIC_ASSERT(sizeof(TESObjectREFR) == 0x068);

class BaseProcess;

// 088
class MobileObject : public TESObjectREFR
{
public:
	MobileObject();
	~MobileObject();

	virtual void		Unk_91(void);
	virtual void		Unk_92(void);
	virtual void		Unk_93(void);
	virtual void		Unk_94(void);
	virtual void		Unk_95(void);
	virtual void		Unk_96(void);
	virtual void		Unk_97(void);
	virtual void		Unk_98(void);
	virtual void		Unk_99(void);
	virtual void		Unk_9A(void);
	virtual void		Unk_9B(void);
	virtual void		Unk_9C(void);
	virtual void		Unk_9D(void);
	virtual void		Unk_9E(void);
	virtual void		Unk_9F(void);
	virtual void		Unk_A0(void);	// StartConversation(targetActor, subjectLocationData, targetLocationData, headTrack, allowMovement, arg, topicID, arg, arg
	virtual void		Unk_A1(void);
	virtual void		Unk_A2(void);
	virtual void		Unk_A3(void);
	virtual void		Unk_A4(void);
	virtual void		Unk_A5(void);
	virtual void		Unk_A6(void);
	virtual void		Unk_A7(void);
	virtual void		Unk_A8(void);
	virtual void		Unk_A9(void);
	virtual void		Unk_AA(void);
	virtual void		Unk_AB(void);
	virtual void		Unk_AC(void);
	virtual void		Unk_AD(void);
	virtual void		Unk_AE(void);
	virtual float		AdjustRot(UInt32 arg1);
	virtual void		Unk_B0(void);
	virtual void		Unk_B1(void);
	virtual void		Unk_B2(void);
	virtual void		Unk_B3(void);
	virtual void		Unk_B4(void);
	virtual void		Unk_B5(void);
	virtual void		Unk_B6(void);
	virtual void		Unk_B7(void);
	virtual void		Unk_B8(void);
	virtual void		Unk_B9(void);
	virtual void		Unk_BA(void);
	virtual void		Unk_BB(void);
	virtual void		Unk_BC(void);
	virtual void		Unk_BD(void);
	virtual void		Unk_BE(void);
	virtual void		Unk_BF(void);
	virtual void		Unk_C0(void);
	
	BaseProcess	* baseProcess;	// 068
	UInt32		unk06C;			// 06C - loaded	set to the talking actor ref when initialising ExtraTalkingActor
	UInt32		unk070;			// 070 - loaded
	UInt32		unk074;			// 074 - loaded
	UInt32		unk078;			// 078 - loaded
	UInt8		unk07C;			// 07C - loaded
	UInt8		unk07D;			// 07D - loaded
	UInt8		unk07E;			// 07E - loaded
	UInt8		unk07F;			// 07F - loaded
	UInt8		unk080;			// 080 - loaded
	UInt8		unk081;			// 081 - loaded
	UInt8		unk082;			// 082 - cleared when starting combat on player
	UInt8		unk083;			// 083 - loaded
	UInt8		unk084;			// 084 - loaded
	UInt8		unk085;			// 085 - loaded
	UInt8		unk086;			// 086 - loaded
	UInt8		unk087;			// 087	Init'd to the inverse of NoLowLevelProcessing
};
STATIC_ASSERT(offsetof(MobileObject, baseProcess) == 0x068);
STATIC_ASSERT(sizeof(MobileObject) == 0x088);

// 00C
class MagicCaster
{
public:
	MagicCaster();
	~MagicCaster();

	UInt32 vtabl;
	UInt32 unk004;	// 004
	UInt32 unk008;	// 008
};
STATIC_ASSERT(sizeof(MagicCaster) == 0x00C);

typedef tList<ActiveEffect> EffectNode;
// 010
class MagicTarget
{
public:
	MagicTarget();
	~MagicTarget();

	virtual void	Destructor(void);
	virtual TESObjectREFR *	GetParent(void);
	virtual EffectNode *	GetEffectList(void);

	UInt8			byt004;		// 004 
	UInt8			byt005;		// 005 
	UInt8			byt006[2];	// 006-7
	tList<void*>	lst008;		// 008

	void RemoveEffect(EffectItem *effItem);

	void StopEffect(void *arg0, bool arg1)
	{
		ThisStdCall(0x8248E0, this, arg0, arg1);
	}
};

STATIC_ASSERT(sizeof(MagicTarget) == 0x010);

class hkaRaycastInterface
{
public:
	hkaRaycastInterface();
	~hkaRaycastInterface();
	virtual hkaRaycastInterface*	Destroy(bool doFree);
	virtual void					Unk_01(void* arg0);
	virtual void					Unk_02(void);

	// Assumed to be 0x010 bytes due to context where the vftable is used
	UInt32	unk000[(0x010 - 0x004) >> 2];	// 0000
};
STATIC_ASSERT(sizeof(hkaRaycastInterface) == 0x010);

class bhkRagdollController : public hkaRaycastInterface
{
public:
	bhkRagdollController();
	~bhkRagdollController();

	UInt32	unk000[(0x021C - 0x010) >> 2];	// 0010
	UInt8	fill021C[3];					// 021C
	bool	bool021F;						// 021F	when null assume FIK status is false
	bool	fikStatus;						// 0220
	UInt8	fill0221[3];					// 0221
};
STATIC_ASSERT(sizeof(bhkRagdollController) == 0x0224);

class bhkRagdollPenetrationUtil;
class ActorMover;
class PlayerMover;
class ImageSpaceModifierInstanceDOF;
class ImageSpaceModifierInstanceDRB;
class PathingSolution;

class PathingLocation
{
public:
	/*virtual void	Unk_00(void);
	virtual void	Unk_01(void);
	virtual void	Unk_02(void);*/

	void** vtbl;		// 00
	NiPoint3		pos;		// 04
	UInt32			unk10[6];	// 10

	PathingLocation(TESObjectREFR* refr) { ThisStdCall(0x6DCD70, this, refr); }
};

class PathingRequest : public NiRefObject
{
public:
	/*08*/virtual void	Unk_02(void);
	/*0C*/virtual void	Unk_03(void);
	/*10*/virtual void	Unk_04(void);
	/*14*/virtual void	Unk_05(void);
	/*18*/virtual void	Unk_06(void);
	/*1C*/virtual void	Unk_07(void);
	/*20*/virtual void	Unk_08(void);

	struct ActorData
	{
		TESForm* baseForm;
		void* inventoryChanges;
		bool		isAlarmed;

		ActorData(Actor* actor) { ThisStdCall(0x502670, this, actor); }
	};

	UInt32					unk08;					// 08
	PathingLocation			start;					// 0C
	PathingLocation			dest;					// 34
	ActorData				actorData;				// 5C
	float					actorRadius;			// 68
	float					flt6C;					// 6C
	float					goalZDelta;				// 70
	float					targetRadius;			// 74
	float					centerRadius;			// 78
	NiVector3				targetPt;				// 7C
	float					unk88;					// 88
	float					goalAngle;				// 8C
	float					initialPathHeading;		// 90
	void*					avoidNodeArray;		// 94 - PathingAvoidNodeArray
	UInt8					bCantOpenDoors;			// 98
	UInt8					bFaceTargetAtGoal;		// 99
	UInt8					byte9A;					// 9A
	UInt8					bAllowIncompletePath;	// 9B
	UInt8					byte9C;					// 9C
	UInt8					bCanSwim;				// 9D
	UInt8					bCanFly;				// 9E
	UInt8					byte9F;					// 9F
	UInt8					bInitialPathHeading;	// A0
	UInt8					byteA1;					// A1
	UInt8					bCurvedPath;			// A2
	UInt8					byteA3;					// A3
	UInt8					bIgnoreLocks;			// A4
	UInt8					padA5[3];				// A5
	UInt32					iSmoothingRetryCount;	// A8
	UInt8					byteAC;					// AC
	UInt8					padAD[3];				// AD
};

class ActorMover
{
public:
	virtual void		Destroy(bool doFree);
	virtual void		Unk_01(void);
	virtual void		ClearMovementFlag(UInt32 flag);
	virtual void		SetMovementFlags(UInt32 movementFlags);
	virtual void		Unk_04(void);
	virtual void		Unk_05(void);
	virtual void		HandleTurnAnimationTimer(float timePassed);
	virtual void		Unk_07(void);
	virtual UInt32		GetMovementFlags();
	virtual void		Unk_09(void);
	virtual void		Unk_0A(void);
	virtual void		Unk_0B(void);
	virtual void		Unk_0C(void);
	virtual void		Unk_0D(void);
	virtual void		Unk_0E(void);

	enum MovementFlags
	{
		kMoveFlag_Forward = 1,
		kMoveFlag_Backward = 2,
		kMoveFlag_Left = 4,
		kMoveFlag_Right = 8,
		kMoveFlag_TurnLeft = 0x10,
		kMoveFlag_TurnRight = 0x20,
		kMoveFlag_IsKeyboard = 0x40,	// (returns that the movement is for non-controller)
		kMoveFlag_Walking = 0x100,
		kMoveFlag_Running = 0x200,
		kMoveFlag_Sneaking = 0x400,
		kMoveFlag_Swimming = 0x800,
		kMoveFlag_Jump = 0x1000,
		kMoveFlag_Flying = 0x2000,
		kMoveFlag_Fall = 0x4000,
		kMoveFlag_Slide = 0x8000
	};

	NiVector3					point04;			// 04
	NiVector3					overrideMovement;	// 10
	PathingRequest*				pathingRequest;	// 1C
	PathingSolution*			pathingSolution;	// 20
	void*						pathHandler;		// 24 - DetailedActorPathHandler
	Actor*						actor;				// 28
	UInt32						unk2C;				// 2C
	void*						pathingMsgQueue;	// 30 - ActorPathingMessageQueue
	UInt32						movementFlags1;		// 34
	UInt32						unk38;				// 38
	UInt32						movementFlags2;		// 3C
	UInt32						unk40;				// 40
	PathingLocation				pathingLocation;	// 44
	UInt32						unk6C;				// 6C
	UInt8						bPathingFailed;		// 70
	UInt8						byte71;				// 71
	UInt8						byte72;				// 72
	UInt8						byte73;				// 73
	UInt8						bWaitingOnPath;		// 74
	UInt8						byte75;				// 75
	UInt8						byte76;				// 76
	UInt8						bOverrideMovement;	// 77
	UInt32						unk78;				// 78
	UInt32						unk7C;				// 7C
	UInt32						unk80;				// 80
	UInt32						unk84;				// 84
};
static_assert(sizeof(ActorMover) == 0x88);


typedef std::vector<TESForm*> EquippedItemsList;

struct ActorValueMod
{
	UInt8		avCode;
	UInt8		pad01[3];
	float		mod;
};

class Actor : public MobileObject
{
public:
	/*304*/virtual UInt8	IsGuard();
	/*308*/virtual void		SetGuard(UInt8 setTo);
	/*30C*/virtual void		Unk_C3(void);
	/*310*/virtual void		Unk_C4(void);
	/*314*/virtual void		Unk_C5(void);
	/*318*/virtual void		Unk_C6(void);
	/*31C*/virtual void		SetIgnoreCrime(bool ignoreCrime);
	/*320*/virtual bool		GetIgnoreCrime();
	/*324*/virtual void		Resurrect(UInt8 arg1, UInt8 arg2, UInt8 arg3);
	/*328*/virtual void		Unk_CA(void);
	/*32C*/virtual void		Unk_CB(void);
	/*330*/virtual void		Unk_CC(void);
	/*334*/virtual void		Unk_CD(void);
	/*338*/virtual void		DamageHealthAndFatigue(float healthDmg, float fatigueDmg, Actor* source);
	/*33C*/virtual void		DamageActionPoints(float amount); // checks GetIsGodMode before decreasing
	/*340*/virtual void		Unk_D0(void);
	/*344*/virtual int		CalculateDisposition(Actor* target, void* arg2);
	/*348*/virtual void		UpdateMovement(float arg1, UInt32 arg2);
	/*34C*/virtual void		Unk_D3(void);
	/*350*/virtual void		Unk_D4(void);
	/*354*/virtual float	GetDefaultTurningSpeed();
	/*358*/virtual bool		IsOverencumbered();
	/*35C*/virtual void		Unk_D7(void);
	/*360*/virtual bool		IsPlayerRef();
	/*364*/virtual void		Unk_D9(void);
	/*368*/virtual void		Unk_DA(void);
	/*36C*/virtual void		Unk_DB(void);
	/*370*/virtual void		Unk_DC(void);
	/*374*/virtual void		Unk_DD(void);
	/*378*/virtual void		Unk_DE(void);
	/*37C*/virtual TESRace* GetRace();
	/*380*/virtual float	GetHandReachTimesCombatDistance();
	/*384*/virtual void		Unk_E1(void);
	/*388*/virtual void		Unk_E2(void);
	/*38C*/virtual bool		IsPushable();
	/*390*/virtual UInt32	GetActorType();	// Creature = 0, Character = 1, PlayerCharacter = 2
	/*394*/virtual void		SetActorValue(UInt32 avCode, float value);
	/*398*/virtual void		SetActorValueInt(UInt32 avCode, UInt32 value);
	/*39C*/virtual void		ModActorValue(UInt32 avCode, float modifier, Actor* attacker);
	/*3A0*/virtual void		Unk_E8(void);
	/*3A4*/virtual void		ForceActorValue(UInt32 avCode, float modifier, UInt32 arg3);
	/*3A8*/virtual void		ModActorValueInt(UInt32 avCode, int modifier, UInt32 arg3);
	/*3AC*/virtual void		DamageActorValue(UInt32 avCode, float damage, Actor* attacker);
	/*3B0*/virtual void		Unk_EC(void);
	/*3B4*/virtual void		Unk_ED(void);
	/*3B8*/virtual void		Unk_EE(void);
	/*3BC*/virtual void* GetPreferedWeapon(UInt32 unk);	// void* == ContChangesEntry
	/*3C0*/virtual void		Unk_F0(void);
	/*3C4*/virtual void		ResetArmorDRDT();
	/*3C8*/virtual bool		DamageItem(void* itemEntry, float damage, int unused); // void* == ContChangesEntry
	/*3CC*/virtual void		DropItem(TESForm* itemForm, ExtraDataList* xDataList, SInt32 count, NiVector3* pos, int arg5);
	/*3D0*/virtual void		DoActivate(TESObjectREFR* activatedRef, UInt32 count, bool arg3);
	/*3D4*/virtual void		Unk_F5(void);
	/*3D8*/virtual void		Unk_F6(void);
	/*3DC*/virtual void		Unk_F7(void);
	/*3E0*/virtual bool		AddActorEffect(SpellItem* actorEffect);
	/*3E4*/virtual bool		RemoveActorEffect(SpellItem* actorEffect);
	/*3E8*/virtual void		Reload(TESObjectWEAP* weapon, UInt32 animType, UInt8 hasExtendedClip);
	/*3EC*/virtual void		Reload2(TESObjectWEAP* weapon, UInt32 animType, UInt8 hasExtendedClip, UInt8 isInstantSwapHotkey);
	/*3F0*/virtual void		DecreaseAmmo(int amount);
	/*3F4*/virtual void		Unk_FD(void);
	/*3F8*/virtual CombatActors* GetCombatGroup();
	/*3FC*/virtual void		SetCombatGroup(CombatActors* combatActors);
	/*400*/virtual void		Unk_100(void);
	/*404*/virtual void		Unk_101(void);
	/*408*/virtual void		Unk_102(void);
	/*40C*/virtual void		Unk_103(void);
	/*410*/virtual void		Unk_104(void);
	/*414*/virtual void		Unk_105(void);
	/*418*/virtual void		InitGetUpPackage();
	/*41C*/virtual void		SetAlpha(float alpha);
	/*420*/virtual float	GetAlpha();
	/*424*/virtual void		ForceAttackActor(Actor* target, CombatActors* combatGroup, UInt8 arg3, UInt32 arg4, UInt8 arg5, UInt32 arg6, UInt32 arg7, UInt32 arg8);
	/*428*/virtual CombatController* GetCombatController();
	/*42C*/virtual Actor* GetCombatTarget();
	/*430*/virtual void		UpdateCombat();
	/*434*/virtual void		StopCombat(Actor* target);
	/*438*/virtual void		Unk_10E(void);
	/*43C*/virtual float	GetTotalArmorDR();
	/*440*/virtual float	GetTotalArmorDT();
	/*444*/virtual UInt32	Unk_111(void);
	/*448*/virtual bool		IsTrespassing();
	/*44C*/virtual void		Unk_113(void);
	/*450*/virtual void		Unk_114(void);
	/*454*/virtual void		Unk_115(void);
	/*458*/virtual float	CalculateWalkSpeed();
	/*45C*/virtual float	CalculateRunSpeed(); 
	/*460*/virtual void		ModDisposition(Actor* target, float value);
	/*464*/virtual float	GetDisposition(Actor* target);
	/*468*/virtual void		ClearDisposition(Actor* target);
	/*46C*/virtual void		SetStartingPosition();
	/*470*/virtual bool		GetAttacked();
	/*474*/virtual void		Unk_11D(void);
	/*478*/virtual void		Unk_11E(void);
	/*47C*/virtual void		Unk_11F(void);
	/*480*/virtual void		Unk_120(void);
	/*484*/virtual void		Unk_121(void);
	/*488*/virtual void		RewardXP(UInt32 amount);
	/*48C*/virtual void		Unk_123(void);
	/*490*/virtual void		Unk_124(void);
	/*494*/virtual void		Unk_125(void);
	/*498*/virtual void		SetPerkRank(BGSPerk* perk, UInt8 rank, bool alt);
	/*49C*/virtual void		RemovePerk(BGSPerk* perk, bool alt);
	/*4A0*/virtual UInt8	GetPerkRank(BGSPerk* perk, bool alt);
	/*4A4*/virtual void		AddPerkEntryPoint(BGSEntryPointPerkEntry* perkEntry, bool alt);
	/*4A8*/virtual void		RemovePerkEntryPoint(BGSEntryPointPerkEntry* perkEntry, bool alt);
	/*4AC*/virtual void* GetPerkEntryPointList(UInt8 entryPointID, bool alt);	// void == PerkEntryPointList
	/*4B0*/virtual void		Unk_12C(void);
	/*4B4*/virtual bool		GetIsImmobileCreature();
	/*4B8*/virtual void		DoHealthDamage(Actor* attacker, float damage);
	/*4BC*/virtual void		Unk_12F(void);
	/*4C0*/virtual void		Unk_130(void);
	/*4C4*/virtual float	Unk_131(void);
	/*4C8*/virtual void		HandleHeadTracking(float arg1);
	/*4CC*/virtual void		UpdateHeadTrackingEmotions(UInt32 arg1);
	/*4D0*/virtual void		Unk_134(void);
	/*4D4*/virtual void		Unk_135(void);
	/*4D8*/virtual NiVector3* GetAnticipatedLocation(NiVector3* resPos, float time);

	enum LifeStates
	{
		kLifeState_Alive = 0,
		kLifeState_Dying = 1,
		kLifeState_Dead = 2,
		kLifeState_Unconscious = 3,
		kLifeState_Reanimate = 4,
		kLifeState_Restrained = 5
	};

	struct Disposition
	{
		float	value;
		Actor* target;
	};

	MagicCaster			magicCaster;			// 088
	MagicTarget			magicTarget;			// 094
	ActorValueOwner		avOwner;				// 0A4
	CachedValuesOwner	cvOwner;				// 0A8

	bhkRagdollController* ragDollController;			// 0AC
	bhkRagdollPenetrationUtil* ragDollPentrationUtil;		// 0B0
	UInt32								unk0B4;						// 0B4-
	float								flt0B8;						// 0B8
	bool								bAIState;					// 0BC-
	UInt8								byte0BD;					// 0BD
	UInt8								byte0BE;					// 0BE
	UInt8								byte0BF;					// 0BF
	Actor* killer;					// 0C0
	UInt8								byte0C4;					// 0C4-
	UInt8								byte0C5;					// 0C5
	UInt8								byte0C6;					// 0C6
	UInt8								byte0C7;					// 0C7
	float								flt0C8;						// 0C8
	float								flt0CC;						// 0CC
	tList<ActorValueMod>				forceAVList;				// 0D0
	UInt8								byte0D8;					// 0D8
	UInt8								byte0D9;					// 0D9
	UInt8								byte0DA;					// 0DA
	UInt8								byte0DB;					// 0DB
	UInt32								unk0DC;						// 0DC
	tList<ActorValueMod>				setAVList;					// 0E0
	UInt8								byte0E8;					// 0E8	const 1
	UInt8								byte0E9;					// 0E9
	UInt8								byte0EA;					// 0EA
	UInt8								byte0EB;					// 0EB
	UInt32								unk0EC;						// 0EC
	UInt8								byte0F0;					// 0F0-
	UInt8								byte0F1;					// 0F1-
	UInt8								byte0F2;					// 0F2
	UInt8								byte0F3;					// 0F3
	tList<void>							list0F4;					// 0F4
	tList<Disposition>					dispositionList;			// 0FC
	bool								isInCombat;					// 104
	UInt8								jipActorFlags1;				// 105
	UInt8								jipActorFlags2;				// 106
	UInt8								jipActorFlags3;				// 107
	UInt32								lifeState;					// 108	saved as byte HasHealth
	UInt32								criticalStage;				// 10C
	UInt32								unk110;						// 110-
	float								painSoundTimer;				// 114
	UInt8								byte118;					// 118-
	UInt8								byte119;					// 119+
	UInt8								byte11A;					// 11A
	UInt8								byte11B;					// 11B
	UInt32								unk11C;						// 11C-
	UInt32								resetTime;					// 120-
	bool								forceRun;					// 124
	bool								forceSneak;					// 125
	UInt8								byte126;					// 126-
	bool								searchingForEnemies;		// 127
	Actor* combatTarget;				// 128
	BSSimpleArray<Actor*>* combatTargets;				// 12C
	BSSimpleArray<Actor*>* combatAllies;				// 130
	UInt8								byte134;					// 134-
	UInt8								byte135;					// 135+
	UInt8								byte136;					// 136
	UInt8								byte137;					// 137
	UInt32								unk138;						// 138-
	UInt32								minorCrimeCount;			// 13C
	UInt32								actorFlags;					// 140	0x80000000 - IsEssential
	bool								ignoreCrime;				// 144
	UInt8								byte145;					// 145	Has to do with package evaluation
	UInt8								byte146;					// 146	Has to do with package evaluation
	UInt8								byte147;					// 147
	UInt32								unk148;						// 148-
	UInt8								inWater;					// 14C
	UInt8								isSwimming;					// 14D
	UInt8								byte14E;					// 14E
	UInt8								byte14F;					// 14F
	UInt32								unk150;						// 150-
	float								flt154;						// 154
	float								flt158;						// 158
	UInt8								byte15C;					// 15C-
	UInt8								byte15D;					// 15D-
	UInt8								byte15E;					// 15E
	UInt8								byte15F;					// 15F
	NiVector3							startingPos;				// 160
	float								startingZRot;				// 16C
	TESForm* startingWorldOrCell;		// 170
	UInt8								byte174;					// 174-
	UInt8								byte175;					// 175-
	UInt8								byte176;					// 176
	UInt8								byte177;					// 177
	float								flt178;						// 178
	float								weaponSpread1;				// 17C
	float								weaponSpread2;				// 180
	float								weaponSpread3;				// 184
	float								flt188;						// 188
	UInt8								byte18C;					// 18C-
	bool								isTeammate;					// 18D
	UInt8								byte18E;					// 18E-
	UInt8								byte18F;					// 18F
	ActorMover* actorMover;				// 190
	UInt32								unk194;						// 194-
	UInt32								unk198;						// 198-
	float								flt19C;						// 19C
	UInt32								unk1A0;						// 1A0-
	UInt32								speechExpression;			// 1A4
	UInt32								emotionValue;				// 1A8
	UInt32								sitSleepState;				// 1AC-
	UInt8								isImmobileCreature;			// 1B0-
	bool								forceHit;					// 1B1-
	UInt8								byte1B2;					// 1B2
	UInt8								byte1B3;					// 1B3

	//__forceinline void EquipItem(TESForm* objType, UInt32 equipCount = 1, ExtraDataList* itemExtraList = NULL, bool applyEnchantment = 1, bool lockEquip = 0, bool noMessage = 1)
	//{
	//	//ThisCall(0x88C650, this, objType, equipCount, itemExtraList, applyEnchantment, lockEquip, noMessage);
	//}
	//__forceinline void UnequipItem(TESForm* objType, UInt32 unequipCount = 1, ExtraDataList* itemExtraList = NULL, bool removeEnchantment = 1, bool lockUnequip = 0, bool noMessage = 1)
	//{
	//	//ThisCall(0x88C790, this, objType, unequipCount, itemExtraList, removeEnchantment, lockUnequip, noMessage);
	//}

	bool GetDead() const { return (lifeState == 1) || (lifeState == 2); }
	bool GetRestrained() const { return lifeState == 5; }
	//__forceinline NiNode* GetRefNiNode() const { return renderState ? renderState->rootNode : nullptr; }

	TESActorBase* GetActorBase() const {
		ExtraLeveledCreature* xLvlCre = (ExtraLeveledCreature*)(extraDataList).GetByType(kExtraData_LeveledCreature);
		return (xLvlCre && xLvlCre->form) ? (TESActorBase*)xLvlCre->form : (TESActorBase*)baseForm;
	}
	bool GetLOS(Actor* target) const;
	char GetCurrentAIPackage() const;
	char GetCurrentAIProcedure() const;
	bool IsFleeing() const;
	ContChangesEntry* GetWeaponInfo() const;	//void == ContChangesEntry
	ContChangesEntry* GetAmmoInfo() const;		//void == ContChangesEntry
	
	
	void EquipItem(TESForm* objType, UInt32 equipCount, ExtraDataList* itemExtraList, UInt32 unk3, bool lockEquip, UInt32 unk5);

	void UnequipItem(TESForm* objType, UInt32 unk1, ExtraDataList* itemExtraList, UInt32 unk3, bool lockUnequip, UInt32 unk5);

	EquippedItemsList GetEquippedItems();
	
	ExtraContainerDataArray GetEquippedEntryDataList();
	ExtraContainerExtendDataArray GetEquippedExtendDataList();
	TESObjectWEAP* GetEquippedWeapon() const;
	void AimWeapon(bool shouldAim, bool hasQueuedIdleFlags10000);
	bool SetBlocking(bool shouldBlock);
	void SetWantsWeaponOut(bool wantsWeaponOut);
	bool IsWeaponOut();
	bool IsItemEquipped(TESForm* item) const;
	bool EquippedWeaponHasMod(UInt32 modType) const;
	UInt8 EquippedWeaponHasScope() const;
	UInt8 EquippedWeaponSilenced() const;
	bool IsSneaking() const;
	void StopCombat();
	bool __fastcall IsInCombatWith(Actor* target) const;
	int __fastcall GetDetectionValue(Actor* detected) const;
	TESPackage* GetStablePackage() const;
	PackageInfo* GetPackageInfo() const;
	TESObjectREFR* GetPackageTarget() const;
	TESCombatStyle* GetCombatStyle() const;
	SInt8 GetKnockedState() const;
	bool IsWeaponOut() const;
	void UpdateActiveEffects(MagicItem* magicItem, EffectItem* effItem, bool addNew);
	bool GetIsGhost() const;
	float GetRadiationLevel() const;
	void* AddBackUpPackage(TESObjectREFR* targetRef, TESObjectCELL* targetCell, UInt32 flags); //void == BackUpPackage
	void __fastcall TurnToFaceObject(TESObjectREFR* target);
	void __vectorcall TurnAngle(float angle);
	void PlayAnimGroup(UInt32 animGroupID);
	UInt32 GetLevel() const;
	double GetKillXP() const;
	void __fastcall GetHitDataValue(UInt32 valueType, double* result) const;
	void DismemberLimb(UInt32 bodyPartID, bool explode);
	void EquipItemAlt(void* entry, UInt32 noUnequip = 0, UInt32 noMessage = 1); // void == ContChangesEntry
	bool HasNoPath() const;
	bool CanBePushed() const;
	float __vectorcall AdjustPushForce(float baseForce);
	void PushActor(float force, float angle, TESObjectREFR* originRef);
	int GetGroundMaterial() const;
	void RefreshAnimData();
	double GetPathingDistance(TESObjectREFR* target);
};


STATIC_ASSERT(offsetof(Actor, magicCaster) == 0x088);

class Character : public Actor
{
public:
	Character();
	~Character();

	virtual void		Unk_137(void);
	virtual void		Unk_138(void);

	ValidBip01Names	* validBip01Names;	// 1B4
	float			flt1B8;				// 1B8
	float			flt1BC;				// 1BC
	UInt8			byt1C0;				// 1C0
	UInt8			byt1C1;				// 1C1
	UInt16			unk1C2;				// 1C2
	float			flt1C4;				// 1C4
};

struct CombatActors;

typedef tList<BGSQuestObjective::Target> QuestObjectiveTargets;

// 9BC
class PlayerCharacter : public Character
{
public:
	PlayerCharacter();
	~PlayerCharacter();

	// used to flag controls as disabled in disabledControlFlags
	enum {
		kControlFlag_Movement		= 1 << 0,
		kControlFlag_Look			= 1 << 1,
		kControlFlag_Pipboy			= 1 << 2,
		kControlFlag_Fight			= 1 << 3,
		kControlFlag_POVSwitch		= 1 << 4,
		kControlFlag_RolloverText	= 1 << 5,
		kControlFlag_Sneak			= 1 << 6,
	};

	virtual void		Unk_139(void);
	virtual void		Unk_13A(void);

	UInt32								unk1C8[9];				// 1C8	208 could be a DialogPackage
	void* posRequest;			// 1EC
	TESForm* queuedWeapon;			// 1F0
	UInt32								unk1F4;					// 1F4
	UInt8								byte1F8;				// 1F8
	UInt8								pad1F9[3];				// 1F9
	UInt32								unk1FC[2];				// 1FC
	UInt8								byte204;				// 204
	UInt8								byte205;				// 205
	UInt8								recalcQuestTargets;		// 206
	UInt8								byte207;				// 207
	TESPackage* package208;			// 208
	void* unk20C;				// 20C
	tList<ActiveEffect>* activeEffects;			// 210
	MagicItem* pcMagicItem;			// 214
	MagicTarget* pcMagicTarget;			// 218
	void* cameaCollision;		// 21C
	UInt32								unk220[8];				// 220	224 is a package of type 1C
	bool								showQuestItems;			// 240
	UInt8								byte241;				// 241
	UInt8								byte242;				// 242
	UInt8								byte243;				// 243
	float								unk244[77];				// 244	have to be a set of ActorValue
	float								permAVMods[77];			// 378	have to be a set of ActorValue
	float								flt4AC;					// 4AC
	float								actorValues4B0[77];		// 4B0	have to be a set of ActorValue
	tList<BGSNote>						notes;					// 5E4
	ImageSpaceModifierInstanceDOF* unk5EC;				// 5EC
	ImageSpaceModifierInstanceDOF* unk5F0;				// 5F0
	ImageSpaceModifierInstanceDRB* unk5F4;				// 5F4
	UInt8								byte5F8;				// 5F8
	UInt8								byte5F9;				// 5F9
	UInt8								byte5FA;				// 5FA
	UInt8								byte5FB;				// 5FB
	tList<TESObjectREFR>				teammates;				// 5FC
	TESObjectREFR* lastExteriorDoor;		// 604
	UInt8								isPlayingAttackSound;	// 608
	UInt8								pad609[3];				// 609
	tList<void>* actionList;			// 60C
	tList<void>* casinoDataList;		// 610
	tList<void>* caravanCards1;			// 614
	tList<void>* caravanCards2;			// 618
	UInt32								unk61C[6];				// 61C
	void* ptr634;				// 634	bhkMouseSpringAction when there's a grabbed reference
	TESObjectREFR* grabbedRef;			// 638
	UInt32								unk63C;					// 63C
	UInt32								unk640;					// 640
	float								grabbedWeight;			// 644
	UInt8								byte648;				// 648
	UInt8								byte649;				// 649
	bool								bIs3rdPersonVisible;	// 64A
	bool								is3rdPerson;			// 64B
	bool								bThirdPerson;			// 64C
	UInt8								byte64D;				// 64D
	UInt8								byte64E;				// 64E
	bool								isUsingScope;			// 64F
	UInt8								byte650;				// 650
	bool								alwaysRun;				// 651
	bool								autoMove;				// 652
	UInt8								byte653;				// 653
	UInt32								sleepHours;				// 654
	UInt8								isResting;				// 658	Something to do with SleepDeprivation; see 0x969DCF
	UInt8								byte659;				// 659
	UInt8								byte65A;				// 65A
	UInt8								byte65B;				// 65B
	float								flt65C;					// 65C
	UInt32								unk660[3];				// 660
	UInt8								byte66C;				// 66C
	UInt8								byte66D;				// 66D
	UInt8								byte66E;				// 66E
	UInt8								byte66F;				// 66F
	float								worldFOV;				// 670
	float								firstPersonFOV;			// 674
	float								flt678;					// 678
	UInt32								unk67C;					// 67C
	UInt8								pcControlFlags;			// 680
	UInt8								byte681;				// 681
	UInt8								byte682;				// 682
	UInt8								byte683;				// 683
	UInt32								unk684[2];				// 684
	BipedAnim* bipedAnims1stPerson;	// 68C
	AnimData* animData1stPerson;		// 690
	NiNode* node1stPerson;			// 694
	float								flt698;					// 698
	UInt32								unk69C[3];				// 69C
	tList<TESTopic>						topicList;				// 6A8
	UInt32								unk6B0[2];				// 6B0
	TESQuest* activeQuest;			// 6B8
	tList<void>			questObjectiveList;		// 6BC
	tList<void>				questTargetList;		// 6C4
	UInt32								unk6CC[5];				// 6CC
	float								sortActorDistanceListTimer;	// 6E0
	float								seatedRotation;			// 6E4
	UInt32								unk6E8;					// 6E8
	UInt32								unk6EC;					// 6EC
	UInt32								playerSpell;			// 6F0
	TESObjectREFR* placedMarkerRef;		// 6F4
	bool								pad[0x38];		// 6F8
	float 								timeGrenadeHeld;		// 730
	UInt32								unk734[10];				// 734
	bool								inCharGen;				// 75C
	UInt8								byte75D;				// 75D
	UInt8								canUseTelekinesis;		// 75E
	UInt8								byte75F;				// 75F
	TESRegion* currentRegion;			// 760
	TESRegionList						regionsList;			// 764
	UInt32								unk774[6];				// 774
	UInt32								initialTickCount;		// 78C
	UInt32								timePlayedCurrGame;		// 790	ms
	UInt32								unk794[6];				// 794
	TESForm* pcWorldOrCell;			// 7AC
	UInt32								unk7B0;					// 7B0
	BGSMusicType* musicType;				// 7B4
	int									gameDifficulty;			// 7B8
	UInt32								isHardcore;				// 7BC
	UInt32								killCamMode;			// 7C0
	UInt8								inCombatWithGuard;		// 7C4
	bool								isYoung;				// 7C5
	bool								isToddler;				// 7C6
	bool								canUsePA;				// 7C7
	tList<void>				mapMarkers;				// 7C8
	TESWorldSpace* mapMarkersWorldspace;	// 7D0
	tList<void>					musicMarkers;			// 7D4
	void* currMusicMarker;		// 7DC
	float								flycamZRot;				// 7E0
	float								flycamXRot;				// 7E4
	NiVector3							flycamPos;				// 7E8
	UInt32								unk7F4[34];				// 7F4
	void* pcLevelData;			// 878
	tList<void>						perkRanksPC;			// 87C
	tList<void>					perkEntriesPC[74];		// 884
	tList<void>						perkRanksTM;			// AD4
	tList<void>					perkEntriesTM[74];		// ADC
	Actor* autoAimActor;			// D2C
	NiVector3							CastRayPos;					// D30
	NiObject* unkD3C;				// D3C
	UInt32								unkD40;					// D40
	Actor* reticleActor;			// D44
	tList<void>* compassTargets;		// D48
	UInt32								unkD4C;					// D4C
	float								lastAmmoSwapTime;		// D50
	UInt8								shouldOpenPipboy;		// D54
	UInt8								unusedByteD55;			// D55
	UInt8								unusedByteD56;			// D56
	UInt8								unusedByteD57;			// D57	Used by SneakBoundingBoxFixHook
	NiVector3							cameraPos3rdPerson;		// D58
	CombatActors* combatActors;			// D64
	UInt32								teammateCount;			// D68
	UInt32								unkD6C[5];				// D6C
	NiNode* niNodeD80;				// D80
	UInt32								unkD84[12];				// D84
	NiNode* niNodeDB4;				// DB4
	UInt32								unkDB8[7];				// DB8
	NiVector3							cameraPos1stPerson;		// DD4
	NiVector3							cameraPos;				// DE0
	void* rigidBody;				// DEC
	bool								pcInCombat;				// DF0
	bool								pcUnseen;				// DF1
	UInt8								isLODApocalypse;		// DF2
	UInt8								byteDF3;				// DF3
	BSSimpleArray<ContChangesEntry*>	rockItLauncherContainer;// DF4
	float								rockItLauncherWeight;	// E04
	UInt8								nightVisionApplied;		// E08
	UInt8								unusedByteE09;			// E09
	UInt8								unusedByteE0A;			// E0A
	UInt8								unusedByteE0B;			// E0B
	TESReputation* lastModifiedRep;		// E0C
	UInt32								unkE10[2];				// E10
	float								killCamTimer;			// E18
	float								killCamCooldown;		// E1C
	UInt8								byteE20;				// E20
	UInt8								isUsingTurbo;			// E21
	UInt8								byteE22;				// E22
	UInt8								byteE23;				// E23
	float								fltE24;					// E24
	float								counterAttackTimer;		// E28
	UInt8								byteE2C;				// E2C
	UInt8								cateyeEnabled;			// E2D
	UInt8								spottingEnabled;		// E2E
	UInt8								byteE2F;				// E2F
	UInt32								itemDetectionTimer;		// E30
	NiNode* ironSightNode;			// E34
	bool								noHardcoreTracking;		// E38	Appears to be unused
	bool								skipHCNeedsUpdate;		// E39
	UInt8								byteE3A;				// E3A
	UInt8								byteE3B;				// E3B
	BSSimpleArray<TESAmmo*>				hotkeyedWeaponAmmos;	// E3C
	UInt32								unkE4C;					// E4C

		// 7C4 is a byte used during Combat evaluation (Player is targetted ?), 
		// 7C6 is a boolean meaning toddler, 
		// 7C7 byte bool PCCanUsePowerArmor, Byt0E39 referenced during LoadGame
		// Used when entering FlyCam : 7E8/7EC/7F0 stores Pos, 7F0 adjusted by scaledHeight multiplied by 0698 , 7E0 stores RotZ, 7E4 RotX
		// Perks forms a list at 87C and AD4. Caravan Cards at 614 and 618. Quest Stage LogEntry at 6B0. Quest Objectives at 6BC.
		// Hardcore flag would be E38. Byte at DF0 seems to be PlayerIsInCombat
		// tList at 6C4 is cleared when there is no current quest. There is another NiNode at 069C
		// list of perk and perkRank are at 0x087C and 0x0AD4 (alt perks). 086C is cleared after equipement change.
		// D68 counts the Teammates.
		// D74: 96 bytes that are cleared when the 3D is cleared.

	bool IsThirdPerson() const { return is3rdPerson ? true : false; }
	UInt32 GetMovementFlags() { return actorMover->GetMovementFlags(); }	// 11: IsSwimming, 9: IsSneaking, 8: IsRunning, 7: IsWalking, 0: keep moving
	bool IsPlayerSwimming() { return (GetMovementFlags()  >> 11) & 1; }

	__forceinline static PlayerCharacter* GetSingleton() { return *(PlayerCharacter**)0x11DEA3C; }

	bool SetSkeletonPath(const char* newPath);
	bool SetSkeletonPath_v1c(const char* newPath);	// Less worse version as used by some modders
	static void UpdateHead(void);
	QuestObjectiveTargets* GetCurrentQuestObjectiveTargets();

	void UpdateCamera(bool isCalledFromFunc21, bool _zero_skipUpdateLOD);
};

extern PlayerCharacter* g_thePlayer;

class PlayerMover : public ActorMover
{
public:
	float            flt88;                // 88
	float            flt8C;                // 8C
	float            flt90;                // 90
	UInt32           pcMovementFlags;	  // 94
	UInt32           turnAnimFlags;	     // 98
	float            turnAnimMinTime;    // 9C
};

STATIC_ASSERT(offsetof(PlayerCharacter, ragDollController) == 0x0AC);
STATIC_ASSERT(offsetof(PlayerCharacter, questObjectiveList) == 0x6BC);
//STATIC_ASSERT(offsetof(PlayerCharacter, bThirdPerson) == 0x64C);
STATIC_ASSERT(offsetof(PlayerCharacter, actorMover) == 0x190);
STATIC_ASSERT(sizeof(PlayerCharacter) == 0xE50);
