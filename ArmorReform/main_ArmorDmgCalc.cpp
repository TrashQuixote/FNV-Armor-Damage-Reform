#include "ArmorFormulaManager.h"
#include "backward.hpp"
//#include "utilities/IConsole.h"
//NoGore is unsupported in xNVSE

IDebugLog		gLog("ArmorReformula.log");
PluginHandle	g_pluginHandle = kPluginHandle_Invalid;
NVSEMessagingInterface* g_messagingInterface{};
NVSEInterface* g_nvseInterface{};
NVSEEventManagerInterface* g_eventInterface{};
_InventoryRefGetForID InventoryRefGetForID;
_InventoryRefCreate InventoryRefCreate;

static void PrintCallStack() {
	std::stringstream _ss;
	using namespace backward;
	StackTrace st;
	st.load_here(32);
	Printer pt;
	pt.print(st, _ss);
	__WriteLog2(true, _ss.str().c_str());
}

template <class T>
inline T& singleton() {
	static T instance;
	return instance;
}



bool NVSEPlugin_Query(const NVSEInterface* nvse, PluginInfo* info)
{
	_MESSAGE("query");

	// fill out the info structure
	info->infoVersion = PluginInfo::kInfoVersion;
	info->name = "ArmorDamageReformula";
	info->version = 114;

	// version checks
	if (nvse->nvseVersion < PACKED_NVSE_VERSION)
	{
		_ERROR("NVSE version too old (got %08X expected at least %08X)", nvse->nvseVersion, PACKED_NVSE_VERSION);
		return false;
	}

	if (!nvse->isEditor)
	{
		if (nvse->runtimeVersion < RUNTIME_VERSION_1_4_0_525)
		{
			_ERROR("incorrect runtime version (got %08X need at least %08X)", nvse->runtimeVersion, RUNTIME_VERSION_1_4_0_525);
			return false;
		}

		if (nvse->isNogore)
		{
			_ERROR("NoGore is not supported");
			return false;
		}
	}
	else
	{
		if (nvse->editorVersion < CS_VERSION_1_4_0_518)
		{
			_ERROR("incorrect editor version (got %08X need at least %08X)", nvse->editorVersion, CS_VERSION_1_4_0_518);
			return false;
		}
	}

	// version checks pass
	// any version compatibility checks should be done here
	return true;
}


						


namespace ArmorReformula {
	enum class ReformulaSetting {
		CustomFormulaForRangedHit = 0x0001, CustomFormulaForMeleeHit = 0x0002, CustomFormulaForExplosionHit = 0x0004,DebugMode = 0x0008,
	};
	enum class FormulaClassify {
		Internal,Custom
	};
	enum class CalculateFor {
		RangedHit,MeleeHit,ExplosionHit,None
	};
	static OptFlag<ReformulaSetting> s_reformula_flag{};
	static FormulaClassify s_rangedHitFormulaCls = FormulaClassify::Internal;
	static FormulaClassify s_meleeHitFormulaCls = FormulaClassify::Internal;
	static FormulaClassify s_explosionHitFormulaCls = FormulaClassify::Internal;



	static __forceinline bool IsDebugMode() {
		return s_reformula_flag.IsFlagOn(ReformulaSetting::DebugMode);
	}

	static __forceinline void InitCustomFormulaMng(CustomFormulaMng::FormulaIndex _which, const std::string& _formulaStr) {
		singleton<CustomFormulaMng>().setFormual(_which, _formulaStr);
	}

	

	static __forceinline CalcResult CalcNewArmorDamage(const ActorHitData* _hData, SInt32 _hitLoc, CalculateFor _for,bool isDebugMode) {
		CalcResult ret{};
		InternalArgs _internalArgs;
		switch (_for)
		{
		case CalculateFor::RangedHit:
			__WriteLogCond(isDebugMode, "Calculate For Ranged Hit");
			_internalArgs = CreateInternalArgsWrap(_hData,isDebugMode);
			ret = (s_rangedHitFormulaCls == FormulaClassify::Custom) ?
				singleton<CustomFormulaMng>().calcByNewFormula(CustomFormulaMng::FormulaIndex::RangedHit, _internalArgs, isDebugMode) :
				singleton<InternalFormulaMng>().calcByNewFormula(InternalFormulaMng::FormulaClass::RangedHit, _internalArgs, isDebugMode);
			break;
		case CalculateFor::MeleeHit:
			__WriteLogCond(isDebugMode, "Calculate For Melee Hit");
			_internalArgs = CreateInternalArgsWrap(_hData, isDebugMode,true);
			ret = (s_meleeHitFormulaCls == FormulaClassify::Custom) ?
				singleton<CustomFormulaMng>().calcByNewFormula(CustomFormulaMng::FormulaIndex::MeleeHit, _internalArgs, isDebugMode) :
				singleton<InternalFormulaMng>().calcByNewFormula(InternalFormulaMng::FormulaClass::MeleeHit, _internalArgs, isDebugMode);
			break;
		case CalculateFor::ExplosionHit:
			__WriteLogCond(isDebugMode, "Calculate For Explosion Hit");
			_internalArgs = CreateInternalArgsWrap(_hData, isDebugMode, false,true);
			ret = (s_explosionHitFormulaCls == FormulaClassify::Custom) ?
				singleton<CustomFormulaMng>().calcByNewFormula(CustomFormulaMng::FormulaIndex::ExplosionHit, _internalArgs, isDebugMode) :
				singleton<InternalFormulaMng>().calcByNewFormula(InternalFormulaMng::FormulaClass::ExplosionHit, _internalArgs, isDebugMode);
			break;
		case CalculateFor::None:
		default:
			return ret;;
		}

		RoughHitLocation _rhloc = GetRoughHitLocation(_hData->hitLocation);
		ret.setRoughHitLocation(_rhloc);

		return ret;
	}

	static CallDetour _actor_OnHitMatter;
	static CallDetour _testHook;
	static CallDetour _actor_OnHitMatter_Explosion;

	static void __forceinline DeConditionArmor(Actor* _target, float _armorDmg,EquipSlotID _eID,bool isDebugMode) {
		float _beforeHealth = GetEquippedCurrentHealth(_target, ToUInt(_eID));
		float _newArmorHealth = _beforeHealth >= _armorDmg ? (_beforeHealth - _armorDmg) : 0.0f;
		SetEquippedCurrentHealth(_target, ToUInt(_eID), _newArmorHealth);
		__WriteLogCond(isDebugMode, ">>> Armor Health Before Hit: %.2f", _beforeHealth);
		__WriteLogCond(isDebugMode, ">>> Armor Health Before Hit: %.2f", _newArmorHealth);
	}

	static void __forceinline DeConditionArmor(Actor* _target, float _armorDmg , RoughHitLocation _rHitLoc, bool tarIsPlayer, bool isDebugMode) {
		if (_rHitLoc == RoughHitLocation::RHL_None)
		{
			return;
		}

		if (_rHitLoc == RoughHitLocation::RHL_ExplosionHit)// explosion
		{
			DeConditionArmor(_target, _armorDmg, EquipSlotID::UpperBody, isDebugMode);
			DeConditionArmor(_target, _armorDmg, EquipSlotID::Headband, isDebugMode);
		}
		else {
			if (!tarIsPlayer)
			{
				__WriteLogCond(isDebugMode, ">>> DeCondition Armor Of NPC");
				DeConditionArmor(_target, _armorDmg, RoughHitLocationToSlot(_rHitLoc), isDebugMode);
			}
			
		}
	}

	static void __forceinline NewArmorDamageCalculation(ActorHitData* _hitData, Actor* _target, bool _isMeleeHit, bool _targetIsPlayer,SInt32 _hitLoc, bool isDebugMode = false) {
		
		CalculateFor _for = _hitLoc == -1 ? CalculateFor::ExplosionHit : (_isMeleeHit ? CalculateFor::MeleeHit : CalculateFor::RangedHit);
		auto _res = CalcNewArmorDamage(_hitData, _hitLoc, _for, IsDebugMode());

		__WriteLogCond(isDebugMode, ">>> ArmorDmg(Vanilla) is %.2f", _hitData->armorDmg);
		if (_res.isCalcSuccess() && _res.getCalcResult() > 0.0f && !std::isnan(_res.getCalcResult()))
		{
			_hitData->armorDmg = _res.getCalcResult();
			__WriteLogCond(isDebugMode, ">> ArmorDmg(Moded) is %.2f", _hitData->armorDmg);
			
			DeConditionArmor(_target, _hitData->armorDmg, _res.getRoughHitLocation(), _isMeleeHit, isDebugMode);
		}
		else {
			__WriteLogCond(isDebugMode, ">>> ArmorDmg Calculate Failed");
		}
		
	}

	void PrintActorHitData(ActorHitData* _hData)  {
		__WriteLog2(true," hitLocation: %d, healthDmg: %.2f, armorDmg: %.2f,dmgMult: %.2f,projectileHitDmg: %.2f",
			_hData->hitLocation, _hData->healthDmg, _hData->armorDmg, _hData->dmgMult, _hData->projectile->hitDamage);
	}

	static void __fastcall Actor_OnHitMatter_Debug(Actor* _target, void* _edx, ActorHitData* _hitData, bool _unkFlag) {
		__WriteLog2(true, ">>>>> Print Current Hit Infos");
		
		if (!_target || !IS_ACTOR(_target)) {
			__WriteLog2(true, "Target Is Null");
			ThisStdCall(_actor_OnHitMatter.GetOverwrittenAddr(), _target, _hitData, _unkFlag);
			return;
		}
		TESObjectWEAP* _weap = _hitData->weapon;
		if (!_weap) {
			__WriteLog2(true, "Weap Is Null");
			ThisStdCall(_actor_OnHitMatter.GetOverwrittenAddr(), _target, _hitData, _unkFlag);
			return;
		}
		__WriteLog2(true, "Weap Type %u", _weap->eWeaponType);
		auto* _proj = _hitData->projectile;
		if (!_hitData || !_proj || !_hitData->source || IS_TYPE(_proj,Explosion) || _hitData->hitLocation == -1) {
			__WriteLog2(true, "HitData Or Projectile, Damage Source Is Null");
			ThisStdCall(_actor_OnHitMatter.GetOverwrittenAddr(), _target, _hitData, _unkFlag);
			return;
		}
		

		PrintActorHitData(_hitData);
		auto* _impData = _hitData->projectile->GetImpactDataAlt();
		if (!_impData) {
			__WriteLog2(true, "proj impact data Is Nullptr");
			ThisStdCall(_actor_OnHitMatter.GetOverwrittenAddr(), _target, _hitData, _unkFlag);
			return;
		}

		bool targetIsPlayer = (_target == PlayerCharacter::GetSingleton());

		if (targetIsPlayer){
			__WriteLog2(true, ">>> Target Is PlayerRef");
		}
		else {
			__WriteLog2(true, ">>> Target Is %s", _target->GetEditorID());
		}
		
		
		bool isMeleeHit = WeapIsMelee(_weap, true);
		
		NewArmorDamageCalculation(_hitData, _target, WeapIsMelee(_weap, true), (_target == PlayerCharacter::GetSingleton()), _hitData->hitLocation, IsDebugMode());
		
		ThisStdCall(_actor_OnHitMatter.GetOverwrittenAddr(), _target, _hitData, _unkFlag);
		
		__WriteLog2(true, ">>>>> Print Current Hit Infos Done");
		__WriteLog2(true, "");
	}

	static void __fastcall Actor_OnHitMatter(Actor* _target, void* _edx, ActorHitData* _hitData, bool _unkFlag) {
		if (!_target || !IS_ACTOR(_target)) {
			ThisStdCall(_actor_OnHitMatter.GetOverwrittenAddr(), _target, _hitData, _unkFlag);
			return;
		}

		auto* _proj = _hitData->projectile;
		if (!_hitData || !_proj || !_hitData->source || IS_TYPE(_proj, Explosion) || _hitData->hitLocation == -1) {
			ThisStdCall(_actor_OnHitMatter.GetOverwrittenAddr(), _target, _hitData, _unkFlag);
			return;
		}

		TESObjectWEAP* _weap = _hitData->weapon;
		if (!_weap) {
			ThisStdCall(_actor_OnHitMatter.GetOverwrittenAddr(), _target, _hitData, _unkFlag);
			return;
		}

		auto* _impData = _hitData->projectile->GetImpactDataAlt();
		if (!_impData) {
			ThisStdCall(_actor_OnHitMatter.GetOverwrittenAddr(), _target, _hitData, _unkFlag);
			return;
		}

		NewArmorDamageCalculation(_hitData, _target, WeapIsMelee(_weap, true), 
									(_target == PlayerCharacter::GetSingleton()), _hitData->hitLocation);

		ThisStdCall(_actor_OnHitMatter.GetOverwrittenAddr(), _target, _hitData, _unkFlag);	
	}
	//Actor_OnHitMatter_Explosion_Debug
	static void __fastcall Actor_OnHitMatter_Explosion(Actor* _target, void* _edx, ActorHitData* _hitData, bool _unkFlag) {
		if (!_target || !IS_ACTOR(_target)) {
			ThisStdCall(_actor_OnHitMatter.GetOverwrittenAddr(), _target, _hitData, _unkFlag);
			return;
		}

		auto* _expl = _hitData->explosion;
		BGSExplosion* _baseExp = static_cast<BGSExplosion*>(_expl->baseForm);

		if (!_baseExp || !IS_TYPE(_baseExp,BGSExplosion))
		{
			ThisStdCall(_actor_OnHitMatter.GetOverwrittenAddr(), _target, _hitData, _unkFlag);
			return;
		}

		if (!_hitData || !_expl || !_hitData->source || !IS_TYPE(_expl, Explosion) || _hitData->hitLocation != -1) {
			ThisStdCall(_actor_OnHitMatter.GetOverwrittenAddr(), _target, _hitData, _unkFlag);
			return;
		}

		TESObjectWEAP* _weap = _hitData->weapon;
		if (!_weap) {
			ThisStdCall(_actor_OnHitMatter.GetOverwrittenAddr(), _target, _hitData, _unkFlag);
			return;
		}

		NewArmorDamageCalculation(_hitData, _target, WeapIsMelee(_weap, true),
			(_target == PlayerCharacter::GetSingleton()), _hitData->hitLocation);

		ThisStdCall(_actor_OnHitMatter_Explosion.GetOverwrittenAddr(), _target, _hitData, _unkFlag);
	}

	static void __fastcall Actor_OnHitMatter_Explosion_Debug(Actor* _target, void* _edx, ActorHitData* _hitData, bool _unkFlag) {
		__WriteLog2(true, ">>>>> Print Current Explosion Hit Infos");
		if (!_target || !IS_ACTOR(_target)) {
			__WriteLog2(true, "Target Is Null");
			ThisStdCall(_actor_OnHitMatter.GetOverwrittenAddr(), _target, _hitData, _unkFlag);
			return;
		}

		auto* _expl = _hitData->explosion;
		BGSExplosion* _baseExp = static_cast<BGSExplosion*>(_expl->baseForm);

		if (!_baseExp || !IS_TYPE(_baseExp, BGSExplosion))
		{
			__WriteLog2(true, "Base Explosion Not Exist, Or Not A Explosion");
			ThisStdCall(_actor_OnHitMatter.GetOverwrittenAddr(), _target, _hitData, _unkFlag);
			return;
		}

		if (!_hitData || !_expl || !_hitData->source || !IS_TYPE(_expl, Explosion) || _hitData->hitLocation != -1) {
			__WriteLog2(true, "HitData Or Explosion, Damage Source Is Null");
			ThisStdCall(_actor_OnHitMatter.GetOverwrittenAddr(), _target, _hitData, _unkFlag);
			return;
		}

		TESObjectWEAP* _weap = _hitData->weapon;
		if (!_weap) {
			__WriteLog2(true, "Weap Is Null");
			ThisStdCall(_actor_OnHitMatter.GetOverwrittenAddr(), _target, _hitData, _unkFlag);
			return;
		}
		bool targetIsPlayer = (_target == PlayerCharacter::GetSingleton());
		if (targetIsPlayer) {
			__WriteLog2(true, ">>> Target Is PlayerRef");
		}
		else {
			__WriteLog2(true, ">>> Target Is %s", _target->GetEditorID());
		}

		NewArmorDamageCalculation(_hitData, _target, WeapIsMelee(_weap, true),
			targetIsPlayer, _hitData->hitLocation,IsDebugMode());

		ThisStdCall(_actor_OnHitMatter_Explosion.GetOverwrittenAddr(), _target, _hitData, _unkFlag);
		__WriteLog2(true, ">>>>> Print Current Explosion Hit Infos Done");
	}

/*
	Test Formula
*/
	static __forceinline void FormulaTest(InternalFormulaMng::FormulaClass _internal_which,float val = 0.0f) {
		InternalArgs _args;
		_args.debugSet(val);
		__WriteLog2(true, ">>>> Perform Internal Formula Test For %s, input internal args are: ", singleton<InternalFormulaMng>().GetFormulaIndexName(_internal_which));
		std::string _debugStr = _args.debugPrintStr();
		__WriteLog2(true, _debugStr.c_str());

		float _res = singleton<InternalFormulaMng>().calcResult(_internal_which, _args, false);

		__WriteLog2(true, "Result: %.2f ",_res);
		__WriteLog2(true, ">>>> Perform Formula Test For %s, Done ", singleton<InternalFormulaMng>().GetFormulaIndexName(_internal_which));
		__WriteLog2(true, "");
	}

	static __forceinline void FormulaTest(CustomFormulaMng::FormulaIndex _custom_which, float val = 0.0f) {
		InternalArgs _args;
		_args.debugSet(val);
		__WriteLog2(true, ">>>> Perform Formula Test For %s, input internal args are: ", CustomFormulaMng::GetFormulaIndexName(_custom_which));
		std::string _debugStr = _args.debugPrintStr();
		__WriteLog2(true, _debugStr.c_str());

		float _res = singleton<CustomFormulaMng>().calcResult(_custom_which, _args, false);

		__WriteLog2(true, "Result: %.2f ", _res);
		__WriteLog2(true, ">>>> Perform Formula Test For %s, Done ", CustomFormulaMng::GetFormulaIndexName(_custom_which));
		__WriteLog2(true, "");
	}


	namespace fs = std::filesystem;
	
	static bool ReadInternalFormulaConfig(InternalFormulaMng::FormulaClass _class, InternalFormulaMng::FormulaIndex _fmIndex) {

		UINT32 _index = (UINT32)_fmIndex;
		std::string _strIndex = std::to_string(_index);
		fs::path readingDirectory = std::filesystem::current_path().append(R"(Data\NVSE\Plugins\ArmorDamageReformulaConfig\InternalFormulaConfig\)");
		__WriteLog2(true, "Read Internal Formula Directory: %s", readingDirectory.string());
		if (fs::exists(readingDirectory) && fs::is_directory(readingDirectory)) {
			for (const auto& _entry : fs::directory_iterator(readingDirectory))
			{
				if (_entry.path().extension() != ".txt")
					continue;

				if (const auto& _pStr = _entry.path().filename().string();!_pStr.empty()  )
				{
					std::string _FileName = _pStr;
					_FileName.erase(std::remove(_FileName.begin(), _FileName.end(), ' '), _FileName.end());
					if (_FileName.find(_strIndex) != (size_t)0)
					{
						continue;
					}


					std::ifstream file;
					file.open(_entry.path(), std::ios::in);

					if (!file.is_open()) {
						__WriteLog2(true, "Open File Failed: %s", _entry.path().string());
						continue;
					}
					__WriteLog2(true, "Open File Success: %s", _entry.path().string());
					std::string _line{};

					while (std::getline(file, _line)) {
						if (_line.empty())
							continue;
						// trim space
						_line.erase(std::remove(_line.begin(), _line.end(), ' '), _line.end());
						size_t _splitPos = _line.find('=');
						
						if (_line.at(0) == ';' || _splitPos == std::string::npos)
							continue;

						std::string _argName = _line.substr(0,_splitPos);
						std::string _sArgValue = _line.substr(_splitPos+1);

						if (_argName.empty() || _sArgValue.empty() || !std::isdigit(_sArgValue.at(0)))
						{
							continue;
						}

						float _argValue = std::stof(_sArgValue);
						std::stringstream _ss;
						_ss << "Internal Arg Setting: " << _argName << " = " << _argValue;
						__WriteLog2(true, _ss.str().c_str());
						singleton<InternalFormulaMng>().addArgSetting(_class,_argName, _argValue);
					}
					break;
				}
			}
		}
	}

	static bool ReadCustomFormulaConfig(CustomFormulaMng::FormulaIndex _which, const std::string& _formulaFileNameStr) {
		std::string _formulaFileName = _formulaFileNameStr;
		if (_formulaFileName.find(".txt") == std::string::npos){
			_formulaFileName.append(".txt");
		}

		fs::path config_root_path = fs::current_path();
		config_root_path += R"(\Data\NVSE\Plugins\ArmorDamageReformulaConfig\CustomFormulaConfig\)";
		config_root_path += _formulaFileName;
		if (!fs::exists(config_root_path)) {
			__WriteLog2(true, "Custom Formula File Not exist %s", config_root_path.string());
			return false;
		} 


		std::ifstream file;
		file.open(config_root_path, std::ios::in);

		if (!file.is_open()) {
			__WriteLog2(true, "Open File Failed: %s", config_root_path.string());
		}
		__WriteLog2(true, "Open File Success: %s", config_root_path.string());
		std::string _line{};

		auto lineHasAlpha = [](const std::string& _line) -> bool {
			int _cnt = 0;
			for (char _ch : _line){
				if (std::isalpha(_ch))
					_cnt++;
			}
			return _cnt > 0;
		};

		while (std::getline(file, _line)) {
			if (_line.empty() || !lineHasAlpha(_line))
				continue;
			// trim space
			_line.erase(std::remove(_line.begin(), _line.end(), ' '), _line.end());
			std::stringstream _debugSS;
			_debugSS << "Set Custom Formula (" << CustomFormulaMng::GetFormulaIndexName(_which) << ") As: " << _line.c_str();
			__WriteLog2(true, _debugSS.str().c_str());
			InitCustomFormulaMng(_which, _line);
			break;
		}

		return true;
	}

	static bool ReadGeneralConfig() {
		gLog.Message("ReadGenericConfig");
		fs::path config_root_path = fs::current_path();
		config_root_path += R"(\Data\NVSE\Plugins\ArmorDamageReformulaConfig\)";
		if (!fs::exists(config_root_path)) {
			gLog.Message("ReadGenericConfig path not exist");
			return false;
		}

		roughinireader::INIReader _ini{ config_root_path };

		auto ret = _ini.SetCurrentINIFileName("ArmorDamageFormulaConfig.ini");
		if (!ret.has_value()) {
			gLog.FormattedMessage("Failed to set generic config filename : %s", ret.error().message());
			return false;
		}
		ret = _ini.ConstructSectionMap();
		if (!ret.has_value()) {
			gLog.FormattedMessage("Failed to construct section map : %s", ret.error().message());
			return false;
		}

		std::string raw_type_val = "";
		UINT32 temp_flag = 0;

		raw_type_val = _ini.GetRawTypeVal("General", "DebugMode");
		temp_flag = raw_type_val.empty() ? 0 : static_cast<UINT32>(std::stoi(raw_type_val));
		gLog.FormattedMessage(raw_type_val.c_str());
		if (temp_flag != 0)
			s_reformula_flag.SetFlagOn(ReformulaSetting::DebugMode);

/*
	Custom Formula
*/

		raw_type_val = _ini.GetRawTypeVal("General", "CustomFormulaOfRangedHit");
		temp_flag = raw_type_val.empty() ? 0 : static_cast<UINT32>(std::stoi(raw_type_val));
		if (temp_flag != 0)
			s_reformula_flag.SetFlagOn(ReformulaSetting::CustomFormulaForRangedHit);

		raw_type_val = _ini.GetRawTypeVal("General", "CustomFormulaOfMeleeHit");
		temp_flag = raw_type_val.empty() ? 0 : static_cast<UINT32>(std::stoi(raw_type_val));
		if (temp_flag != 0)
			s_reformula_flag.SetFlagOn(ReformulaSetting::CustomFormulaForMeleeHit);

		raw_type_val = _ini.GetRawTypeVal("General", "CustomFormulaOfExplosionHit");
		temp_flag = raw_type_val.empty() ? 0 : static_cast<UINT32>(std::stoi(raw_type_val));
		if (temp_flag != 0)
			s_reformula_flag.SetFlagOn(ReformulaSetting::CustomFormulaForExplosionHit);

/*   Ranged Hit Formula   */
		if (s_reformula_flag.IsFlagOn(ReformulaSetting::CustomFormulaForRangedHit))
		{
			raw_type_val = _ini.GetRawTypeVal("General", "FormulaFileForRangedHit");
			ReadCustomFormulaConfig(CustomFormulaMng::FormulaIndex::RangedHit,raw_type_val);
			FormulaTest(CustomFormulaMng::FormulaIndex::RangedHit);
			FormulaTest(CustomFormulaMng::FormulaIndex::RangedHit, 10.0f);
		}
		else {
			raw_type_val = _ini.GetRawTypeVal("General", "InternalFormulaIndexOfRangedHit");
			InternalFormulaMng::FormulaIndex _index = (InternalFormulaMng::FormulaIndex)(raw_type_val.empty() ? 0 : (std::stol(raw_type_val)));
			singleton<InternalFormulaMng>().setInternalFormulaIndex(InternalFormulaMng::FormulaClass::RangedHit,_index);
			ReadInternalFormulaConfig(InternalFormulaMng::FormulaClass::RangedHit, _index);
			FormulaTest(InternalFormulaMng::FormulaClass::RangedHit);
			FormulaTest(InternalFormulaMng::FormulaClass::RangedHit,10.0f);
		}

/*   Melee Hit Formula   */
		if (s_reformula_flag.IsFlagOn(ReformulaSetting::CustomFormulaForMeleeHit))
		{
			raw_type_val = _ini.GetRawTypeVal("General", "FormulaFileForMeleeHit");
			ReadCustomFormulaConfig(CustomFormulaMng::FormulaIndex::MeleeHit, raw_type_val);
			FormulaTest(CustomFormulaMng::FormulaIndex::MeleeHit);
			FormulaTest(CustomFormulaMng::FormulaIndex::MeleeHit, 10.0f);
		}
		else {
			raw_type_val = _ini.GetRawTypeVal("General", "InternalFormulaIndexOfMeleeHit");
			InternalFormulaMng::FormulaIndex _index = (InternalFormulaMng::FormulaIndex)(raw_type_val.empty() ? 1 : (std::stol(raw_type_val)));
			singleton<InternalFormulaMng>().setInternalFormulaIndex(InternalFormulaMng::FormulaClass::MeleeHit,_index);
			ReadInternalFormulaConfig(InternalFormulaMng::FormulaClass::MeleeHit, _index);
			FormulaTest(InternalFormulaMng::FormulaClass::MeleeHit);
			FormulaTest(InternalFormulaMng::FormulaClass::MeleeHit, 10.0f);
		}

/*   Explosion Hit Formula   */
		if (s_reformula_flag.IsFlagOn(ReformulaSetting::CustomFormulaForExplosionHit))
		{
			raw_type_val = _ini.GetRawTypeVal("General", "FormulaFileForExplosionHit");
			ReadCustomFormulaConfig(CustomFormulaMng::FormulaIndex::ExplosionHit, raw_type_val);
			FormulaTest(CustomFormulaMng::FormulaIndex::ExplosionHit);
			FormulaTest(CustomFormulaMng::FormulaIndex::ExplosionHit, 10.0f);
		}
		else {
			raw_type_val = _ini.GetRawTypeVal("General", "InternalFormulaIndexOfExplosionHit");
			InternalFormulaMng::FormulaIndex _index = (InternalFormulaMng::FormulaIndex)(raw_type_val.empty() ? 2 : (std::stol(raw_type_val)));
			singleton<InternalFormulaMng>().setInternalFormulaIndex(InternalFormulaMng::FormulaClass::ExplosionHit, _index);
			ReadInternalFormulaConfig(InternalFormulaMng::FormulaClass::ExplosionHit, _index);
			FormulaTest(InternalFormulaMng::FormulaClass::ExplosionHit);
			FormulaTest(InternalFormulaMng::FormulaClass::ExplosionHit, 10.0f);
		}
	}

	static void __fastcall TestHook(Actor* _victim) {
		if (_victim == PlayerCharacter::GetSingleton())
		{
			PrintCallStack();
		}
		ThisStdCall(_testHook.GetOverwrittenAddr(), _victim);
	}

	static void InstallHook() {
		//_actorHitData_SetDMG.WriteRelCall(0x9B5702, (UINT32)ActorHitData_SetDmgHook);
		//_testHook.WriteRelCall(0x89A7BD, (UINT32)TestHook);
		_actor_OnHitMatter.WriteRelCall(0x89A738, IsDebugMode() ? (UINT32)Actor_OnHitMatter_Debug : (UINT32)Actor_OnHitMatter);
		_actor_OnHitMatter_Explosion.WriteRelCall(0x9B0503, IsDebugMode() ? (UINT32)Actor_OnHitMatter_Explosion_Debug : (UINT32)Actor_OnHitMatter_Explosion);
	}
};



// This is a message handler for nvse events
// With this, plugins can listen to messages such as whenever the game loads
void MessageHandler(NVSEMessagingInterface::Message* msg)
{
	switch (msg->type)
	{
	case NVSEMessagingInterface::kMessage_DeferredInit:
		initSingleton();
		ArmorReformula::ReadGeneralConfig();
		ArmorReformula::InstallHook();

		break;
	default:
		break;
	}
}

bool NVSEPlugin_Load(NVSEInterface* nvse)
{
	_MESSAGE("MissileHook load");
	g_pluginHandle = nvse->GetPluginHandle();
	// save the NVSE interface in case we need it later
	g_nvseInterface = nvse;
	NVSEDataInterface* nvseData = (NVSEDataInterface*)nvse->QueryInterface(kInterface_Data);
	InventoryRefGetForID = (_InventoryRefGetForID)nvseData->GetFunc(NVSEDataInterface::kNVSEData_InventoryReferenceGetForRefID);
	InventoryRefCreate = (_InventoryRefCreate)nvseData->GetFunc(NVSEDataInterface::kNVSEData_InventoryReferenceCreateEntry);

	// register to receive messages from NVSE

	if (!nvse->isEditor)
	{
		g_messagingInterface = static_cast<NVSEMessagingInterface*>(nvse->QueryInterface(kInterface_Messaging));
		g_messagingInterface->RegisterListener(g_pluginHandle, "NVSE", MessageHandler);
	}
	return true;
}