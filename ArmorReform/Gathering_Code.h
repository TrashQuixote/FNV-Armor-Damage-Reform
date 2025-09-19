#pragma once
#include <unordered_set>
#include <chrono>
#include <random>
#include <array>
#include <variant>
#include <intrin.h>
#include "nvse/GameSettings.h"
#include "nvse/PluginAPI.h"
#include "nvse/GameData.h"
#include "nvse/SafeWrite.h"
#include "nvse/NiObjects.h"
#include "GameForms.h"
#include "GameObjects.h"
#include "NiPoint.h"
#include "internal/class_vtbls.h"
#include "GameExtraData.h"
#include "Gathering_GameOSInput.h"
#include "GameUI.h"
/*
float	radius;					// 00
float	widthX;					// 04
float	widthY;					// 08
float	height;					// 0C
float	DPS;					// 10
float	medicineSkillMult;		// 14
float	survivalSkillMult;		// 18
float	paralysis;				// 1C
float	healRate;				// 20
float	fatigueReturnRate;		// 24
float	perceptionCondition;	// 28
float	eyeHeight;				// 2C
SInt32	aggression;				// 30
SInt32	assistance;				// 34
float	walkSpeed;				// 38
float	runSpeed;				// 3C
UInt8	hasNoCrippledLegs;		// 40
UInt8	pad41[3];				// 41
UInt32	flags;					// 44
*/


using _FactionListData = TESActorBaseData::FactionListData;
#define BodyPart(_part) TESBipedModelForm::EPartBit::ePart_##_part
#define ADDR_ReturnTrue			0x8D0360
#define ADDR_ReturnThis			0x6815C0
#define ADDR_ReturnThis2		0xE68810
#define UseInventoryThings 0
#define UnuseInThisPlugin 1
#define IS_REFERENCE(form) ((*(UInt32**)form)[0xF0 >> 2] == ADDR_ReturnTrue)
#define NOT_REFERENCE(form) ((*(UInt32**)form)[0xF0 >> 2] != ADDR_ReturnTrue)
#define IS_ACTOR(form) ((*(UInt32**)form)[0x100 >> 2] == ADDR_ReturnTrue)
#define NOT_ACTOR(form) ((*(UInt32**)form)[0x100 >> 2] != ADDR_ReturnTrue)
#define IS_PROJECTILE(form) ((*(UInt32**)form)[0x224 >> 2] == ADDR_ReturnTrue)
#define IS_NODE(object) ((*(UInt32**)object)[0xC >> 2] == ADDR_ReturnThis)
#define IS_GEOMETRY(object) ((*(UInt32**)object)[0x18 >> 2] == ADDR_ReturnThis2)

//#if UnuseInThisPlugin 
typedef InventoryRef* (*_InventoryRefGetForID)(UInt32 refID);
extern _InventoryRefGetForID InventoryRefGetForID;

typedef TESObjectREFR* (__stdcall* _InventoryRefCreate)(TESObjectREFR* container, TESForm* itemForm, SInt32 countDelta, ExtraDataList* xData);
extern _InventoryRefCreate InventoryRefCreate;
//#endif

class ExtraStartingPosition : public BSExtraData
{
public:
	NiVector3	posVector;	// 0C
	NiVector3	rotVector;	// 18
};

/*
ArmorClass enum
Light, Medium, Heavy Armor
*/
enum class ArmorClass {
	None,Light,Medium,Heavy
};

enum class WeapClass {
	None, OneHandGun, TwoHandGun, TwoHandHeavyGun,
	OneHandMelee,TwoHandMelee,Throwable
};

// copy from xnvse
class MatchBySlot : public FormMatcher
{
	UInt32 m_slotMask;
public:
	MatchBySlot(UInt32 slot) : m_slotMask(TESBipedModelForm::MaskForSlot(slot)) {}
	bool Matches(TESForm* pForm) const {
		UInt32 formMask = 0;
		if (pForm) {
			if (pForm->IsWeapon()) {
				formMask = TESBipedModelForm::eSlot_Weapon;
			}
			else {
				TESBipedModelForm* pBip = DYNAMIC_CAST(pForm, TESForm, TESBipedModelForm);
				if (pBip) {
					formMask = pBip->partMask;
				}
			}
		}
		return (formMask & m_slotMask) != 0;
	}
};


void InitInv(const NVSEDataInterface* nvse_data) {
	if (!nvse_data) return;
	InventoryRefGetForID = (_InventoryRefGetForID)nvse_data->GetFunc(NVSEDataInterface::kNVSEData_InventoryReferenceGetForRefID);
	InventoryRefCreate = (_InventoryRefCreate)nvse_data->GetFunc(NVSEDataInterface::kNVSEData_InventoryReferenceCreateEntry);
}


class BSTaskletData
{
public:
	virtual void	Destroy(bool doFree);
	virtual bool	Unk_01(void);	// Returns true
	virtual void	ExecuteQueuedTasks();
	virtual void	Unk_03(void);	// Does nothing

	UInt8		byte04;		// 04
	UInt8		pad05[3];	// 05
};

// 14
class BSWin32TaskletData : public BSTaskletData
{
public:
	UInt32		unk08;		// 08
	UInt8		byte0C;		// 0C
	UInt8		pad0D[3];	// 0D
	UInt32		unk10;		// 10
};

// 14
class BSTCommonLLMessageQueue
{
public:
	virtual void	Destroy(bool doFree);
	virtual void	Unk_01(void);
	virtual void	Unk_02(void);
	virtual void	Unk_03(void);
	virtual void	Unk_04(void);
	virtual void	Unk_05(void);
	virtual void	Unk_06(void);

	UInt32		unk04[4];	// 04
};

// 38
class MobileObjectTaskletData : public BSWin32TaskletData
{
public:
	virtual void	EnterCriticalSections(UInt32 unused);
	virtual void	ExecuteTask(TESObjectREFR* execOn);
	virtual void	LeaveCriticalSections(UInt32 unused);

	UInt32					unk14[2];		// 14
	BSTCommonLLMessageQueue	messageQueue;	// 1C
	UInt32					unk30[2];		// 30
};

// 40
class DetectionTaskData : public MobileObjectTaskletData
{
public:
	float			flt38;		// 38
	UInt32			unk3C;		// 3C
};

// 38
class AnimationTaskData : public MobileObjectTaskletData
{
public:
};

// 44
class PackageUpdateTaskData : public MobileObjectTaskletData
{
public:
	UInt32			unk38[3];	// 38
};

// 3C
class ActorUpdateTaskData : public MobileObjectTaskletData
{
public:
	UInt32			unk38;		// 38
};

// 40
class ActorsScriptTaskData : public MobileObjectTaskletData
{
public:
	UInt32			unk38[2];	// 38
};

// 3C
class MovementTaskData : public MobileObjectTaskletData
{
public:
	UInt32			unk38;		// 38
};

struct LipTask;

// Straight from JIP. 
struct ProcessManager
{
	UInt32									unk000;				// 000
	NiTArray<MobileObject*>					objects;			// 004
	UInt32									beginOffsets[4];	// 014	0: High, 1: Mid-High, 2: Mid-Low, 3: Low
	UInt32									endOffsets[4];		// 024
	UInt32									offsets034[4];		// 034	Same as beginOffsets
	UInt32									unk044[5];			// 044
	tList<void>								list058;			// 058
	tList<void>						tempEffects;		// 060
	tList<void>						muzzFlashList;		// 068
	tList<Projectile*>								list070;			// 070
	tList<void>								list078;			// 078
	tList<Actor>							highActors;			// 080
	Actor* nearestActors[50];	// 088
	UInt32									nearestActorsCount;	// 150
	float									commentPCActionsTimer;	// 154
	float									commentPCKnockingTimer;	// 158
	UInt8									pcInRadiation1;		// 15C
	UInt8									pcInRadiation2;		// 15D
	UInt8									pad15E[2];			// 15E
	DetectionTaskData						detectionTasks;		// 160
	AnimationTaskData						animationTasks;		// 1A0
	PackageUpdateTaskData					packageUpdTasks;	// 1D8
	ActorUpdateTaskData						actorUpdTasks;		// 21C
	ActorsScriptTaskData					actorScriptTasks;	// 258
	MovementTaskData						movementTasks;		// 298
	UInt32									unk2D4[0x4023];		// 2D4
	//LockFreeMap<MobileObject*, LipTask*>	lipTasksMap;		// 10360
	UInt64									lipTasksMapPad[8];	// 10360
	UInt8									toggleAI;			// 103A0
	UInt8									toggleDetection;	// 103A1
	UInt8									toggleDetectionStats;	// 103A2
	UInt8									byte103A3;			// 103A3
	UInt32									detectionStats;		// 103A4
	UInt8									toggleHighProcess;	// 103A8
	UInt8									toggleLowProcess;	// 103A9
	UInt8									toggleMidHighProcess;	// 103AA
	UInt8									toggleMidLowProcess;	// 103AB
	UInt8									toggleAISchedules;	// 103AC
	UInt8									showSubtitle;		// 103AD
	UInt8									byte103AE;			// 103AE
	UInt8									byte103AF;			// 103AF
	UInt32									numHighActors;		// 103B0
	float									crimeUpdateTimer;	// 103B4
	UInt32									crimeNumber;		// 103B8
	float									removeDeadActorsTime;	// 103BC
	UInt32									unk103C0[3];		// 103C0
	__forceinline static ProcessManager* Get() { return (ProcessManager*)0x11E0E80; }
};


class Projectile;
class Explosion;
class DetectionData
{
public:
	Actor* actor;				// 00
	UInt8		detectionLevel;		// 04
	UInt8		byte05;				// 05
	UInt8		byte06;				// 06
	UInt8		byte07;				// 07
	SInt32		detectionValue;		// 08
	NiPoint3	detectedLocation;	// 0C
	float		fTimeStamp;			// 18
	UInt8		forceResetCombatLOSBuffer;	// 1C
	UInt8		byte1D;				// 1D
	bool		inLOS;				// 1E
	UInt8		byte1F;				// 1F
	UInt32		unk20;				// 20
};


__declspec(naked) bhkCharacterController* TESObjectREFR::GetCharacterController() const
{
	__asm
	{
		mov		eax, [ecx]
		cmp		dword ptr[eax + 0x100], ADDR_ReturnTrue
		jnz		retnNULL
		mov		ecx, [ecx + 0x68]
		test	ecx, ecx
		jz		retnNULL
		cmp		byte ptr[ecx + 0x28], 1
		ja		retnNULL
		mov		eax, [ecx + 0x138]
		retn
		retnNULL :
		xor eax, eax
			retn
	}
}

class hkpRigidBody;

// 104
class Explosion : public MobileObject
{
public:
	virtual void	CheckInit3D();

	float				unk088;			// 088	init'd to 0
	float				unk08C;			// 08C	init'd to 3.0
	float				unk090;			// 090	init'd to 0
	float				radius;			// 094
	float				ISradius;		// 098
	float				unk09C;			// 09C	init'd to 1.0
	NiRefObject* object0A0;		// 0A0
	tList<void>			list0A4;		// 0A4
	Sound				sound0AC;		// 0AC
	Sound				sound0B8;		// 0B8
	void* pointLight;	// 0C4 NiPointLight
	TESObjectREFR* sourceRef;		// 0C8
	UInt32				unk0CC;			// 0CC
	NiRefObject* object0D0;		// 0D0
	UInt8				byte0D4;		// 0D4
	UInt8				byte0D5;		// 0D5
	UInt8				byte0D6;		// 0D6
	UInt8				byte0D7;		// 0D7
	UInt8				byte0D8;		// 0D8
	UInt8				byte0D9;		// 0D9
	UInt8				pad0DA[2];		// 0DA
	void*				caster;		// 0DC NonActorMagicCaster
	UInt32				unk0E0;			// 0E0
	UInt32				unk0E4;			// 0E4
	NiVector3			vec0E8;			// 0E8
	NiVector3			vec0F4;			// 0F4
	float				hitDamage;		// 100	init'd to -1.0
};
static_assert(sizeof(Explosion) == 0x104);

	class Projectile : public MobileObject
	{
	public:
		Projectile();
		~Projectile();

		enum
		{
			kProjType_Beam = 1,
			kProjType_Flame,
			kProjType_Grenade,
			kProjType_Missile,
			kProjType_ContinuousBeam
		};

		/*304*/virtual UInt32	GetProjectileType();
		/*308*/virtual void	Do3DLoaded();
		/*30C*/virtual void	HandleTracer();
		/*310*/virtual void	UpdateProjectile(float timePassed);
		/*314*/virtual bool	ProcessImpact();
		/*318*/virtual bool	IsProximityTriggered();
		/*31C*/virtual void	Unk_C7(void);
		/*320*/virtual bool	DisarmPlacedExplosives(TESObjectREFR* refr, bool bSilent);
		/*324*/virtual void	Unk_C9(void);
		/*328*/virtual void	Unk_CA(void);
		/*32C*/virtual void	Unk_CB(void);

		enum
		{
			/*0*/kProjFlag_IsHitScan = 0x1,
			/*1*/kProjFlag_HitScanNonTracer = 0x2,
			/*2*/kProjFlag_IsStuck = 0x4,
			/*3*/kProjFlag_Bit03Unk_HitScan = 0x8,
			/*4*/kProjFlag_IsTracer = 0x10,
			/*5*/kProjFlag_Fades = 0x20,
			/*6*/kProjFlag_HasGravity = 0x40,
			/*7*/kProjFlag_IsAddedToProcessHandlerArrowsList = 0x80,
			/*8*/kProjFlag_Bit08Unk = 0x100,
			/*9*/kProjFlag_MineDisarmed = 0x200,
			/*10*/kProjFlag_IsPickpocketLiveExplosive = 0x400,
			/*11*/kProjFlag_Bit0BUnk = 0x800,
			/*12*/kProjFlag_AlwaysHit = 0x1000,
			/*13*/kProjFlag_Bit0DUnk_HitScan = 0x2000,
			/*14*/kProjFlag_MineIgnoresPlayer = 0x4000,
			/*15*/kProjFlag_Bit0FUnk_NonHitScan = 0x8000,		// Don't apply source-weapon's damage upon impact
			/*16*/kProjFlag_Bit10Unk = 0x10000,
			/*17*/kProjFlag_IgnoreGravity = 0x20000,
			/*18*/kProjFlag_Bit12Unk = 0x40000,
			/*19*/kProjFlag_SourceActorInCombat = 0x80000,
			/*20*/kProjFlag_Bit14Unk = 0x100000
		};

		struct ImpactData
		{
			TESObjectREFR* refr;			// 00
			NiVector3		pos;			// Gotten from JIP 04
			NiVector3		rot;			// Gotten from JIP 10
			hkpRigidBody* rigidBody;				// 1C type - hkpRigidBody 
			UInt32			materialType;	// 20, gotten from JIP
			SInt32			hitLocation;	// 24
			UInt32			unk28;			// 28
			UInt32			unk2C;			// 2C
		};

		struct Struct128
		{
			UInt32			unk00;
			UInt8			byte04;
			UInt8			pad05[3];
			UInt32			status;		//	0 - Not triggered, 1 - Triggered, 2 - Disarmed
		};

		tList<ImpactData>	impactDataList;	// 088
		UInt8				hasImpacted;	// 090
		UInt8				pad091[3];		// 091
		NiTransform			transform;		// 094
		UInt32				projFlags;		// 0C8
		float				power;			// 0CC
		float				speedMult;		// 0D0
		float				BaseProjRange;	// 0D4
		float				lifeTime;		// 0D8
		float				hitDamage;		// 0DC
		float				alpha;			// 0E0
		float				detonationTime;	// 0E4
		float				blinkTimer;		// 0E8
		float				angMomentumZ;	// 0EC
		float				angMomentumX;	// 0F0
		float				wpnHealthPerc;	// 0F4
		TESObjectWEAP* sourceWeap;			// 0F8
		TESObjectREFR* sourceRef;			// 0FC
		TESObjectREFR* liveGrenadeTarget;	// 100
		NiVector3			UnitVector;			// 104
		float				distTravelled;		// 110
		NiRefObject* object114;		// 114
		UInt8				byte118;		// 118
		UInt8				pad119[3];		// 119
		NiNode* node11C;					// 11C
		UInt32				unk120;			// 120
		float				decalSize;		// 124
		Sound			passPlayerSound;	// 128
		Sound			countDownSound;		// 134
		UInt32				unk140;			// 140
		ContChangesEntry* rockItLauncherEntry;			// 144
		UInt8				hasPlayedPassPlayerSound;		// 148
		bool				hasSplitBeams;			// 149	JIP only!
		UInt8				numProjectiles;			// 14A	JIP only!
		UInt8				weaponModFlags_TWEAKS;	// 14B
		float				range;			// 14C

		// Copied from JIP's Projectile::GetData
		[[nodiscard]] TESObjectREFR* GetImpactRef() const;

		[[nodiscard]] ImpactData* GetImpactData() const;
		Projectile::ImpactData* GetImpactDataAlt() const;
		TESObjectREFR* GetImpactRefAlt() const;
		// Copied from Tweaks
		static Projectile* __cdecl Spawn(BGSProjectile* projectile, Actor* source, CombatController* combatCtrl, TESObjectWEAP* sourceWeap,
			NiVector3 pos, float rotZ, float rotX, float angularMomentumZ, float angularMomentumX, TESObjectCELL* cell,
			bool ignoreGravity = false);

	};
	STATIC_ASSERT(sizeof(Projectile) == 0x150);

	// 154
	class BeamProjectile : public Projectile
	{
	public:
		BeamProjectile();
		~BeamProjectile();

		NiRefObject* object150;		// 150
	};

	// 158
	class ContinuousBeamProjectile : public Projectile
	{
	public:
		ContinuousBeamProjectile();
		~ContinuousBeamProjectile();

		NiRefObject* object150;		// 150
		UInt32			unk154;			// 154
	};

	// 158
	class FlameProjectile : public Projectile
	{
	public:
		FlameProjectile();
		~FlameProjectile();

		virtual void	Unk_CC(void);

		float		flt150;		// 150
		float		flt154;		// 154
	};

	// 154
	class GrenadeProjectile : public Projectile
	{
	public:
		GrenadeProjectile();
		~GrenadeProjectile();

		virtual void	Unk_CC(void);

		UInt8		byte150;		// 150
		UInt8		pad151[3];		// 151
	};

	// 160
	class MissileProjectile : public Projectile
	{
	public:
		MissileProjectile();
		~MissileProjectile();

		enum ImpactResult : UInt32
		{
			IR_NONE = 0x0,
			IR_DESTROY = 0x1,
			IR_BOUNCE = 0x2,
			IR_IMPALE = 0x3,
			IR_STICK = 0x4,
			IR_COUNT = 0x5
		};

		ImpactResult eImpactResult;
		UInt8 byte154;				//DealStickAndImpaleResult?
		UInt8 pad155[3];
		float lifetime;
		float flt15C;
	};

	
	// From JIP
	TESObjectREFR* Projectile::GetImpactRef() const
	{
		if (hasImpacted)
		{
			const ListNode<ImpactData>* traverse = impactDataList.Head();
			if (!traverse) return nullptr;
			do
			{
				ImpactData* impactData = traverse->data;
				if (impactData && impactData->refr)
				{
					return impactData->refr;
				}
			} while (traverse = traverse->next);
		}
		return nullptr;
	}

	inline Projectile::ImpactData* Projectile::GetImpactData() const
	{
		if (hasImpacted)
		{
			const ListNode<ImpactData>* traverse = impactDataList.Head();
			if (!traverse) return nullptr;
			do
			{
				ImpactData* impactData = traverse->data;
				if (impactData && impactData->refr)
				{
					return impactData;
				}
			} while (traverse = traverse->next);
		}
		return nullptr;
	}

	inline Projectile::ImpactData* Projectile::GetImpactDataAlt() const
	{
		const ListNode<ImpactData>* traverse = impactDataList.Head();
		if (!traverse) return nullptr;
		do
		{
			ImpactData* impactData = traverse->data;
			if (impactData)
			{
				return impactData;
			}
		} while (traverse = traverse->next);
		return nullptr;
	}

	inline TESObjectREFR* Projectile::GetImpactRefAlt() const
	{
		const ListNode<ImpactData>* traverse = impactDataList.Head();
		if (!traverse) return nullptr;
		do
		{
			ImpactData* impactData = traverse->data;
			if (impactData && impactData->refr)
			{
				return impactData->refr;
			}
		} while (traverse = traverse->next);
		return nullptr;
	}


// From Tweaks
Projectile* __cdecl Projectile::Spawn(BGSProjectile* projectile, Actor* source, CombatController* combatCtrl, TESObjectWEAP* sourceWeap,
	NiVector3 pos, float rotZ, float rotX, float angularMomentumZ, float angularMomentumX, TESObjectCELL* cell, bool ignoreGravity)
{
	return CdeclCall<Projectile*>(0x9BCA60, projectile, source, combatCtrl, sourceWeap, pos, rotZ, rotX, 
		/* Projectile node for muzzle flash?*/ 0, 
		/* live grenade target_ should be a tesobjectrefr*/ 0, 
		/* Set Is Always Hit*/ 0, 
		ignoreGravity, angularMomentumZ, angularMomentumX, cell);
}

struct SpawnPJInfo {
	Actor* source;
	UINT32 DetectionDataToSrc_Maybe;
	float PosX;
	float PosY;
	float PosZ;
	char pad_14[8];
};
static_assert(sizeof(SpawnPJInfo) == 0x1Cu);

static __forceinline void Projectile_SetUNK120(Projectile* _this,SpawnPJInfo* _pjInfo) {
	
	ThisStdCall<void>(0x978ED0,_this,_pjInfo);
}

static __forceinline SpawnPJInfo* ToddHoward_Allocate_HisMama(UINT32 size) {
	gLog.Message("Allocate For SpawnPJInfo");
	return CdeclCall<SpawnPJInfo*>(0x401000, size);
}

static __forceinline SpawnPJInfo* ConstructSpawnPJSrcInfo(SpawnPJInfo* SpwanPJInfo,Actor* _source) {
	if (!SpwanPJInfo)
	{
		gLog.Message("TheSpwanPJInfo is nullptr");
		return nullptr;
	}
	gLog.Message("TheSpwanPJInfo is success to create");
	return ThisStdCall<SpawnPJInfo*>(0x66E010, SpwanPJInfo,_source);
}

static __forceinline void AddInfoToSpawnPJ(Projectile* _pj,Actor* _src) {
	auto* Info = ConstructSpawnPJSrcInfo(ToddHoward_Allocate_HisMama(0x1Cu),_src);
	if (!Info){
		gLog.Message("Info is nullptr,AddInfoToSpawnPJ Failed");
		return;
	}
	gLog.Message("Info is created,set unk120 for newproj");
	Projectile_SetUNK120(_pj,Info);
}

//#if UnuseInThisPlugin 
double __fastcall GetArmorEffectiveDX(TESObjectREFR* thisObj, UInt32 funcAddr)
{
	if (!thisObj) return 0;
	if NOT_TYPE(thisObj->baseForm, TESObjectARMO)
		return 0;
	if (InventoryRef* invRef = InventoryRefGetForID(thisObj->refID))
		return ThisStdCall<double>(funcAddr, invRef->data.entry, 0);
	ContChangesExtraList extendData{ &thisObj->extraDataList };
	ContChangesEntry tempEntry(&extendData, 1, thisObj->baseForm);
	return ThisStdCall<double>(funcAddr, &tempEntry, 0);
}


TESObjectREFR* __fastcall GetEquippedItemRef(Actor* actor, UInt32 slotIndex)
{
	ContChangesEntryList* entryList = actor->GetContainerChangesList();
	if (!entryList) return nullptr;
	UInt32 partMask = 1 << slotIndex;
	TESForm* item;
	ContChangesEntry* entry;
	ExtraDataList* xData;
	if (actor->GetRefNiNode())
	{
		if (slotIndex == 5)
		{
			entry = actor->baseProcess->GetWeaponInfo();
			if (entry && entry->extendData)
				return InventoryRefCreate(actor, entry->type, entry->countDelta, entry->extendData->GetFirstItem());
		}
		else if (BipedAnim* equipment = actor->GetBipedAnim())
		{
			BipedAnim::Data* slotData = equipment->slotData;
			for (UInt8 count = 20; count; count--, slotData++)
			{
				item = slotData->item;
				if (!item || NOT_TYPE(item, TESObjectARMO) || !(((TESObjectARMO*)item)->bipedModel.partMask & partMask))
					continue;
				if (!(entry = entryList->FindForItem(item)) || !(xData = entry->GetEquippedExtra()))
					break;
				return InventoryRefCreate(actor, item, entry->countDelta, xData);
			}
		}
	}
	else
	{
		auto listIter = entryList->Head();
		do
		{
			entry = listIter->data;
			if (!entry || !entry->extendData)
				continue;
			item = entry->type;
			if (slotIndex == 5)
			{
				if NOT_ID(item, TESObjectWEAP)
					continue;
			}
			else if (NOT_TYPE(item, TESObjectARMO) || !(((TESObjectARMO*)item)->bipedModel.partMask & partMask))
				continue;
			if (xData = entry->GetEquippedExtra())
				return InventoryRefCreate(actor, item, entry->countDelta, xData);
		} while (listIter = listIter->next);
	}
	return nullptr;
}


struct DTDR {
	float DT{0};
	float DR{0};

	DTDR() :DT(0),DR(0){}
	DTDR(float _dt,float _dr):DT(_dt),DR(_dr) {}
};

float __forceinline GetEqHelmetDT(Actor* _actor) {
	return GetArmorEffectiveDX(GetEquippedItemRef(_actor, 9), 0x4BE0B0);
}

float __forceinline GetEqHelmetDR(Actor* _actor) {
	return GetArmorEffectiveDX(GetEquippedItemRef(_actor, 9), 0x4BDF90);
}

float __forceinline GetEqArmorDT(Actor* _actor) {
	return GetArmorEffectiveDX(GetEquippedItemRef(_actor, 2), 0x4BE0B0);
}

float __forceinline GetEqArmorDR(Actor* _actor) {
	return GetArmorEffectiveDX(GetEquippedItemRef(_actor, 2), 0x4BDF90);
}

auto __forceinline GetEqHelmetDTDR(Actor* _actor) {
	return DTDR{ GetEqHelmetDT(_actor),GetEqHelmetDR(_actor)};
}

auto __forceinline GetEqArmorDTDR(Actor* _actor) {
	return DTDR{ GetEqArmorDT(_actor),GetEqArmorDR(_actor) };
}

float GetP2PRayCastRange(const NiVector3& vec_a,const NiVector3& vec_b)
{
	gLog.FormattedMessage("vec_a X-%.2f,Y-%.2f,Z-%.2f", vec_a.x,vec_a.y,vec_a.z);
	gLog.FormattedMessage("vec_b X-%.2f,Y-%.2f,Z-%.2f", vec_b.x, vec_b.y, vec_b.z);
	SInt32 layerType = 6;
	NiMatrix33 rotMat{};
	gLog.Message("Begin to p2p casting");
	float length = rotMat.From2Points(vec_a.PS(), vec_b.PS());
	gLog.FormattedMessage("Form 2Points length %.2f",length);
	NiVector4 rcPos{};
	gLog.Message("Begin to RayCastCoords");
	/*if (rcPos.RayCastCoords(vec_a, rotMat + 1, length, layerType)) {
		gLog.Message("RayCastCoords Success");
		return Point3Distance(rcPos, vec_b);
	}*/
	return -1;
}
//#endif


struct _Sound
{
	UInt32		soundKey;	// 00
	UInt8		byte04;		// 04
	UInt8		pad05[3];	// 05
	UInt32		unk08;		// 08

	_Sound() : soundKey(0xFFFFFFFF), byte04(0), unk08(0) {}

	__forceinline void SetPos(const NiVector3& posVec)
	{
		ThisStdCall(0xAD8B60, this, posVec.x, posVec.y, posVec.z);
	}
	__forceinline void SetNiNode(NiNode* node)
	{
		ThisStdCall(0xAD8F20, this, node);
	}

	void SetVolume(float volume)
	{
		ThisStdCall(0xAD89E0, this, volume);
	}

	__forceinline void Play()
	{
		ThisStdCall(0xAD8830, this, 0);
	}

	static void PlayFile(const char* filePath, UInt32 flags, TESObjectREFR* ref);
	static void PlaySound3D(TESSound* soundForm, TESObjectREFR* refr, bool use_ref_ninode, TESObjectREFR* sound_target);
	static void PlayTESSound(TESSound* soundForm, UInt32 flags, TESObjectREFR* refr);
	static void PlayTESSound(TESSound* soundForm, UInt32 flags, TESObjectREFR* refr,const NiVector3& _pos);
	static void PlayTESSoundAtPos(TESSound* soundForm, UInt32 flags, const NiVector3& _pos);
	static void PlayTESSoundAtPos(TESSound* soundForm, const char* filePath, UInt32 flags, const NiVector3& _pos);
};

class BSWin32Audio {
public:
	/*00*/virtual void	Destroy(bool doFree);
	/*04*/virtual void	CreateAudioListener(HWND _window);
	/*08*/virtual void	Unk_02(void);
	/*0C*/virtual void	Unk_03(void);	// Does nothing
	/*10*/virtual void	Unk_04(void);	// Does nothing
	/*14*/virtual void* CreateGameSound(const char* filePath);
	/*18*/virtual void	InsertPathPrefix(char* filePath);	// Prefixes path with data\\sound\\ if fx\\ or song\\.
	/*1C*/virtual void	Unk_07(void);	// Does nothing

	char pad[0xA0];

	__forceinline static BSWin32Audio* Get() { return *(BSWin32Audio**)0x11F6D98; }
};
static_assert(sizeof(BSWin32Audio) == 0xA4);

class BSAudioManager
{
public:
	
	enum AudioFlags
	{
		/*00*/kAudioFlags_2D = 0x1,
		/*01*/kAudioFlags_3D = 0x2,
		/*02*/kAudioFlags_IsVoice = 0x4,
		/*03*/kAudioFlags_IsFootsteps = 0x8,
		/*04*/kAudioFlags_Loop = 0x10,
		/*05*/kAudioFlags_SystemSound = 0x20,
		/*06*/kAudioFlags_RandomFrequencyShift = 0x40,
		/*07*/kAudioFlags_80 = 0x80,
		/*08*/kAudioFlags_100 = 0x100,
		/*09*/kAudioFlags_IsMusic = 0x800,
		/*10*/kAudioFlags_RegionSound_MuteWhenSubmerged = 0x1000,
		/*11*/kAudioFlags_MaybeUnderwater = 0x2000,
		/*12*/kAudioFlags_4000 = 0x4000,
		/*13*/kAudioFlags_IsTemporary8000 = 0x8000,
		/*14*/kAudioFlags_DontCache = 0x10000,
		/*15*/kAudioFlags_20000 = 0x20000,
		/*16*/kAudioFlags_FirstPerson = 0x40000,
		/*17*/kAudioFlags_Modulated = 0x80000,
		/*18*/kAudioFlags_IsRadio = 0x100000,
		/*19*/kAudioFlags_IgnoreTimescale = 0x200000,
		/*20*/kAudioFlags_Radio400000 = 0x400000,
		/*21*/kAudioFlags_IsMusic2 = 0x800000,
		/*22*/kAudioFlags_1000000 = 0x1000000,
		/*23*/kAudioFlags_EnvelopeFast = 0x2000000,
		/*24*/kAudioFlags_EnvelopeSlow = 0x4000000,
		/*25*/kAudioFlags_2DRadius = 0x8000000,
		/*26*/kAudioFlags_20000000 = 0x20000000,
		/*27*/kAudioFlags_AnimationDriven = 0x40000000,
		/*28*/kAudioFlags_BeamEmitter = 0x10000000,
	};

	enum Volumes
	{
		kMaster = 0,
		kFoot,
		kVoice,
		kEffects,
		kMusic,
		kRadio,
		kVATSCamera,
	};

	virtual void				Destroy(bool doFree);

	char pad[0x184];

	__forceinline static BSAudioManager* Get() { return (BSAudioManager*)0x11F6EF0; }

	__forceinline void InitSoundEDID(_Sound& sound, const char* soundEDID, UInt32 flags)
	{
		ThisStdCall(0xAE5680, this, &sound, soundEDID, flags);
	}
	__forceinline void InitSoundPath(_Sound& sound, const char* filePath, UInt32 flags)
	{
		ThisStdCall(0xAE5A50, this, &sound, filePath, flags, nullptr);
	}
	__forceinline void InitSoundForm(_Sound& sound, UInt32 formRefID, UInt32 flags)
	{
		ThisStdCall(0xAE5870, this, &sound, formRefID, flags);
	}
};
static_assert(sizeof(BSAudioManager) == 0x188);

void _Sound::PlayFile(const char* filePath, UInt32 flags, TESObjectREFR* ref)
{
	NiNode* refrNode = ref->GetRefNiNode();
	if (!refrNode) return;
	_Sound sound;
	BSAudioManager::Get()->InitSoundPath(sound, filePath, flags);
	if (sound.soundKey != 0xFFFFFFFF)
	{
		sound.SetPos(*ref->GetPos());
		sound.SetNiNode(refrNode);
		sound.Play();
	}
}

void _Sound::PlaySound3D(TESSound* soundForm, TESObjectREFR* refr ,bool use_ref_ninode = false,TESObjectREFR* altref = nullptr) {
	if (!soundForm) {
		gLog.Message("soundForm invaild");
		return;
	}
	const char* filePath = soundForm->soundFile.path.m_data;
	if ( !filePath) {
		gLog.Message("refr dont have node or soundform dont have path");
		return;
	}
	
	_Sound sound;
	//BSAudioManager::Get()->InitSoundForm(sound, soundForm->refID, 0x4102);//8004102
	ThisStdCall(0xAD7480, BSWin32Audio::Get(), &sound, filePath, 0x4102, soundForm);
	if (sound.soundKey != 0xFFFFFFFF)
	{
		sound.SetPos(*refr->GetPos());
		NiNode* ref_node = nullptr;
		if (use_ref_ninode) {
			if (altref) {
				if (altref->GetRefNiNode()) ref_node = altref->GetRefNiNode();
			}
			else if (refr->GetRefNiNode()) ref_node = refr->GetRefNiNode();
		}
		else if (refr->GetRefNiNode()) ref_node = refr->GetRefNiNode();
		sound.SetNiNode(ref_node);
		sound.Play();
	}
}

void _Sound::PlayTESSound(TESSound* soundForm, UInt32 flags, TESObjectREFR* refr)
{
	if (!soundForm){
		gLog.Message("soundForm invaild");
		return;
	}
	const char* filePath = soundForm->soundFile.path.m_data;
	NiNode* refrNode = refr->GetRefNiNode();
	if (!refrNode || !filePath) { 
		gLog.Message("refr dont have node or soundform dont have path");
		return; 
	}
	_Sound sound;
	ThisStdCall(0xAD7480, BSWin32Audio::Get(), &sound, filePath, flags, soundForm);
	//BSAudioManager::Get()->InitSoundForm(sound, soundForm->refID, flags);
	if (sound.soundKey != 0xFFFFFFFF)
	{
		sound.SetPos(refrNode->m_transformWorld.translate);
		sound.SetNiNode(refrNode);
		sound.Play();
	}
}

inline void _Sound::PlayTESSound(TESSound* soundForm, UInt32 flags, TESObjectREFR* refr,const NiVector3& _pos)
{
	if (!soundForm) {
		gLog.Message("soundForm invaild");
		return;
	}
	const char* filePath = soundForm->soundFile.path.m_data;
	NiNode* refrNode = refr->GetRefNiNode();
	if (!refrNode || !filePath) {
		gLog.Message("refr dont have node or soundform dont have path");
		return;
	}
	_Sound sound;
	ThisStdCall(0xAD7480, BSWin32Audio::Get(), &sound, filePath, flags, soundForm);
	//BSAudioManager::Get()->InitSoundForm(sound, soundForm->refID, flags);
	if (sound.soundKey != 0xFFFFFFFF)
	{
		sound.SetPos(_pos);
		sound.SetNiNode(refrNode);
		sound.Play();
	}
}

inline void _Sound::PlayTESSoundAtPos(TESSound* soundForm, UInt32 flags, const NiVector3& _pos)
{
	if (!soundForm) {
		gLog.Message("soundForm invaild");
		return;
	}
	const char* filePath = soundForm->soundFile.path.m_data;
	if (!filePath) {
		gLog.Message("refr dont have node or soundform dont have path");
		return;
	}
	_Sound sound;
	ThisStdCall(0xAD7480, BSWin32Audio::Get(), &sound, filePath, flags, soundForm);
	//BSAudioManager::Get()->InitSoundForm(sound, soundForm->refID, flags);
	if (sound.soundKey != 0xFFFFFFFF)
	{
		sound.SetVolume(100.0);
		sound.SetPos(_pos);
		sound.SetNiNode(nullptr);
		sound.Play();
	}
}

inline void _Sound::PlayTESSoundAtPos(TESSound* soundForm, const char* filePath, UInt32 flags, const NiVector3& _pos)
{
	if (!soundForm) {
		gLog.Message("soundForm invaild");
		return;
	}
	if (!filePath) {
		gLog.Message("refr dont have node or input a invalid sound path");
		return;
	}
	_Sound sound;
	ThisStdCall(0xAD7480, BSWin32Audio::Get(), &sound, filePath, flags, soundForm);
	//BSAudioManager::Get()->InitSoundForm(sound, soundForm->refID, flags);
	if (sound.soundKey != 0xFFFFFFFF)
	{
		sound.SetVolume(100.0);
		sound.SetPos(_pos);
		sound.SetNiNode(nullptr);
		sound.Play();
	}
}


TESForm* TempCloneForm(TESForm* _to_clone, bool bPersist = false)
{
	TESForm* form = NULL;
	if (!_to_clone) return nullptr;

	TESForm* clonedForm = _to_clone->CloneForm(bPersist);
	if (clonedForm) return clonedForm;

	return nullptr;
}
static TESSound* clonded_sound = nullptr;
// 0x21f - FSTSnow
static TESSound* GetClonedTESSoundForRico(UINT32 form_id) {
	if ( clonded_sound ){
		if (IS_TYPE(clonded_sound, TESSound)) return clonded_sound;
	}
	
	gLog.Message("rico_sound is none,created it");
	TESForm* to_clone = LookupFormByID(form_id);
	if (!to_clone) { 
		gLog.Message("Run LookupFormByID For FSTSnow faild"); 
		return nullptr;
	}
	else if (!IS_TYPE(to_clone, TESSound)) {
		gLog.Message("Run LookupFormByID For FSTSnow success,but form is not TESSound");
		return nullptr;
	}
	clonded_sound = (TESSound*)TempCloneForm(to_clone);
	
	if (!clonded_sound){
		gLog.Message("Run TempCloneForm For clonded_sound faild");
		return nullptr;
	}
	else if (!IS_TYPE(to_clone, TESSound)) {
		gLog.Message("Run TempCloneForm For clonded_sound success,but form is not TESSound");
		return nullptr;
	}
	clonded_sound->soundFile.path.Set( R"(fx\RicochetSound\)" );
	clonded_sound->soundFlags |= TESSound::kFlag_360LFE;
	//clonded_sound->soundFlags |= TESSound::kFlag_2DRadius;
	gLog.FormattedMessage("original mindis %u,maxdis %u", clonded_sound->minAttenuationDist,clonded_sound->maxAttenuationDist);
	clonded_sound->minAttenuationDist = 128;
	clonded_sound->maxAttenuationDist = 32;
	gLog.Message("clonded_sound has created");
	return clonded_sound;
}

// 0x21f - FSTSnow
static TESSound* GetNewTESSound( OUT TESSound* _new_sound, IN const char* _sound_path ,UINT32 cloned_form_id = 0x21f) {
	if (_new_sound) {
		if (IS_TYPE(_new_sound, TESSound)) return _new_sound;
	}

	gLog.Message("rico_sound is none,created it");
	TESForm* to_clone = LookupFormByID(cloned_form_id);
	if (!to_clone) {
		gLog.Message("Run LookupFormByID For FSTSnow faild");
		return nullptr;
	}
	else if (!IS_TYPE(to_clone, TESSound)) {
		gLog.Message("Run LookupFormByID For FSTSnow success,but form is not TESSound");
		return nullptr;
	}
	_new_sound = (TESSound*)TempCloneForm(to_clone);

	if (!_new_sound) {
		gLog.Message("Run TempCloneForm For clonded_sound faild");
		return nullptr;
	}
	else if (!IS_TYPE(to_clone, TESSound)) {
		gLog.Message("Run TempCloneForm For clonded_sound success,but form is not TESSound");
		return nullptr;
	}
	_new_sound->soundFile.path.Set(R"(fx\RicochetSound\)");
	_new_sound->soundFlags |= TESSound::kFlag_360LFE;
	//clonded_sound->soundFlags |= TESSound::kFlag_2DRadius;
	gLog.FormattedMessage("original mindis %u,maxdis %u", _new_sound->minAttenuationDist, _new_sound->maxAttenuationDist);
	_new_sound->minAttenuationDist = 128;
	_new_sound->maxAttenuationDist = 32;
	gLog.Message("clonded_sound has created");
	return _new_sound;
}

static __forceinline void initSingleton() {
	g_TES = TES::GetSingleton();
	g_thePlayer = PlayerCharacter::GetSingleton();
	g_gridCellArray = g_TES->gridCellArray;
}

