#pragma once
#include "Gathering_Utility.h"
#include "Gathering_Node.h"
#include "RoughINIReader.h"

#include "exprtk.hpp"

template <class T>
size_t constexpr ToUInt(T _type) {
	return static_cast<size_t>(_type);
}

struct InternalArgs {
	/*
	EffectiveDT: Actual DT in current hit processing.Not Base DT.
	EffectiveDR: Actual DR in current hit processing.Not Base DR.
	HealthDamage: Health Damage in current hit processing
	RawDamage: Damage that not Involve any calculation
	WeaponDamage: Weapon Damage
	WeaponDamage: Weapon Weight
	*/
	enum class Index {
		EffectiveDT, EffectiveDR, HealthDamage, RawDamage,WeaponDamage,WeaponWeight, ArgsCount
	};
	/*
		InputArgs is for custom formula flag.
		As bitmask of what internal args the custom furmula will use
	*/
	enum class InputArgs {
		EffectiveDT = 0x0001, EffectiveDR = 0x0002, HealthDamage = 0x0004, RawDamage = 0x0008,
		WeaponDamage = 0x0010, WeaponWeight = 0x0020,None = 0x10000000
	};

#define ArgMapped(_argName) case InternalArgs::Index::_argName:\
								return InputArgs::_argName

	static __forceinline InputArgs GetInputArgByIndex(InternalArgs::Index _idx) {
		switch (_idx)
		{
			ArgMapped(EffectiveDT);
			ArgMapped(EffectiveDR);
			ArgMapped(HealthDamage);
			ArgMapped(RawDamage);
			ArgMapped(WeaponDamage);
			ArgMapped(WeaponWeight);
		case InternalArgs::Index::ArgsCount:
		default:
			return InputArgs::None;
		}
	}
#define InputArgStrMapped(_argName,_mapped) case InputArgs::_argName:\
										return _mapped

#define ArgIndexStrMapped(_argName,_mapped) case Index::_argName:\
										return _mapped
	static __forceinline const char* getArgName(InputArgs _arg) {
		switch (_arg)
		{
			InputArgStrMapped(EffectiveDT, "effective_dt");
			InputArgStrMapped(EffectiveDR, "effective_dr");
			InputArgStrMapped(HealthDamage, "health_damage");
			InputArgStrMapped(RawDamage, "raw_damage");
			InputArgStrMapped(WeaponDamage, "weapon_damage");
			InputArgStrMapped(WeaponWeight, "weapon_weight");
		default:
			return "";
		}
		return "";
	}

	static __forceinline const char* getArgName(Index _idx) {
		switch (_idx)
		{
			ArgIndexStrMapped(EffectiveDT, "effective_dt");
			ArgIndexStrMapped(EffectiveDR, "effective_dr");
			ArgIndexStrMapped(HealthDamage, "health_damage");
			ArgIndexStrMapped(RawDamage, "raw_damage");
			ArgIndexStrMapped(WeaponDamage, "weapon_damage");
			ArgIndexStrMapped(WeaponWeight, "weapon_weight");
		default:
			return "";
		}
		return "";
	}

	__forceinline void debugSet(float _uniVal) { 
		_args.fill(_uniVal);
	}

	__forceinline std::string debugPrintStr() const {
		std::stringstream _ss{};
		for (size_t idx = ToUInt(Index::EffectiveDT); idx < ToUInt(Index::ArgsCount); idx++)
		{
			char _end = '\n';
			if (idx == (ToUInt(Index::ArgsCount) - 1) )
			{
				_end = ' ';
			}
			_ss << "Arg( " << getArgName((Index)idx) << " ) = " << _args.at(idx) << _end;
		}
		return _ss.str();
	}

	std::array <float, ToUInt(Index::ArgsCount)> _args = { 0.0f };
	void __forceinline setInternalArg(Index _idx, float _val) { _args[ToUInt(_idx)] = _val; }
	float __forceinline getInternalArg(Index _idx) const { return _args.at(ToUInt(_idx)); }
#define ExtraInternalArg(_varName,_internalIndex) _varName.getInternalArg(InternalArgs::Index::_internalIndex)
};

struct CalcResult {
	/* CalcResult{ false,0.0 } */
	CalcResult() = default;

	bool __forceinline isCalcSuccess() const { return _success; }
	void __forceinline setCalcSuccess(bool _set) { _success = _set; }
	float __forceinline getCalcResult() const { return _res; }
	void __forceinline setCalcResult(float _set) { _res = _set; }
	RoughHitLocation __forceinline getRoughHitLocation() const { return _rHitLoc; }
	void __forceinline setRoughHitLocation(RoughHitLocation _rHL) { _rHitLoc = _rHL; }
private:
	bool _success = false;
	float _res = 0.0;
	RoughHitLocation _rHitLoc = RoughHitLocation::RHL_None;
};


typedef exprtk::symbol_table<float> f_SymbolTable;
typedef exprtk::expression<float>   f_Expression;
typedef exprtk::parser<float>       f_Parser;
using InputArgs = InternalArgs::InputArgs;
/*
		For Custom Formula
*/
struct CustomFormulaMng {

	enum class FormulaIndex {
		RangedHit,MeleeHit,ExplosionHit
	};

	static const char* GetFormulaIndexName(FormulaIndex _which){
		switch (_which)
		{
		case CustomFormulaMng::FormulaIndex::RangedHit:
			return "Ranged Hit";
		case CustomFormulaMng::FormulaIndex::MeleeHit:
			return "Melee Hit";
		case CustomFormulaMng::FormulaIndex::ExplosionHit:
			return "Explosion Hit";
		default:
			return "";
		}
	}



	typedef OptFlag<InputArgs>		Arg_enable_flag;

	CustomFormulaMng(CustomFormulaMng&&) = delete;
	CustomFormulaMng() = default;


	

	__forceinline void setFormual(CustomFormulaMng::FormulaIndex _which, const std::string& _str) {
		std::string& _formula = getCustomFormulaStr(_which);
		_formula = _str;
		for (char& _ch : _formula)
		{
			if (std::isalpha(_ch))
			{
				_ch = std::tolower(_ch);
			}
		}
		buildArgsSet(_which);
	}

	__forceinline bool argValid(FormulaIndex _wihch, InputArgs _arg) {
		return getInternalArgFlagByIndex(_wihch).IsFlagOn(_arg);
	}

	__forceinline Arg_enable_flag getInternalArgFlagByIndex(FormulaIndex _which) const {
		switch (_which)
		{
		case CustomFormulaMng::FormulaIndex::RangedHit:
			return _internalArgsFlag_rangedHit;
		case CustomFormulaMng::FormulaIndex::MeleeHit:
			return _internalArgsFlag_meleeHit;
		case CustomFormulaMng::FormulaIndex::ExplosionHit:
			return _internalArgsFlag_explosionHit;
		default:
			break;
		}
	}
	/*
	Calculate by custom formula
	*/
	__forceinline CalcResult calcByNewFormula(CustomFormulaMng::FormulaIndex _which, const InternalArgs& _internalArgs, bool debugMode) {
		CalcResult ret{};
		__WriteLog2(true,"Custom Formula( %s ): ",CustomFormulaMng::GetFormulaIndexName(_which));
		__WriteLog2(true,"%s ", getCustomFormulaStrCst(_which).c_str());

		f_SymbolTable sTbl;
		auto tryAddVariable = [this, &sTbl, _which](InputArgs _arg, float _val) {
			if (argValid(_which,_arg))
				sTbl.add_constant(InternalArgs::getArgName(_arg), _val);
			};

		//#define AddArg(is_debug_mode,_InputArg,_ArgVal) if (argValid(_InputArg)){tryAddVariable(_InputArg, _ArgVal);}



#define AddArg(is_debug_mode,_InputArg,_ArgVal) if (!is_debug_mode){\
													tryAddVariable(_InputArg, _ArgVal);\
												}\
												else{\
													float _val = _ArgVal;\
													tryAddVariable(_InputArg, _val);\
													std::stringstream _ss{};\
													_ss << "Input Internal Arg Name: " << InternalArgs::getArgName(_InputArg) << " = " << _val; \
													gLog.FormattedMessage(_ss.str().c_str());\
												}
		AddArg(debugMode, InputArgs::EffectiveDT, ExtraInternalArg(_internalArgs, EffectiveDT));
		AddArg(debugMode, InputArgs::EffectiveDR, ExtraInternalArg(_internalArgs, EffectiveDR));
		AddArg(debugMode, InputArgs::HealthDamage, ExtraInternalArg(_internalArgs, HealthDamage));
		AddArg(debugMode, InputArgs::RawDamage, ExtraInternalArg(_internalArgs, RawDamage));
		AddArg(debugMode, InputArgs::WeaponDamage, ExtraInternalArg(_internalArgs, WeaponDamage));
		AddArg(debugMode, InputArgs::WeaponWeight, ExtraInternalArg(_internalArgs, WeaponWeight));

		f_Expression expression;
		if (!expression.register_symbol_table(sTbl))
		{
			__WriteLog2(true, "Register Symbol For Expression Failed!");
			return ret;
		}

		f_Parser parser;
		if (!parser.compile(getCustomFormulaStrCst(_which), expression))
		{
			__WriteLog2(true, ">>>>>Parser Compiles Failed!<<<<<");
			__WriteLog2(true, "Error Count: %u", parser.error_count());
			for (std::size_t i = 0; i < parser.error_count(); ++i)
			{
				const exprtk::parser_error::type error = parser.get_error(i);
				std::stringstream _errSS{};
				_errSS << "ErrorIndex: " << i << " Position: " << static_cast<unsigned int>(error.token.position)
					<< " Type: " << exprtk::parser_error::to_str(error.mode).c_str()
					<< " Msg: " << error.diagnostic.c_str()
					<< " Expression: " << getCustomFormulaStrCst(_which).c_str();
				__WriteLog2(true, _errSS.str().c_str());
			}
			return ret;
		}
		ret.setCalcSuccess(true);
		ret.setCalcResult(expression.value());
		__WriteLog2(true, "Calc Success: %.2f", ret.getCalcResult());
		return ret;
	}

	__forceinline float calcResult(CustomFormulaMng::FormulaIndex _which, const InternalArgs& _internalArgs, bool debugMode) {
		f_SymbolTable sTbl;
		auto tryAddVariable = [this, &sTbl, _which](InputArgs _arg, float _val) {
			if (argValid(_which, _arg))
				sTbl.add_constant(InternalArgs::getArgName(_arg), _val);
			};

		//#define AddArg(is_debug_mode,_InputArg,_ArgVal) if (argValid(_InputArg)){tryAddVariable(_InputArg, _ArgVal);}



#define AddArg(is_debug_mode,_InputArg,_ArgVal) if (!is_debug_mode){\
													tryAddVariable(_InputArg, _ArgVal);\
												}\
												else{\
													float _val = _ArgVal;\
													tryAddVariable(_InputArg, _val);\
													std::stringstream _ss{};\
													_ss << "InputArgName: " << InternalArgs::getArgName(_InputArg) << " = " << _val; \
													gLog.FormattedMessage(_ss.str().c_str());\
												}
		AddArg(debugMode, InputArgs::EffectiveDT, ExtraInternalArg(_internalArgs, EffectiveDT));
		AddArg(debugMode, InputArgs::EffectiveDR, ExtraInternalArg(_internalArgs, EffectiveDR));
		AddArg(debugMode, InputArgs::HealthDamage, ExtraInternalArg(_internalArgs, HealthDamage));
		AddArg(debugMode, InputArgs::RawDamage, ExtraInternalArg(_internalArgs, RawDamage));
		AddArg(debugMode, InputArgs::WeaponDamage, ExtraInternalArg(_internalArgs, WeaponDamage));
		AddArg(debugMode, InputArgs::WeaponWeight, ExtraInternalArg(_internalArgs, WeaponWeight));

		f_Expression expression;
		if (!expression.register_symbol_table(sTbl))
		{
			__WriteLog2(true, "Register Symbol For Expression Failed!");
			return -1;
		}

		f_Parser parser;
		if (!parser.compile(getCustomFormulaStrCst(_which), expression))
		{
			__WriteLog2(true, ">>>>>Parser Compiles Failed!<<<<<");
			__WriteLog2(true, "Error Count: %u", parser.error_count());
			for (std::size_t i = 0; i < parser.error_count(); ++i)
			{
				const exprtk::parser_error::type error = parser.get_error(i);
				std::stringstream _errSS{};
				_errSS << "ErrorIndex: " << i << " Position: " << static_cast<unsigned int>(error.token.position)
					<< " Type: " << exprtk::parser_error::to_str(error.mode).c_str()
					<< " Msg: " << error.diagnostic.c_str()
					<< " Expression: " << getCustomFormulaStrCst(_which).c_str();
				__WriteLog2(true, _errSS.str().c_str());
			}
			return -1;
		}
		
		return expression.value();
	}

private:
	// Marks which internal args will be used in custom formula
	Arg_enable_flag _internalArgsFlag_rangedHit{};
	Arg_enable_flag _internalArgsFlag_meleeHit{};
	Arg_enable_flag _internalArgsFlag_explosionHit{};
	std::string _customFormulaStr_rangedHit = "";
	std::string _customFormulaStr_meleeHit = "";
	std::string _customFormulaStr_explosionHit = "";

	__forceinline const std::string& getCustomFormulaStrCst(CustomFormulaMng::FormulaIndex _which) const {
		switch (_which)
		{
		case CustomFormulaMng::FormulaIndex::RangedHit:
			return _customFormulaStr_rangedHit;
		case CustomFormulaMng::FormulaIndex::MeleeHit:
			return _customFormulaStr_meleeHit;
		case CustomFormulaMng::FormulaIndex::ExplosionHit:
			return _customFormulaStr_explosionHit;
		default:
			return _customFormulaStr_rangedHit;
		}
	}
	
	__forceinline std::string& getCustomFormulaStr(CustomFormulaMng::FormulaIndex _which)  {
		switch (_which)
		{
		case CustomFormulaMng::FormulaIndex::RangedHit:
			return _customFormulaStr_rangedHit;
		case CustomFormulaMng::FormulaIndex::MeleeHit:
			return _customFormulaStr_meleeHit;
		case CustomFormulaMng::FormulaIndex::ExplosionHit:
			return _customFormulaStr_explosionHit;
		default:
			return _customFormulaStr_rangedHit;
		}
	}

	__forceinline bool hasArgInFormula(CustomFormulaMng::FormulaIndex _which,const char* arg_name) const {
		return getCustomFormulaStrCst(_which).find(arg_name) != std::string::npos;
	}

	__forceinline void buildArgsSet(CustomFormulaMng::FormulaIndex _which) {
		for (size_t idx = (size_t)InternalArgs::Index::EffectiveDT; idx < (size_t)InternalArgs::Index::ArgsCount; idx++) {
			InputArgs _inputArg = InternalArgs::GetInputArgByIndex(static_cast<InternalArgs::Index>(idx));

			const char* _argName = InternalArgs::getArgName(_inputArg);
			if (hasArgInFormula(_which, _argName)) {
				switch (_which)
				{
				case CustomFormulaMng::FormulaIndex::RangedHit:
					_internalArgsFlag_rangedHit.SetFlagOn(_inputArg);
					break;
				case CustomFormulaMng::FormulaIndex::MeleeHit:
					_internalArgsFlag_meleeHit.SetFlagOn(_inputArg);
					break;
				case CustomFormulaMng::FormulaIndex::ExplosionHit:
					_internalArgsFlag_explosionHit.SetFlagOn(_inputArg);
					break;
				default:
					break;
				}
			}
		}
	}
};


using internal_formula_func = std::function<float(const InternalArgs&)>;

class InternalFormulaMng {
	InternalFormulaMng(InternalFormulaMng&&) = delete;
public:
	enum class FormulaClass {
		RangedHit, MeleeHit, ExplosionHit,Count
	};

	enum class FormulaIndex {
		Lowbeebob_projectile, Lowbeebob_melee, Lowbeebob_explosion,Count
	};
private:
	std::array<internal_formula_func, ToUInt(FormulaIndex::Count)> _formualArray {};

	InternalFormulaMng::FormulaIndex _internalRangedFormula{ FormulaIndex::Lowbeebob_projectile };
	InternalFormulaMng::FormulaIndex _internalMeleeFormula{ FormulaIndex::Lowbeebob_melee };
	InternalFormulaMng::FormulaIndex _internalExplFormula{ FormulaIndex::Lowbeebob_explosion };
	// These settings are reading form config
	std::unordered_map<std::string, float> _argMapForRangedHit{};
	std::unordered_map<std::string, float> _argMapForMeleeHit{};
	std::unordered_map<std::string, float> _argMapForExplosionHit{};

	std::unordered_map<std::string, float>& getArgMap(FormulaClass _class) {
		switch (_class)
		{
		case InternalFormulaMng::FormulaClass::RangedHit:
			return _argMapForRangedHit;
		case InternalFormulaMng::FormulaClass::MeleeHit:
			return _argMapForMeleeHit;
		case InternalFormulaMng::FormulaClass::ExplosionHit:
			return _argMapForExplosionHit;
		case InternalFormulaMng::FormulaClass::Count:
		default:
			return _argMapForRangedHit;
		}
	}

	__forceinline FormulaIndex& GetInternalFormulaIndexRef(FormulaClass _class) {
		switch (_class)
		{
		case InternalFormulaMng::FormulaClass::RangedHit:
			return _internalRangedFormula;
		case InternalFormulaMng::FormulaClass::MeleeHit:
			return _internalMeleeFormula;
		case InternalFormulaMng::FormulaClass::ExplosionHit:
			return _internalExplFormula;
		case InternalFormulaMng::FormulaClass::Count:
		default:
			return _internalRangedFormula;
		}
		return _internalRangedFormula;
	}
	

	float __forceinline getArgSetting(FormulaClass _class, const std::string& _argName) {
		const auto& _argMap = getArgMap(_class);
		if (const auto& _argItor = _argMap.find(_argName); _argItor != _argMap.end())
		{
			return _argItor->second;
		}
		return 0.0f;
	}

	float __forceinline callFormula(FormulaClass _class, const InternalArgs& _args) {
		FormulaIndex _index = GetInternalFormulaIndex(_class);
		return _formualArray.at(ToUInt(_index))(_args);
	}

public:
	__forceinline FormulaIndex GetInternalFormulaIndex(FormulaClass _class) const {
		switch (_class)
		{
		case InternalFormulaMng::FormulaClass::RangedHit:
			return _internalRangedFormula;
		case InternalFormulaMng::FormulaClass::MeleeHit:
			return _internalMeleeFormula;
		case InternalFormulaMng::FormulaClass::ExplosionHit:
			return _internalExplFormula;
		case InternalFormulaMng::FormulaClass::Count:
		default:
			return _internalRangedFormula;
		}
		return _internalRangedFormula;
	}

	std::stringstream __forceinline DebugInfosStream(FormulaClass _class,FormulaIndex _who) {
		const auto& _args = getArgMap(_class);
		
		std::stringstream _ss{};
		_ss << ">>>Print Internal Formual: " << GetFormulaIndexName(_who) << '\n';

		for (const auto& pair : _args)
		{
			_ss << "ArgSettingName: " << pair.first << " = " << pair.second << '\n';
		}

		return _ss;
	}
	
	float __forceinline calcResult(FormulaClass _class, const InternalArgs& _args, bool isDebugMode) {
#define RegisterInternalFormula(_index) case  _index:\
												return callFormula(_index,_args)

		if (isDebugMode){
			std::string _debugInfo = _args.debugPrintStr();
			__WriteLog2(true, ">>> Print Internal Arguments:");
			__WriteLog2(true, _debugInfo.c_str());
		}

		switch (_class)
		{
			RegisterInternalFormula(FormulaClass::RangedHit);
			RegisterInternalFormula(FormulaClass::MeleeHit);
			RegisterInternalFormula(FormulaClass::ExplosionHit);
		default:
			return -1.0f;
		}
	}

	__forceinline void setInternalFormulaIndex(FormulaClass _class, FormulaIndex _index) {
		if (_index < FormulaIndex::Count)
		{
			GetInternalFormulaIndexRef(_class) = _index;
		}
	}

	__forceinline void setInternalFormulaIndex(FormulaClass _class,UINT32 _index) {
		setInternalFormulaIndex(_class,static_cast<FormulaIndex>(_index));
	}

	void __forceinline addArgSetting(FormulaClass _class, const std::string& _argName, float _val) {
		std::string _temp = _argName;
		for (char& _ch : _temp) {
			_ch = std::tolower(_ch);
		}

		getArgMap(_class).emplace(_temp, _val);
	}

	
	__forceinline const char* GetFormulaIndexName(FormulaClass _class) {
		FormulaIndex _index = GetInternalFormulaIndex(_class);
		return GetFormulaIndexName(_index);
	}

	static __forceinline const char* GetFormulaIndexName(FormulaIndex _fmIndex) {
		switch (_fmIndex)
		{
		case InternalFormulaMng::FormulaIndex::Lowbeebob_projectile:
			return "Lowbeebob_projectile";
		case InternalFormulaMng::FormulaIndex::Lowbeebob_melee:
			return "Lowbeebob_melee";
		case InternalFormulaMng::FormulaIndex::Lowbeebob_explosion:
			return "Lowbeebob_explosion";
		case InternalFormulaMng::FormulaIndex::Count:
			return "Invalid";
		default:
			return "Invalid";
		}
	}

	CalcResult __forceinline calcByNewFormula(FormulaClass _class, const InternalArgs& _args, bool _isDebugMode) {
		CalcResult _ret;

		float _res = calcResult(_class, _args, _isDebugMode);
		if (_res < 0)
		{
			return _ret;
		}

		_ret.setCalcResult(_res);
		_ret.setCalcSuccess(true);
		return _ret;
	}


#define SetFormula(_who,_formula_func) _formualArray[ToUInt(_who)] = _formula_func
	InternalFormulaMng() {
		SetFormula(FormulaIndex::Lowbeebob_projectile, [this](const InternalArgs& _args) -> float {
			float _effectDR = ExtraInternalArg(_args, EffectiveDR);
			float _minScale = _effectDR <= 0 ? getArgSetting(FormulaClass::RangedHit, "minscale") : std::clamp(1 - (_effectDR  / 100), getArgSetting(FormulaClass::RangedHit, "minscale"), 1.0f);
			
			float _maxScale = getArgSetting(FormulaClass::RangedHit, "maxscale") > _minScale ? getArgSetting(FormulaClass::RangedHit, "maxscale") : _minScale;
			float _healthDmg = ExtraInternalArg(_args, HealthDamage);
			float _effectDT = ExtraInternalArg(_args, EffectiveDT);
			return (_effectDT <= 0 ? _maxScale : std::clamp((_healthDmg / _effectDT),
				_minScale , _maxScale) )*
				ExtraInternalArg(_args, RawDamage) * getArgSetting(FormulaClass::RangedHit, "mult");
		});
		 
		SetFormula(FormulaIndex::Lowbeebob_melee, [this](const InternalArgs& _args) -> float {
			float _effectDR = ExtraInternalArg(_args, EffectiveDR);
			float _minScale = _effectDR <= 0 ? getArgSetting(FormulaClass::RangedHit, "minscale") : std::clamp(1 - (_effectDR / 100), getArgSetting(FormulaClass::RangedHit, "minscale"), 1.0f);

			float _maxScale = getArgSetting(FormulaClass::MeleeHit, "maxscale") > _minScale ? getArgSetting(FormulaClass::MeleeHit, "maxscale") : _minScale;
			float _healthDmg = ExtraInternalArg(_args, HealthDamage);
			float _effectDT = ExtraInternalArg(_args, EffectiveDT);
			return (_effectDT <= 0 ? _maxScale : std::clamp((_healthDmg / _effectDT),
				_minScale, _maxScale) )*
				ExtraInternalArg(_args, WeaponDamage) * getArgSetting(FormulaClass::MeleeHit, "mult") * ExtraInternalArg(_args, WeaponWeight);
			});
		
		SetFormula(FormulaIndex::Lowbeebob_explosion, [this](const InternalArgs& _args) -> float {
			float _effectDR = ExtraInternalArg(_args, EffectiveDR);
			float _minScale = _effectDR <= 0 ? getArgSetting(FormulaClass::ExplosionHit, "minscale") : std::clamp(1 - (_effectDR / 100), getArgSetting(FormulaClass::ExplosionHit, "minscale"), 1.0f);
		

			float _maxScale = getArgSetting(FormulaClass::ExplosionHit, "maxscale") > _minScale ? getArgSetting(FormulaClass::ExplosionHit, "maxscale") : _minScale;
			float _healthDmg = ExtraInternalArg(_args, HealthDamage);
			float _effectDT = ExtraInternalArg(_args, EffectiveDT);
			return (_effectDT <= 0 ? _maxScale : std::clamp((_healthDmg / _effectDT),
				_minScale, _maxScale) )*
				ExtraInternalArg(_args, RawDamage) * getArgSetting(FormulaClass::ExplosionHit, "mult");
			});

	}
};


/*
	Check Actor, Weapon, Projectile/Explosion valid before call this func
*/
static __forceinline InternalArgs CreateInternalArgsWrap(const ActorHitData* _hData, bool isDebugMode = false, bool isMelee = false, bool isExplosion = false) {
	InternalArgs _args{};

	Actor* _tar = _hData->target;
	Actor* _src = _hData->source;

	SInt32 _hitloc = _hData->hitLocation;
	bool isHeadShot = (_hitloc == 1 || _hitloc == 2 || _hitloc == 13);

	Wrapper_EffectiveDTDR _wrapper{ (isHeadShot ? GetEqHelmetDT(_tar) : GetEqArmorDT(_tar)),
										(isHeadShot ? GetEqHelmetDR(_tar) : GetEqArmorDR(_tar)) };

	__WriteLogCond(isDebugMode, ">DT provided by armor: %.2f", _wrapper.DT);
	__WriteLogCond(isDebugMode, ">DR provided by armor: %.2f", _wrapper.DR);

	if (!isMelee)
	{
		if (TESAmmo* _currentAmmo = GetCurEqAmmo(_hData->weapon, _src))
		{
			if (isDebugMode) {
				PrintAmmoEffects(_currentAmmo);
			}
			ApplyAmmoEffects(_currentAmmo, OUT _wrapper);
			__WriteLogCond(isDebugMode, ">Effictive DT(After Apply Ammo Effect): %.2f", _wrapper.DT);
			__WriteLogCond(isDebugMode, ">Effictive DR(After Apply Ammo Effect): %.2f", _wrapper.DR);
		}
	}
	

	InternalArgs _internalArgs{};
	_internalArgs.setInternalArg(InternalArgs::Index::EffectiveDT, _wrapper.DT);
	_internalArgs.setInternalArg(InternalArgs::Index::EffectiveDR, _wrapper.DR);
	_internalArgs.setInternalArg(InternalArgs::Index::HealthDamage, _hData->healthDmg);
	if (isMelee)
	{
		_internalArgs.setInternalArg(InternalArgs::Index::RawDamage, _hData->wpnBaseDmg);
	}
	else {
		float newVal = 0.0f;
		if (isExplosion){
			newVal = static_cast<BGSExplosion*>(_hData->explosion->baseForm)->damage;
		}
		else {
			newVal = _hData->projectile->hitDamage;
		}
		_internalArgs.setInternalArg(InternalArgs::Index::RawDamage, newVal);
	}

	_internalArgs.setInternalArg(InternalArgs::Index::WeaponDamage, _hData->weapon->attackDmg.damage);
	_internalArgs.setInternalArg(InternalArgs::Index::WeaponWeight, _hData->weapon->weight.weight);

	return _internalArgs;
}


