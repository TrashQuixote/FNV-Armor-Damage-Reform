#pragma once
#include "Gathering_Code.h"
#include "Gathering_Node.h"

constexpr bool log_enable = false;

#define __WriteLog(log_text, ...) if constexpr(log_enable)\
                            gLog.FormattedMessage(log_text, __VA_ARGS__);

#define __WriteLog2(log_enable_flag ,log_text, ...) if constexpr(log_enable_flag)\
                            gLog.FormattedMessage(log_text, __VA_ARGS__);

#define __WriteLogCond(cond ,log_text, ...) if (cond)\
                            gLog.FormattedMessage(log_text, __VA_ARGS__);

#define __WriteLogCond2(log_enable_flag, cond ,log_text, ...) if constexpr(log_enable_flag)\
                                                                if (cond)\
                                                                    gLog.FormattedMessage(log_text, __VA_ARGS__);


// convert the 'user radius' to 'game radius' ( 0 to 360) => ( 0 to 180 U -180 to 0)
#define Radiu_Z_User_to_Game(x) x = (std::fabs(x) > PI ? ( (Double_PI - std::fabs(x)) * (-sgn(x))) : x) 

// convert the 'game radius' to 'user radius' ( 0 to 360) => ( 0 to 180 U -180 to 0)
#define Radiu_Z_Game_to_User(x) x = ( x < 0 ? (Double_PI - x) : x)

template<typename TYPE>
struct OptFlag{
    UINT32 flag{};
    OptFlag() = default;
    bool IsFlagOn(TYPE _flag) const {
        return ((flag & static_cast<UINT32>(_flag)) != 0);
    }

    void SetFlagOn(TYPE _flag) {
        flag |= static_cast<UINT32>(_flag);
    }

    void SetFlagOff(TYPE _flag) {
        flag &= (~static_cast<UINT32>(_flag));
    }
};

template<typename T>
constexpr int sgn(const T& a, const T& b) noexcept {
    return (a > b) - (a < b);
}

template<typename T>
constexpr int sgn(const T& a) noexcept {
    return sgn(a, T(0));
}

/*
===== AngleCompensation =====
deal with the situation when the rot > 3.14
*/
#define AngleCompensationZ(z) z -= (Double_PI * (-sgn(z))) 

#define AngleCompensationX(x) x = (PI - std::fabs(x)) * sgn(x)

#define PC_Ref PlayerCharacter::GetSingleton()
#define Ut_Square(x) ( (x) * (x) )
constexpr UINT32 cdecl_loadTempEffectParticle = 0x6890B0;
constexpr UINT32 cdecl_getSubNodeByNameAddr = 0x4AAE30;
constexpr UINT32 thiscall_isActorAiming = 0x8BBC10;
constexpr UINT32 cdecl_isISAnimation = 0x5F2750;
constexpr UINT32 cdecl_cdBody_UnkGetNode = 0x4B5820;
constexpr UINT32 cdecl_GetGlobalTimeMult = 0x716440;
constexpr UINT32 cdecl_LookupByEditorID = 0x483A00;
float constexpr RdToAg = 57.29578018;
float constexpr PI = 3.1415926;
float constexpr Double_PI = 6.2831852;
float constexpr Half_PI = 1.5707963;




static __forceinline TESForm* LookupFromInMod(const char* modName, UINT32 formIDWithoutIndex) {
    const ModInfo* _modInfo = DataHandler::Singleton()->LookupModByName(modName);
    if (UInt32 _modIndex = _modInfo->modIndex; _modIndex != 0xFF)
    {
        _modIndex &= 0x000000FF;
        _modIndex <<= 24;
        UINT32 formID = _modIndex + formIDWithoutIndex;
        // todo...
    }
    return nullptr;
}

static __forceinline TESForm* LookupByEditorID(const char* editorID) {
    TESForm* form = CdeclCall<TESForm*>(cdecl_LookupByEditorID, editorID);
    return form;
}

/*
*   WeaponGetModEffectValue
*    which -> 1, 2, 3
*/
static __forceinline float WeaponGetModEffectValue(TESObjectWEAP* _weap, UINT8 which, bool _getEffect1 = true) {
    return _getEffect1 ? _weap->GetItemModValue1(which) : _weap->GetItemModValue2(which);
}
/*
* which -> 1, 2, 3
kWeaponModEffect_None = 0,
kWeaponModEffect_IncreaseDamage,
kWeaponModEffect_IncreaseClipCapacity,
kWeaponModEffect_DecreaseSpread,
kWeaponModEffect_DecreaseWeight,
kWeaponModEffect_Unused05,
kWeaponModEffect_RegenerateAmmo,
kWeaponModEffect_DecreaseEquipTime,
kWeaponModEffect_IncreaseRateOfFire,		// 8
kWeaponModEffect_IncreaseProjectileSpeed,
kWeaponModEffect_IncreaseMaxCondition,
kWeaponModEffect_Silence,
kWeaponModEffect_SplitBeam,
kWeaponModEffect_VATSBonus,
kWeaponModEffect_IncreaseZoom,				// 14
*/
static __forceinline UINT32 GetWeaponItemModEffect(TESObjectWEAP* _weap,UINT32 which) {
   return _weap->GetItemModEffect(which);
}
/*
    Get Mod Slot
    Will return a value which greater 2 when failed to find
*/
static __forceinline UINT8 GetModIndexOfWeaponItemEffect(TESObjectWEAP* _weap, UINT32 _kWeaponModEffect) {
    UINT8 _idx = 0;
    for (UINT32 _effectType : _weap->effectMods)
    {
        if (_effectType == _kWeaponModEffect)
        {
            return _idx;
        }
        _idx++;
    }
    return _idx;
}

static __forceinline ActorValueCode GetWeaponSkill(const TESObjectWEAP* _weap) {
    return static_cast<ActorValueCode>(_weap->weaponSkill);
}

static __forceinline float GetGlobalTimeMult() {
    return CdeclCall<float>(cdecl_GetGlobalTimeMult);
}

static __forceinline NiNode* GetActorPJNode(Actor* _actor) {
    if (_actor && _actor->baseProcess && !_actor->baseProcess->processLevel) return _actor->baseProcess->GetProjectileNode();
    return nullptr;
}

template <typename T>
T RoughClamp(T val, T min, T max) {
    T temp = (val < min ? min : val);
    return (temp > max ? max : val);
}

template <typename T>
T MinClamp(T val, T min) {
    return ((val < min) ? min : val);
}

template <typename T>
T MaxClamp(T val, T max) {
    return ((val > max) ? max : val);
}
/*
* Below Function Not Utility Functions
* No Range Check. 
* No Clamp
* No Slope Control
* Ensure argument is in (0,1) range so that it can return a value in (0, 1)
*/
static __forceinline float RoughSmoothStep(float t) {
    return t * t * (3 - 2 * t);
}

static __forceinline float RoughSlowDown(float t) {
    return (t - 1) * ( t - 1 );
}


static __forceinline float Remap(float value,float bottom,float top,float remapBottom,float remapTop) {
    float _ratio = value / (top - bottom);
    return remapBottom + ((remapTop - remapBottom) * _ratio);
}


/*
* ¡ü Not Utility Functions
*/

/* 
This enum is differentiate from hit location of game
Aiming on making geting hit-location more convenience with GetHitLimbLocation function
*/
enum HitLocation {
    HL_None,
    Torso,
    Head,
    Arm,
    Leg,
    Weapon
};

enum class RoughHitLocation {
    RHL_None,
    RHL_ExplosionHit,
    RHL_Torso,
    RHL_Head,
    RHL_Weapon
};


static RoughHitLocation GetRoughHitLocation(SInt32 hit_location) {
    switch (hit_location)
    {
    case -1:
        return RoughHitLocation::RHL_ExplosionHit;
        // head
    case 1:
    case 2:
    case 13:
        return RoughHitLocation::RHL_Head;
        // torso
    case 0:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
        return RoughHitLocation::RHL_Torso;
    case 14:
        return RoughHitLocation::RHL_Weapon;
    default:
        return RoughHitLocation::RHL_None;
    }
    return  RoughHitLocation::RHL_None;
}



/*
GetHitLimbLocation
Return is a custom enum which is differentiate from hit location of game
Aiming on making geting hit-location more convenience 
*/
static HitLocation GetHitLimbLocation(SInt32 hit_location) {
    switch (hit_location)
    {
    case 0:
        return HitLocation::Torso;
    case 1:
    case 2:
    case 13:
        return HitLocation::Head;
    case 3:
    case 4:
    case 5:
    case 6:
        return HitLocation::Arm;
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
        return HitLocation::Leg;
    default:
        return HitLocation::HL_None;
    }
}



static __forceinline NiVector4 getWorldRotationAlt(NiAVObject* niBlock,UInt32 getMode) {
    NiVector4 rot;
    __asm
    {
        mov		ecx, niBlock
        lea		eax, [ecx + 0x68]
        add		ecx, 0x34
        test	getMode, 1
        cmovnz	ecx, eax
        mov		eax, NiMatrix33::ToEulerPRY
        mov		edx, NiMatrix33::ToEulerPRYInv
        test	getMode, 2
        cmovnz	eax, edx
        call	eax
        mulps	xmm0, PS_V3_180dPI
        movups	rot, xmm0
    }
    return NiVector4{rot.x,rot.y,rot.z,rot.w};
}

static __forceinline bool ProjectileIsExplosion(Projectile* _pj) {
    if (const BGSProjectile* basepj = (BGSProjectile*)_pj->baseForm ) {// Check type and explosion flag
        return ((basepj->projFlags & 2) != 0 && (basepj->type & 1) != 0);
    }
    return false;
}

// Effects\ImpactBallisticBlood01.NIF
// DEAL IMPACT -> _unknown == 7, _ukNode == nullptr
static __forceinline UINT32 LoadTempEffectParticle(TESObjectCELL* _cell, float _lifeTime, const char* _model_path,
//static __forceinline void LoadTempEffectParticle(TESObjectCELL* _cell, float _lifeTime, const char* _model_path,
                                                const NiVector3& _impactRot, const NiVector3& _impactPos,float scale,
                                                int _unknown,const NiNode* _ukNode) {

    return CdeclCall<UINT32>(cdecl_loadTempEffectParticle, _cell, _lifeTime, _model_path, _impactRot, _impactPos, scale, _unknown, _ukNode);
    
}

//static __forceinline void TESObjectCELL_AddDecal(TESObjectCELL* _cell, BGSImpactData* _impactData, const NiVector3& _pos, const NiVector3& _rot) {
static __forceinline void TESObjectCELL_AddDecal(TESObjectCELL* _cell, Decal* _decal, const NiVector3& _pos, const NiVector3& _rot) {
    Decal l_decal{ *_decal };
    l_decal.worldPos = _pos;
    NiVector3 _nRot{_rot};
    _nRot.Normalize();
    l_decal.rotation = _nRot;
}

static __forceinline NiAVObject* getActorProjectileNode(Actor* _actor) {
    return ThisStdCall<NiAVObject*>(0x8BE0A0,_actor);
}

static __forceinline bool GetPCUsingScope() {
    return HUDMainMenu::Get()->isUsingScope ? true : false;
}

static __forceinline auto printWorldRotation(const NiMatrix33& _this) {
    float roll = _this.ExtractRoll();
    float yaw = _this.ExtractYaw();
    float pitch = _this.ExtractPitch();
    __WriteLog("pitch %.5f,roll %.5f,yaw %.5f", pitch * RdToAg, roll * RdToAg, yaw * RdToAg);
}

static __forceinline auto printWorldRotation(NiNode* _this) {
    printWorldRotation(_this->m_transformWorld.rotate);
}

static __forceinline bool isActorAiming(Actor* _act) {
    return ThisStdCall<bool>(thiscall_isActorAiming,_act);
}

static __forceinline bool SetStartingPos(TESObjectREFR* _ref,const NiVector3& _startPos) {
    ExtraStartingPosition* start_pos = static_cast<ExtraStartingPosition*>(_ref->extraDataList.GetByType(kExtraData_StartingPosition));
    if (!start_pos) return false;
    start_pos->posVector.x = _startPos.x;
    start_pos->posVector.y = _startPos.y;
    start_pos->posVector.z = _startPos.z;
    return true;
}

static __forceinline bool PCHasBlock(const char* _blockName) {
    if (auto* _pc = PlayerCharacter::GetSingleton())
        if (auto* _1stNode = _pc->node1stPerson)
            if (auto* _sNode = _1stNode->GetBlock(_blockName))
                return true;
    return false;
}

static __forceinline bool IsUsingB42OpticsLens() {
    return PCHasBlock("OldIS");
}

static __forceinline float CookedAngleZ(float _rawZ) {
    if (_rawZ < -180)
        _rawZ += 360;
    else if (_rawZ > 180)
        _rawZ -= 360;
    return _rawZ;
}

static __forceinline bool PCHasSightingNode() {
    return PCHasBlock("##SightingNode");
}

static __forceinline float GetHeadingAngleX_PosBased(const NiVector3& _outPos, float _outRotX,const NiVector3& _toPos) {
    float opposite = _outPos.z - _toPos.z;
    float hypotenuse = Point3Distance(_outPos, _toPos);
    float fraction = opposite / hypotenuse;
    return (asin(fraction) - _outRotX) * Flt180dPI;
}

static __forceinline float GetHeadingAngleZ_PosBased (const NiVector3& _outPos, float _outRotZ, const NiVector3& _toPos)
{
    float result = (atan2(_toPos.x - _outPos.x, _toPos.y - _outPos.y) - _outRotZ) * RdToAg;
    if (result < -180)
        result += 360;
    else if (result > 180)
        result -= 360;
    return result;
}

static __forceinline float GetHeadingAngleZBetweenNode(const NiNode* _outNd,float _outRZ,const NiNode* _toNd) {
    return GetHeadingAngleZ_PosBased(_outNd->m_transformWorld.translate,_outRZ,_toNd->m_transformWorld.translate);
}

static __forceinline float GetHeadingAngleXBetweenNode(const NiNode* _outNd, float _outRX, const NiNode* _toNd) {
    return GetHeadingAngleZ_PosBased(_outNd->m_transformWorld.translate, _outRX, _toNd->m_transformWorld.translate);
}

// only use _proj to calc
static __forceinline float GetImpactPosHeadingAngleToProj_X(Projectile* _proj,float _rotX) {
    const auto& _proj_direction = _proj->UnitVector;
    const auto _proj_cur_pos = _proj->GetPos();
    NiVector3 offset_pos{ _proj_cur_pos->x - _proj_direction.x,_proj_cur_pos->y - _proj_direction.y,_proj_cur_pos->z - _proj_direction.z };

    return GetHeadingAngleX_PosBased(*_proj_cur_pos, _rotX, offset_pos);
}

static __forceinline float GetImpactPosHeadingAngleToProj_Z(Projectile* _proj,float _rotZ) {
    const auto _proj_direction = _proj->UnitVector;
    const auto _proj_cur_pos = _proj->GetPos();
    NiVector3 offset_pos{ _proj_cur_pos->x - _proj_direction.x,_proj_cur_pos->y - _proj_direction.y,_proj_cur_pos->z - _proj_direction.z };

    return GetHeadingAngleX_PosBased(*_proj_cur_pos, _rotZ, offset_pos);
}

namespace ActorDTDR_Details {
    struct CreatureDTDR {
        DTDR native_dtdr;

        float __forceinline GetNativeDT() const { return native_dtdr.DT; }
        float __forceinline GetNativeDR() const { return native_dtdr.DR; }
    };

    struct CharacterDTDR {
        DTDR armor_dtdr;
        DTDR helmet_dtdr;

        float __forceinline GetHelmetDT() const { return helmet_dtdr.DT; }
        float __forceinline GetHelmetDR() const { return helmet_dtdr.DR; }
        float __forceinline GetArmorDT() const { return armor_dtdr.DT; }
        float __forceinline GetArmorDR() const { return armor_dtdr.DR; }
        float __forceinline GetTotalDT() const { return armor_dtdr.DT + helmet_dtdr.DT; }
        float __forceinline GetTotalDR() const { return armor_dtdr.DR + helmet_dtdr.DR; }
    };
};
using CreatureDTDR = ActorDTDR_Details::CreatureDTDR;
using CharacterDTDR = ActorDTDR_Details::CharacterDTDR;
using ActorDTDR = std::variant<std::monostate, CharacterDTDR, CreatureDTDR>;

static __forceinline ActorDTDR GetCreatureDTDR(Actor* actor_ref) {
    return ActorDTDR{ CreatureDTDR { DTDR{ actor_ref->avOwner.GetActorValue(76),actor_ref->avOwner.GetActorValue(18) } } };
}

static __forceinline ActorDTDR GetCharacterDTDR(Actor* actor_ref) {
    return ActorDTDR{ CharacterDTDR{ GetEqArmorDTDR(actor_ref),GetEqHelmetDTDR(actor_ref) } };
}

static __forceinline auto GetActorDTDR(Actor* _actor) {
    if (_actor->IsCreature()) {			// character
        return GetCreatureDTDR(_actor);
    }
    else return GetCharacterDTDR(_actor);
}

/*
 0: Head
 1: Hair
 2: Upper body
 3: Left hand
 4: Right hand
 5: Weapon
 6: Pip boy
 7: Backpack
 8: Necklace
 9: Headband
10: Hat
11: Eyeglasses
12: Nosering
13: Earrings
14: Mask
15: Choker
16: Mouth object
17: Body addon 1
18: Body addon 2
19: Body addon 3
*/
enum class EquipSlotID {
    Head, Hair, UpperBody, LeftHand,RightHand,Weapon,Pipboy,
    Backpack,Necklace,Headband,Hat,Eyeglasses,Nosering,Earrings,
    Mask,Choker,MouthObject,BodyAddon1, BodyAddon2, BodyAddon3,None
};

static EquipSlotID RoughHitLocationToSlot(RoughHitLocation _rhl) { 
    switch (_rhl)
    {
    case RoughHitLocation::RHL_Torso:
        return EquipSlotID::UpperBody;
    case RoughHitLocation::RHL_Head:
        return EquipSlotID::Headband;
    case RoughHitLocation::RHL_Weapon:
        return EquipSlotID::Weapon;
    case RoughHitLocation::RHL_None:
    default:
        return EquipSlotID::None;
    }
}

static EquipData FindEquipped(TESObjectREFR* thisObj, FormMatcher& matcher) {
    ExtraContainerChanges* pContainerChanges = static_cast<ExtraContainerChanges*>(thisObj->extraDataList.GetByType(kExtraData_ContainerChanges));
    return (pContainerChanges) ? pContainerChanges->FindEquipped(matcher) : EquipData();
}

static TESForm* GetEquippedObject(Actor* _actor, UINT32 _slot_index) {
    MatchBySlot matcher(_slot_index);
    EquipData equipD = FindEquipped(_actor, matcher);
    TESForm* pFound = equipD.pForm;
    if (pFound) return pFound;
    return nullptr;
}

static float GetEquippedCurrentHealth(Actor* _actor, UINT32 _slot_index) {
    MatchBySlot matcher(_slot_index);
    EquipData equipD = FindEquipped(_actor, matcher);
    if (equipD.pForm)
    {
        //__WriteLog2(true," if (equipD.pForm) Pass")
        ExtraHealth* pXHealth = equipD.pExtraData ? (ExtraHealth*)equipD.pExtraData->GetByType(kExtraData_Health) : NULL;
        if (pXHealth)
        {
            //__WriteLog2(true, " if (pXHealth) Pass")
            return pXHealth->health;
        }
        else
        {
            //__WriteLog2(true, " if (pXHealth) Not Pass")
            TESHealthForm* pHealth = DYNAMIC_CAST(equipD.pForm, TESForm, TESHealthForm);
            if (pHealth)
                return pHealth->health;
        }
    }
    return 0.0f;
}

static bool AdjustHealth(TESHealthForm* pHealth, ExtraDataList* pExtraData, float nuHealth)
{
    ExtraHealth* pXHealth = (ExtraHealth*)pExtraData->GetByType(kExtraData_Health);
    if (nuHealth < 0) nuHealth = 0;
    if (nuHealth >= pHealth->health) {
        if (pXHealth) {
            pExtraData->Remove(pXHealth, true);
        }
    }
    else if (pXHealth) {
        pXHealth->health = nuHealth;
    }
    else {
        // need to create a new pXHealth
        pXHealth = ExtraHealth::Create();
        if (pXHealth) {
            pXHealth->health = nuHealth;
            pExtraData->Add(pXHealth);
        }
    }
    return true;
}

static bool SetEquippedCurrentHealth(Actor* _actor, UINT32 _slot_index,float _new_val)
{
    MatchBySlot matcher(_slot_index);
    EquipData equipD = FindEquipped(_actor, matcher);
    if (equipD.pForm && equipD.pExtraData) {
        TESHealthForm* pHealth = DYNAMIC_CAST(equipD.pForm, TESForm, TESHealthForm);
        if (pHealth) {
            return AdjustHealth(pHealth, equipD.pExtraData, _new_val);
        }
    }
    return false;
}

/*
* GetEquippedWeaponModFlags
* return bit flag: 0b000
* 0b000 - No Weapon Mod Equipped
* 0b001 - First Weapon Mod Equipped
* 0b011 - First Two Weapon Mode Equiped
* ....
*/
static UInt8 GetEquippedWeaponModFlags(TESObjectREFR* thisObj)
{
    MatchBySlot matcher(5);
    EquipData equipD = FindEquipped(thisObj, matcher);

    if (!equipD.pForm) return UInt8(0);
    if (!equipD.pExtraData) return UInt8(0);

    ExtraWeaponModFlags* pXWeaponModFlags = (ExtraWeaponModFlags*)equipD.pExtraData->GetByType(kExtraData_WeaponModFlags);
    if (pXWeaponModFlags)return pXWeaponModFlags->flags;
    else return UInt8(0);
}

/*
    GetEffectValueFromEquippedWeaponMod
    get effect value of weapon item mod from actor's current equipped weapon
*/
static float GetEffectValueFromEquippedWeaponMod(Actor* _actor, UINT32 _effectID) {
    if (auto* eqWeap = GetEquippedObject(_actor, 5); IS_TYPE(eqWeap,TESObjectWEAP)) {
        UINT8 flag = GetEquippedWeaponModFlags(_actor);
        UINT8 match_index = 4;
        UINT32 l_effectID = TESObjectWEAP::kWeaponModEffect_None;
        if ((flag & 0b001) != 0) {
            l_effectID = GetWeaponItemModEffect(static_cast<TESObjectWEAP*>(eqWeap), 1);
            match_index = (_effectID == l_effectID) ? 1 : 4;
        }
        else if ((flag & 0b010) != 0) {
            l_effectID = GetWeaponItemModEffect(static_cast<TESObjectWEAP*>(eqWeap), 2);
            match_index = (_effectID == l_effectID) ? 2 : 4;
        }
        else if ((flag & 0b100) != 0) {
            l_effectID = GetWeaponItemModEffect(static_cast<TESObjectWEAP*>(eqWeap), 3);
            match_index = (_effectID == l_effectID) ? 3 : 4;
        }

        return match_index < 4 ? WeaponGetModEffectValue(static_cast<TESObjectWEAP*>(eqWeap), match_index) : 0.0f;
    }
    return 0.0f;
}

/*
    GetEffectValueFromEquippedWeaponMod some bug for now
    care later
*/
static float GetEffectValueFromEquippedWeaponMod(Actor* _actor,TESObjectWEAP* _baseWeap, UINT32 _effectID) {
    UINT8 flag = GetEquippedWeaponModFlags(_actor);
    __WriteLog2(true,"[GetEffectValueFromEquippedWeaponMod]GetEquippedWeaponModFlags: %u", flag);
    UINT8 match_index = 4;
    UINT32 l_effectID = TESObjectWEAP::kWeaponModEffect_None;
    if ((flag & 0b001) != 0) {
        l_effectID = GetWeaponItemModEffect(_baseWeap, 1);
        match_index = (_effectID == l_effectID) ? 1 : 4;
    }
    if ((flag & 0b010) != 0) {
        l_effectID = GetWeaponItemModEffect(_baseWeap, 2);
        match_index = (_effectID == l_effectID) ? 2 : 4;
    }
    if ((flag & 0b100) != 0) {
        l_effectID = GetWeaponItemModEffect(_baseWeap, 3);
        match_index = (_effectID == l_effectID) ? 3 : 4;
    }

    return match_index < 4 ? WeaponGetModEffectValue(_baseWeap, match_index) : 0.0f;
}
/*
    GetEquippedWeaponFireRate some bug for now
    care later
*/
static float GetEquippedWeaponFireRate(Actor* _actor) {
    if (auto* eqWeap = GetEquippedObject(_actor, 5); IS_TYPE(eqWeap, TESObjectWEAP)) {
        float _BuffFormMod = GetEffectValueFromEquippedWeaponMod(_actor, 
                                                                 static_cast<TESObjectWEAP*>(eqWeap),
                                                                 TESObjectWEAP::kWeaponModEffect_IncreaseRateOfFire);
        
        __WriteLog2(true,"[GetEquippedWeaponFireRate] _BuffFormMod %.5f", _BuffFormMod);
        return static_cast<TESObjectWEAP*>(eqWeap)->fireRate + _BuffFormMod;
    }

    return 0.0F;
}

static TESObjectARMO* GetEquippedArmorObj(Actor* _actor) {
    if (auto* eq_form = GetEquippedObject(_actor,BodyPart(UpperBody))){
        if (IS_TYPE(eq_form, TESObjectARMO)) {
            return static_cast<TESObjectARMO*>(eq_form);
        }
    }
    return nullptr;
}

static TESObjectARMO* GetEquippedHelmetObj(Actor* _actor) {
    if (auto* eq_form = GetEquippedObject(_actor, BodyPart(Headband))) {
        if (IS_TYPE(eq_form, TESObjectARMO)) {
            return static_cast<TESObjectARMO*>(eq_form);
        }
    }
    return nullptr;
}


static bool IsUnarmOrWeaponIsNull(Actor* _actor) {
    auto* eq_form = GetEquippedObject(_actor, BodyPart(Weapon));
    if (!eq_form)
        return true;
    if (IS_TYPE(eq_form, TESObjectWEAP)) {
        auto* _weap = static_cast<TESObjectWEAP*>(eq_form);
        //__WriteLog("%d", _weap->eWeaponType);
        return _weap->eWeaponType == TESObjectWEAP::EWeaponType::kWeapType_HandToHandMelee;
    }
    return false;
}

static bool IsEquipThrowableMelee(Actor* _actor) {
    auto* eq_form = GetEquippedObject(_actor, BodyPart(Weapon));
    if (eq_form && IS_TYPE(eq_form, TESObjectWEAP)) {
        auto* _weap = static_cast<TESObjectWEAP*>(eq_form);
        return (_weap->eWeaponType == TESObjectWEAP::EWeaponType::kWeapType_OneHandThrown);
    }
    return false;
}

static bool IsEquipMelee(Actor* _actor) {
    auto* eq_form = GetEquippedObject(_actor, BodyPart(Weapon));
    if (eq_form && IS_TYPE(eq_form, TESObjectWEAP)) {
        auto* _weap = static_cast<TESObjectWEAP*>(eq_form);
        return (_weap->eWeaponType >= TESObjectWEAP::EWeaponType::kWeapType_HandToHandMelee &&
            _weap->eWeaponType <= TESObjectWEAP::EWeaponType::kWeapType_TwoHandMelee);
    }
    return false;
}

static bool WeapIsMelee(const TESObjectWEAP* _weap,bool checkSkill = false) {
    bool _ret = false;
    _ret = ((_weap->eWeaponType >= TESObjectWEAP::EWeaponType::kWeapType_HandToHandMelee &&
        _weap->eWeaponType <= TESObjectWEAP::EWeaponType::kWeapType_TwoHandMelee)) ;
    
    if (_ret)
        return _ret;

    _ret = checkSkill ? 
        (_weap->weaponSkill == ActorValueCode::kAVCode_MeleeWeapons || _weap->weaponSkill == ActorValueCode::kAVCode_Unarmed) : 
        false;

    return _ret;
}

static bool WeapIsRanged(const TESObjectWEAP* _weap) {
    return (_weap->eWeaponType >= TESObjectWEAP::EWeaponType::kWeapType_OneHandPistol &&
        _weap->eWeaponType <= TESObjectWEAP::EWeaponType::kWeapType_TwoHandLauncher);
}

static bool IsEquipLauncher(Actor* _actor) {
    auto* eq_form = GetEquippedObject(_actor, BodyPart(Weapon));
    if (eq_form && IS_TYPE(eq_form, TESObjectWEAP)) {
        auto* _weap = static_cast<TESObjectWEAP*>(eq_form);
        return _weap->eWeaponType == TESObjectWEAP::EWeaponType::kWeapType_TwoHandMelee ;
    }
    return false;
}

/*
kWeapType_HandToHandMelee = 0,
        kWeapType_OneHandMelee,
        kWeapType_TwoHandMelee,
        kWeapType_OneHandPistol,
        kWeapType_OneHandPistolEnergy,
        kWeapType_TwoHandRifle,
        kWeapType_TwoHandAutomatic,
        kWeapType_TwoHandRifleEnergy,
        kWeapType_TwoHandHandle,
        kWeapType_TwoHandLauncher,
        kWeapType_OneHandGrenade,
        kWeapType_OneHandMine,
        kWeapType_OneHandLunchboxMine,
        kWeapType_OneHandThrown,
        kWeapType_Last

*/

static bool WeapIsBallisticRanged(const TESObjectWEAP* _weap,bool _checkUseSkill) {
    switch (_weap->eWeaponType)
    {
    case TESObjectWEAP::kWeapType_OneHandPistol:
    case TESObjectWEAP::kWeapType_TwoHandRifle:
    case TESObjectWEAP::kWeapType_TwoHandAutomatic:
    case TESObjectWEAP::kWeapType_TwoHandHandle:
        if (_checkUseSkill)
        {
            if (_weap->weaponSkill != 41 && _weap->weaponSkill != 33) { // Not Big Gun And Gun
                return false;
            }
        }
        return true;
    default:
        return false;
    }
}


static bool IsEquipRanged(Actor* _actor) {
    auto* eq_form = GetEquippedObject(_actor, BodyPart(Weapon));
    if (eq_form && IS_TYPE(eq_form, TESObjectWEAP)) {
        auto* _weap = static_cast<TESObjectWEAP*>(eq_form);
        return WeapIsRanged(_weap);
    }
    return false;
}

static bool IsEquipNotMelee(Actor* _actor) {
    auto* eq_form = GetEquippedObject(_actor, BodyPart(Weapon));
    if (eq_form && IS_TYPE(eq_form, TESObjectWEAP)) {
        auto* _weap = static_cast<TESObjectWEAP*>(eq_form);
        return (_weap->eWeaponType > TESObjectWEAP::EWeaponType::kWeapType_TwoHandMelee &&
            _weap->eWeaponType != TESObjectWEAP::EWeaponType::kWeapType_Last);
    }
    return false;
}

static bool GetWeaponClass(Actor* _actor) {
    auto* eq_form = GetEquippedObject(_actor, BodyPart(Weapon));
    if (eq_form && IS_TYPE(eq_form, TESObjectWEAP)) {
        auto* _weap = static_cast<TESObjectWEAP*>(eq_form);
        return (_weap->eWeaponType > TESObjectWEAP::EWeaponType::kWeapType_TwoHandMelee &&
            _weap->eWeaponType != TESObjectWEAP::EWeaponType::kWeapType_Last);
    }
    return false;
}

/*
GetArmorClassInfo
the return is an information struct:
struct ArmorClassInfo {
        bool isPowerArmor;
        ArmorClass arClass;
    };

arClass will set to ArmorClass when _armor->bipedModel is nullptr
*/
static auto GetArmorClassInfo(TESObjectARMO* _armor) {
    struct ArmorClassInfo {
        bool isPowerArmor;
        ArmorClass arClass;
    };
    TESBipedModelForm* biped = &(_armor->bipedModel);
    if (!biped) return ArmorClassInfo{ false, ArmorClass::None };
    return ArmorClassInfo{ 
        biped->IsPowerArmor(),
        ((biped->bipedFlags & 0x80) ? ArmorClass::Heavy :
            ((biped->bipedFlags & 8) ? ArmorClass::Medium : 
                ArmorClass::Light))
            };
}

static auto GetWeapClassInfo(TESObjectWEAP* _weap) {
    struct ArmorClassInfo {
        bool isPowerArmor;
        WeapClass wpClass;
    };
    
}



static __forceinline float ApplyAmmoEffect(float _oldVal,UINT32 _operation,float _OpValue) {
    switch (_operation)
    {
    case TESAmmoEffect::kOperation_Add:
        return _oldVal + _OpValue;
    case TESAmmoEffect::kOperation_Multiply:
        return _oldVal * _OpValue;
    case TESAmmoEffect::kOperation_Subtract:
        return _oldVal - _OpValue;
    default:
        return _oldVal;
    }
}

static __forceinline float ApplyAmmoEffect(const TESAmmoEffect* _ammoEffect, TESAmmoEffect::EffectType _expect,float _input) {
    if (static_cast<TESAmmoEffect::EffectType>(_ammoEffect->type) == _expect) {
        return ApplyAmmoEffect(_input, _ammoEffect->operation, _ammoEffect->value);
    }
    return _input;
}

struct Wrapper_EffectiveDTDR {
    float DT, DR;
    Wrapper_EffectiveDTDR() = delete;
    Wrapper_EffectiveDTDR(float _dt, float _dr) : DT(_dt), DR(_dr){}
};

static __forceinline void ApplyAmmoEffects(const TESAmmo* _ammo,OUT Wrapper_EffectiveDTDR& _DT_DR_DMG) {
    const auto& effect_list = _ammo->effectList;
    auto iter = effect_list.Head();

    do
    {
        if (!iter) break;
        if (const auto* effect = iter->data) {
            _DT_DR_DMG.DT = ApplyAmmoEffect(effect,TESAmmoEffect::EffectType::DTMod, _DT_DR_DMG.DT);
            _DT_DR_DMG.DR = ApplyAmmoEffect(effect,TESAmmoEffect::EffectType::DRMod, _DT_DR_DMG.DR);
        }
    } while (iter = iter->next);
}

static __forceinline void PrintAmmoEffect(const TESAmmoEffect* _ammoEffect) {
    auto printEffectType = [](TESAmmoEffect::EffectType _eType) -> const char* {
        switch (_eType)
        {
        case TESAmmoEffect::EffectType::DamageMod:
            return "Damage";
        case TESAmmoEffect::EffectType::DRMod:
            return "DR";
        case TESAmmoEffect::EffectType::DTMod:
            return "DT";
        case TESAmmoEffect::EffectType::SpreadMod:
            return "Spread";
        case TESAmmoEffect::EffectType::ConditionMod:
            return "Condition";
        case TESAmmoEffect::EffectType::FatigueMod:
            return "Fatigue";
        default:
            return "Unknown_Effect";
        }
    };
    auto printOpType = [](UInt32 _eType) -> const char* {
        switch (_eType)
        {
        case TESAmmoEffect::kOperation_Add:
            return " + ";
        case TESAmmoEffect::kOperation_Multiply:
            return " * ";
        case TESAmmoEffect::kOperation_Subtract:
            return " - ";
        default:
            return "Unknown_Operation";
        }
    };

    std::stringstream _ss;
    _ss << ">AmmoEffect: " << printEffectType(static_cast<TESAmmoEffect::EffectType>(_ammoEffect->type)) 
                            << printOpType(_ammoEffect->operation) 
                            << _ammoEffect->value;
    __WriteLog2(true, _ss.str().c_str());
}

static __forceinline void PrintAmmoEffects(const TESAmmo* _ammo) {
    const auto& effect_list = _ammo->effectList;
    auto iter = effect_list.Head();

    __WriteLog2(true,">>Ammo: %s",_ammo->GetEditorID());
    __WriteLog2(true,">>Ammo Effect Count: %u", effect_list.Count());
    do
    {
        if (!iter) break;
        if (const auto* effect = iter->data) {
            PrintAmmoEffect(effect);
        }
    } while (iter = iter->next);
}

static __forceinline NiNode* PCGet1stBlock(const char* _nodeName) {
    PlayerCharacter* _pc = PlayerCharacter::GetSingleton();
    if (_pc->IsThirdPerson())
        return nullptr;

    if (NiNode* _1stNode = _pc->node1stPerson) {
        return _1stNode->GetBlock(_nodeName)->GetNiNode();
    }
}

static __forceinline NiNode* PCGet1stIronSightNode() {
    PlayerCharacter* _pc = PlayerCharacter::GetSingleton();
    if (_pc->IsThirdPerson())
        return nullptr;

    if (NiNode* _isNode = _pc->ironSightNode) {
        return _isNode;
    }
    else if (NiNode* _1stNode = _pc->node1stPerson) {
        if (NiNode* _isNode = CdeclCall<NiNode*>(cdecl_getSubNodeByNameAddr, _1stNode, "##SightingNode")){
            return _isNode;
        }
    }
    return nullptr;
}

static __forceinline NiNode* GetPC1stPJNodeAlt() {
    PlayerCharacter* _pc = PlayerCharacter::GetSingleton();
    if (_pc->IsThirdPerson()) {
        //__WriteLog("PC IS 3rd");
        return nullptr;
    }
    
   
    if (NiNode* _1stNode = _pc->node1stPerson) {
        if(NiAVObject* _pjNode = _1stNode->GetBlock("ProjectileNode"))
        {
            return _pjNode->GetNiNode();
        }
    }
    return nullptr;
}

static __forceinline NiNode* GetPC1stPJNode() {
    PlayerCharacter* _pc = PlayerCharacter::GetSingleton();
    if (_pc->IsThirdPerson()){
        return nullptr;
    }
    if (NiNode* _1stPjNode = GetActorPJNode(_pc)){
        return _1stPjNode;
    }
    
    if (NiNode* _1stNode = _pc->node1stPerson) {
        if (NiAVObject* _pjNode = _1stNode->GetBlock("ProjectileNode")) {
            return _pjNode->GetNiNode();
        }
        else if (NiNode* _1stPjNode = CdeclCall<NiNode*>(cdecl_getSubNodeByNameAddr, _1stNode, "ProjectileNode")) {
            return _1stPjNode;
        }
    }

   /* if (NiNode* _1stNode = _pc->node1stPerson) {
        NiNode* _pjNode =  _1stNode->GetBlock("ProjectileNode")->GetNiNode();
        if (_pjNode)
        {
            return _pjNode;
        }
    }*/
    return nullptr;
}

static __forceinline NiVector3 GetActorPJNodePos(Actor* _actor) {
    if (auto* pj_node = GetActorPJNode(_actor)) {
        return pj_node->m_transformWorld.translate;
    }
    return { 0,0,0 };
}

static __forceinline bool PJIsExplosion(const BGSProjectile* basepj) {
	return (basepj->projFlags & 2) != 0 && (basepj->type & 1) != 0;
}

static __forceinline float CalcProjSpeed(const BGSProjectile* basepj, const Projectile* _pjref) {
	return (basepj->speed * _pjref->speedMult);
}

static __forceinline SceneGraph* GetSceneGraph() {
	return (*(SceneGraph**)0x11DEB7C);
}

static __forceinline float CalcPosSquareDis(const NiVector3& posA, const NiVector3& posB) {
    return (Ut_Square((posA.x - posB.x)) + Ut_Square((posA.y - posB.y)) + Ut_Square((posA.z - posB.z)));
}

static __forceinline float CalcPosSquareDisSIMD(const NiVector3& posA, const NiVector3& posB) {
    __m128 _p12_diff = _mm_sub_ps(posA.PS(), posB.PS());
    __m128 _mul = _mm_mul_ps(_p12_diff, _p12_diff);
    __m128 hadd1 = _mm_hadd_ps(_mul, _mul);
    __m128 hadd2 = _mm_hadd_ps(hadd1, hadd1);
    float _res = _mm_cvtss_f32(hadd2);
    return _res;
}

static __forceinline void CalcMidPoint3(const NiVector3& posA,const NiVector3& posB,NiVector3& out_mid) {
    out_mid.x = (posA.x + posB.x) / 2;
    out_mid.y = (posA.y + posB.y) / 2;
    out_mid.z = (posA.z + posB.z) / 2;
}

static __forceinline NiVector3 ScalarSub(const NiVector3& _from, const NiVector3& _to) {
    return {_to.x - _from.x,
            _to.y - _from.y,
            _to.z - _from.z};
}

static __forceinline NiVector3 ScalarAdd(const NiVector3& _v1, const NiVector3& _v2) {
    return { _v1.x + _v2.x,
            _v1.y + _v2.y,
            _v1.z + _v2.z };
}

static __forceinline float GetRefDistanceSquare(const TESObjectREFR* _refA, const TESObjectREFR* _refB) {
	return (Ut_Square(_refB->position.x - _refA->position.x) + 
            Ut_Square(_refB->position.y - _refA->position.y) + 
            Ut_Square(_refB->position.z - _refA->position.z));
}

static __forceinline float GetRefDistanceSquare2D(const TESObjectREFR* _refA, const TESObjectREFR* _refB) {
	return (Ut_Square(_refB->position.x - _refA->position.x) + Ut_Square(_refB->position.y - _refA->position.y));
}

static __forceinline bool IsPJHitscan(const BGSProjectile* _baseproj) {
	return (_baseproj->projFlags & BGSProjectile::kFlags_Hitscan) != 0;
}

static __forceinline float GetPJMuzzleFlashDuration(const BGSProjectile* _baseproj) {
	return _baseproj->flashDuration;
}

static __forceinline bool IsPJHitscan(const Projectile* _proj) {
	return (((BGSProjectile*)_proj->baseForm)->projFlags & BGSProjectile::kFlags_Hitscan) != 0;
}

static __forceinline BGSImpactDataSet* GetImpactDataSet(const TESObjectWEAP* _weap) {
	return _weap->impactDataSet;
}

enum CreatureType : UInt8 {
    NotCreature = 9,
    Animal = 0,
    Mutated_Animal = 1,
    Mutated_Insect = 2,
    Abomination = 3,	// Intellective
    Supermutant = 4,	// Intellective
    Feral_Ghoul = 5,
    Robot = 6,			// Intellective
    Giant = 7,
    Count = 8
};

static const char* GetCreatureTypeCStr(CreatureType _type) {
    switch (_type)
    {
    case CreatureType::Animal:
        return "Animal";
    case CreatureType::Mutated_Animal:
        return "Mutated_Animal";
    case CreatureType::Mutated_Insect:
        return "Mutated_Insect";
    case CreatureType::Abomination:
        return "Abomination";
    case CreatureType::Supermutant:
        return "Supermutant";
    case CreatureType::Feral_Ghoul:
        return "Feral_Ghoul";
    case CreatureType::Robot:
        return "Robot";
    case CreatureType::Giant:
        return "Giant";
    case CreatureType::NotCreature:
        return "NotCreature";
    case CreatureType::Count:
        return "Null";
    default:
        return "Null";
    }
    return "Null";
}

static std::string GetCreatureTypeStr(CreatureType _type) {
    return GetCreatureTypeCStr(_type);
}

enum Ret_CreatureType : UInt8 {
    Ret_Creature = 0,
    Ret_IntellectiveCreature = 1,
    Ret_NotCreature = 2
};

static CreatureType __forceinline GetCreatureType(const TESActorBase* _base) {
    if IS_ID(_base, TESCreature) {
        return (CreatureType)((TESCreature*)_base)->type;
    }
    return NotCreature;
}

static CreatureType __forceinline GetCreatureType(const Actor* _actor) {
    if (const TESCreature* creature = (TESCreature*)(_actor->GetActorBase())) {
        return GetCreatureType(creature);
    }
    return NotCreature;
}

static bool __forceinline IsIntellective(const Actor* _actor) {
    CreatureType c_type = GetCreatureType(_actor);
    return (c_type == NotCreature || c_type == Abomination || c_type == Supermutant || c_type == Robot);
}

static bool __forceinline IsGunUser(const Actor* _actor) {
    CreatureType c_type = GetCreatureType(_actor);
    return (c_type == NotCreature  || c_type == Supermutant || c_type == Robot);
}

static bool __forceinline IsRobot(const Actor* _actor) {
    return GetCreatureType(_actor) == Robot;
}

static bool __forceinline IsSupermutant(const Actor* _actor) {
    return GetCreatureType(_actor) == Supermutant;
}

static bool __forceinline CreatureHaveWeap(Actor* _actor) {
    return IsSupermutant(_actor) && GetEquippedObject(_actor, 5);
}

static bool __forceinline CreatureHaveWeap(Actor* _actor,CreatureType _cType) {
    return (_cType == Supermutant) && GetEquippedObject(_actor, 5);
}

/*
    https://geckwiki.com/index.php?title=Form_Type_IDs
*/
static bool __forceinline FormTypeIsProjectile(UINT8 _typeCode) {
    switch (_typeCode) {
    case 51:
    case 61:
    case 62:
    case 63:
    case 64:
    case 105:
        return true;
    default:
        return false;
    }
    return false;
}

static bool __forceinline FormTypeIsProjectile(const TESObjectREFR* _refr) {
    return FormTypeIsProjectile(_refr->typeID);
}

static bool __forceinline FormTypeIsExplosion(const TESObjectREFR* _refr) {
    return _refr->typeID == 81;
}

/*
    Raycast
*/
static __forceinline float GetRayCastRange(const TESObjectREFR* _refr, float rot_x, float rot_z, float _maxRange = 1024.0f, SInt32 _layerType = 6) {
    NiVector3 pos{ _refr->position };

    NiVector4 rot(_mm_setzero_ps());
    rot.x = rot_x * RdToAg;
    rot.z = rot_z * RdToAg;
    if (NiMatrix33 rotMat; rotMat.FromEulerPRY(rot * GET_PS(8)) &&
        (rot.RayCastCoords(pos, rotMat + 1, _maxRange, _layerType)))
    {
        /*auto _invMat = rotMat.ToEulerPRY();
        alignas(16) float result[4];
        _mm_store_ps(result, _invMat);*/
        /*__WriteLog("_invMat: %.5f, %.5f, %.5f, %.5f", result[0] * DeToAg, result[1] * DeToAg, result[2] * DeToAg, result[3] * DeToAg);
        __WriteLog("cast pos: %.5f, %.5f, %.5f", rot.x, rot.y, rot.z);*/
        /* __WriteLog("dis: %.5f", CalcPosSquareDis(pos, rot));*/
        __WriteLog("cast pos: %.5f, %.5f, %.5f", rot.x, rot.y, rot.z);
        return CalcPosSquareDis(pos, rot);
    }
    else {
        __WriteLog("Ray Cast Failed");
    }
    return 0;
}

static __forceinline float GetRayCastRange(const NiVector3& _pos, float rot_x, float rot_z, float _maxRange = 1024.0f, SInt32 _layerType = 6) {

    NiVector4 rot(_mm_setzero_ps());
    rot.x = rot_x * RdToAg;
    rot.z = rot_z * RdToAg;
    if (NiMatrix33 rotMat; rotMat.FromEulerPRY(rot * GET_PS(8)) &&
        (rot.RayCastCoords(_pos, rotMat + 1, _maxRange, _layerType)))
    {
        /*auto _invMat = rotMat.ToEulerPRY();
        alignas(16) float result[4];
        _mm_store_ps(result, _invMat);*/
        /*__WriteLog("_invMat: %.5f, %.5f, %.5f, %.5f", result[0] * DeToAg, result[1] * DeToAg, result[2] * DeToAg, result[3] * DeToAg);
        __WriteLog("cast pos: %.5f, %.5f, %.5f", rot.x, rot.y, rot.z);*/
        /* __WriteLog("dis: %.5f", CalcPosSquareDis(pos, rot));*/
        return CalcPosSquareDis(_pos, rot);
    }
    else {
        __WriteLog("Ray Cast Failed");
    }
    return 0;
}

static __forceinline auto GetRayCastPos(const NiVector3& beginPos, float rot_x, float rot_z, float _maxRange = 1024.0f, SInt32 _layerType = 6) {
    struct {
        bool raycastSuccess = false;
        NiVector3 raycastPos{};
    } raycastResult;
    __WriteLog("[GetRayCastPos] begin pos = %.2f, %.2f, %.2f, rot x is %.2f( rad: %.2f ), roz is %.2f( rad: %.2f) ",
        beginPos.x, beginPos.y, beginPos.z, rot_x, rot_x * RdToAg, rot_z,  rot_z * RdToAg);
    

    NiVector4 rot(_mm_setzero_ps());
    rot.x = rot_x * RdToAg;
    rot.z = rot_z * RdToAg;
    if (NiMatrix33 rotMat; rotMat.FromEulerPRY(rot * GET_PS(8)) &&
        (rot.RayCastCoords(beginPos, rotMat + 1, _maxRange, _layerType)))
    {
        __WriteLog("[GetRayCastPos] raycast pos = %.2f, %.2f, %.2f",
            rot.x, rot.y, rot.z);
        raycastResult.raycastPos = NiVector3{ rot };
        raycastResult.raycastSuccess = true;
        return raycastResult;
    }
    else {
        __WriteLog("[GetRayCastPos] Ray Cast Failed");
    }
    return raycastResult;
}

static __forceinline NiVector3 GetRayCastPos(const TESObjectREFR* _refr, float rot_x, float rot_z, float _maxRange = 1024.0f, SInt32 _layerType = 6) {
    NiVector3 beginPos{ _refr->position };
    __WriteLog("[GetRayCastPos] begin pos = %.2f, %.2f, %.2f, rot x is %.2f, roz is %.2f",
        beginPos.x, beginPos.y, beginPos.z, rot_x, rot_z);


    NiVector4 rot(_mm_setzero_ps());
    rot.x = rot_x * RdToAg;
    rot.z = rot_z * RdToAg;
    if (NiMatrix33 rotMat; rotMat.FromEulerPRY(rot * GET_PS(8)) &&
        (rot.RayCastCoords(beginPos, rotMat + 1, _maxRange, _layerType)))
    {
        __WriteLog("[GetRayCastPos] raycast pos = %.2f, %.2f, %.2f",
            rot.x, rot.y, rot.z);
        return NiVector3{ rot };
    }
    else {
        __WriteLog("Ray Cast Failed");
    }
    return NiVector3{};
}
/*
    Raycast
*/

enum MaterialType
{
    kMaterial_Stone,
    kMaterial_Dirt,
    kMaterial_Grass,
    kMaterial_Glass,
    kMaterial_Metal,
    kMaterial_Wood,
    kMaterial_Organic,
    kMaterial_Cloth,
    kMaterial_Water,
    kMaterial_HollowMetal,
    kMaterial_OrganicBug,
    kMaterial_OrganicGlow
};

alignas(16) const UInt8 kMaterialConvert[] =
{
    kMaterial_Stone,
    kMaterial_Cloth,
    kMaterial_Dirt,
    kMaterial_Glass,
    kMaterial_Grass,
    kMaterial_Metal,
    kMaterial_Organic,
    kMaterial_Organic,
    kMaterial_Water,
    kMaterial_Wood,
    kMaterial_Stone,
    kMaterial_Metal,
    kMaterial_Wood,
    kMaterial_Metal,
    kMaterial_Metal,
    kMaterial_Metal,
    kMaterial_HollowMetal,
    kMaterial_HollowMetal,
    kMaterial_Dirt,
    kMaterial_Stone,
    kMaterial_Metal,
    kMaterial_Metal,
    kMaterial_HollowMetal,
    kMaterial_HollowMetal,
    kMaterial_Glass,
    kMaterial_HollowMetal,
    kMaterial_Metal,
    kMaterial_Metal,
    kMaterial_HollowMetal,
    kMaterial_HollowMetal,
    kMaterial_Wood,
    kMaterial_Wood
};

// Deal with the material type from the 'raw' hit material which is recorded in impact data
static __forceinline UINT8 MaterialConverter(UINT32 hitmaterial) {
    __WriteLog("[MaterialConverter] raw hit material is %d", hitmaterial);
    return (hitmaterial <= 31 ? kMaterialConvert[hitmaterial] : (UINT8)255); 
}

// arg - hit material which have been converted
static __forceinline UINT8 IsMetal(UINT32 material) {
    return material == 4 || material == 9;
}

/*
 ==== material(converted) ====
0	Stone | 1	Dirt | 2	Grass | 3	Glass
4	Metal | 5	Wood | 6  Organic | 7	Cloth
8	Water | 9	Hollow Metal | 10	Organic Bug | 11	Organic Glow
*/
static __forceinline BGSImpactData* GetImpactDataByMaterial(const TESObjectWEAP* _weap,UINT8 _material) {

    __WriteLog("[GetImpactDataByMaterial] current arg material is %d", _material);
    if (_material < 12) {
        if (auto* _impactDataSet = GetImpactDataSet(_weap)) {
            return _impactDataSet->impactDatas[_material];
        }
    }
	return nullptr;
}

static __forceinline const char* GetMaterialName(UInt32 _material) {
    switch ((MaterialType)_material)
    {
    case kMaterial_Stone:
        return "Stone";
    case kMaterial_Dirt:
        return "Dirt";
    case kMaterial_Grass:
        return "Grass";
    case kMaterial_Glass:
        return "Glass";
    case kMaterial_Metal:
        return "Metal";
    case kMaterial_Wood:
        return "Wood";
    case kMaterial_Organic:
        return "Organic";
    case kMaterial_Cloth:
        return "Cloth";
    case kMaterial_Water:
        return "Water";
    case kMaterial_HollowMetal:
        return "HollowMetal";
    case kMaterial_OrganicBug:
        return "OrganicBug";
    case kMaterial_OrganicGlow:
        return "OrganicGlow";
    default:
        break;
    }
}

// crash
static __forceinline void LoadDecalTempEffect(TESObjectCELL* _cell,BGSImpactData* _impactData,NiVector3 _rot,NiVector3 _pos,
                                            float scale,int arg,NiNode* eNode = nullptr) {

    CdeclCall(0x6890B0, _cell, _impactData->effectDuration,_impactData->model.nifPath,_rot,_pos, scale, arg, eNode);
}

// why this return null?
static __forceinline TESAmmo* GetCurEqAmmo(const Actor* _actor) {
    if (ContChangesEntry* ammoInfo = _actor->GetAmmoInfo(); ammoInfo && ammoInfo->type)
        return static_cast<TESAmmo*>(ammoInfo->type);
    return nullptr;
}

static __forceinline TESAmmo* GetCurEqAmmo(const TESObjectWEAP* _Weap, const Actor* _Actor) {
    return ThisStdCall<TESAmmo*>(0x525980, _Weap, _Actor);
}

static bool CheckIsAPPenetrateAmmo(const TESAmmo* _ammo) {
    bool _isAPPenetrate = false;
    float _APModify = 0.0f;
    const auto& effect_list = _ammo->effectList;

    auto iter = effect_list.Head();
    do
    {
        if (!iter) break;
        if (const auto* effect = iter->data) {
            if (effect->type == TESAmmoEffect::kEffectType_DTMod) {
                switch (effect->operation)
                {
                case TESAmmoEffect::kOperation_Add:
                    _APModify += effect->value;	// Hollow
                    break;
                case TESAmmoEffect::kOperation_Multiply:
                    if (effect->value > 1) return false;
                    if (effect->value < 1) return true;
                    break;
                case TESAmmoEffect::kOperation_Subtract:
                    _APModify -= effect->value;	// AP
                    break;
                default:
                    break;
                }
            }
        }
    } while (iter = iter->next);
    return (_APModify < 0.0f);
}

static __forceinline void ResetRefCollision(TESObjectREFR* _ref) {
    std::vector<NiNode*> col_nodes;
    GetCollisionNodes(_ref, OUT col_nodes);
    for (NiNode* nd : col_nodes)
    {
        ResetNodeCollision(nd);
    }
}

static __forceinline void InfoRefCollision(TESObjectREFR* _ref) {
    std::vector<NiNode*> col_nodes;
    __WriteLog("render state flags %u",_ref->renderState->flags);
    GetCollisionNodes(_ref, OUT col_nodes);
    for (NiNode* nd : col_nodes)
    {
        __WriteLog("col flag %u", nd->m_collisionObject->flags);
        PrintWorldObjInfo(nd);
    }
}

static __forceinline NiNode* CdBodyUnkGetNode(hkCdBody* a3) {
    return CdeclCall<NiNode*>(cdecl_cdBody_UnkGetNode, a3);
}

static __forceinline void FindRefInMovableObjList(TESObjectREFR* _ref) {
    auto& obj_list = ProcessManager::Get()->objects;
    for (auto iter = obj_list.Begin(); iter; ++iter){
        if (*iter && (*iter)->refID == _ref->refID) {
            __WriteLog("Find _ref %x", _ref->refID);
            return;
        }
    }
    __WriteLog("Cant Find _ref %x", _ref->refID);
}

/*
    Call this func after null check
    Just check missile type and explosion flag is on
*/
static __forceinline bool BGSProjIsMissileExplosion(const BGSProjectile* _pj) {
    if ((_pj->projFlags & 2) != 0 && (_pj->type & 1) != 0) { 
        return true;
    }
    return false;
}

/*
    Call this func after null check
*/
static __forceinline bool ProjIsMissileExplosion(Projectile* _pj) {
    if (const TESForm* basepj = _pj->baseForm; IS_TYPE(basepj,BGSProjectile)){
        if (BGSProjIsMissileExplosion(static_cast<const BGSProjectile*>(basepj))){
            return true;
        }
    }
    return false;
}

static __forceinline SInt32 GetCurrentAmmoRounds(Actor* _actor) {
    if (ContChangesEntry* ammoInfo = _actor->GetAmmoInfo())
        return ammoInfo->countDelta;

    return -1;
}



static bool SetNumericGameSetting(const char* settingName, float newVal)
{
    Setting* setting;
    GameSettingCollection* gmsts = GameSettingCollection::GetSingleton();
    if (gmsts && gmsts->GetGameSetting(settingName, &setting))
    {

        if (setting->Set(newVal)) {
            return true;
        }

        __WriteLog("[SetNumericGameSetting] Set GameSetting %s To %.5f Failed", settingName, newVal);
        return false;
    }
    __WriteLog("[SetNumericGameSetting] GameSetting Singleton Not Exist Or Setting %s Not Exist", settingName);
    return false;
}