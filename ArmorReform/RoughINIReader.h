#pragma once
#include <string>
#include <filesystem>
#include <string_view>
#include <map>
#include <set>
#include <fstream>
#include <expected>
#include <algorithm>


namespace roughinireader {
	namespace ns_reader_error {	// error code

		enum class read_errc {
			success = 0,
			filename_empty,
			path_is_empty,
			path_not_exist,
			not_ini_file,
			path_without_filename,
			open_file_failed,
		};

		// ReaderErrorCategory instance
		inline const auto& reader_error_category() {
			static const struct : std::error_category {
				virtual std::string message(int val) const override {
					switch ((read_errc)val) {
					case read_errc::success:return "success";
					case read_errc::filename_empty:return "given filename is empty";
					case read_errc::path_is_empty:return "given path is empty";
					case read_errc::path_not_exist:return "given path is not exist";
					case read_errc::not_ini_file:return "file exist but no the ini file";
					case read_errc::path_without_filename:return "path without filename given";
					case read_errc::open_file_failed:return "failed to open file";
					}
					return "unkown error";
				}

				virtual const char* name() const noexcept override {
					return "INI_Reader_Error";
				}
			} instance;
			return instance;
		}

		inline std::error_code make_error_code(read_errc r_ec) {
			return std::error_code((int)r_ec, reader_error_category());
		}

		// content below needed C++ 23
		// std::expected<int,std::error_code>
		// return error code ==> return std::unexpected{ make_error_code( error_code ) };
	};

	namespace utility {

		// find format [ * ] 
		static inline bool isSection(std::string_view _section) {
			return _section.find_first_of('[') != std::string_view::npos &&
				_section.find_last_of(']') != std::string_view::npos;
		}

		// trim front space and back space
		static inline void trimFrontBackSpace(std::string& _raw_str) {
			_raw_str.erase(0, _raw_str.find_first_not_of(" \t\n\r"));			// trim front space
			_raw_str.erase(_raw_str.find_last_not_of(" \t\n\r") + 1);			// trim back space
		}

		// Trim comment
		static inline void trimComment(std::string& _raw_str) {
			if (auto comment_pos = _raw_str.find(';'); comment_pos != std::string::npos) {
				_raw_str = _raw_str.substr(0, comment_pos);
			}
		}

		// Get section name 
		static inline bool trimToSectionName(std::string& _raw_str) {
			auto fst_brk = _raw_str.find_first_of('[');
			auto lst_brk = _raw_str.find_last_of(']');
			if (fst_brk == std::string::npos || lst_brk == std::string::npos) return false;

			_raw_str = _raw_str.substr(fst_brk + 1, (lst_brk - fst_brk - 1));	// get string which inside the [ ]
		
			trimFrontBackSpace(_raw_str);												// trim string front space and back space
			return true;
		}

		static inline bool matchSection(std::string_view _cur_line, std::string_view _section_name) {
			if (_cur_line.empty() || _section_name.empty()) return false;
			if (auto fst_brk = _cur_line.find_first_of('['); fst_brk != std::string_view::npos) {
				if (auto lst_brk = _cur_line.find_last_of(']'); lst_brk != std::string_view::npos) {

					if (_cur_line.substr(fst_brk + 1, (lst_brk - fst_brk - 1)) == _section_name) return true;
				}
			}

			return false;
		}

		static inline auto toKeyValuePair(const std::string& _cur_line) {
			struct kv_ret {
				bool success;
				std::string key, value;
			};
#define EMPTY_PAIR kv_ret{false,"",""}
			if (_cur_line.empty()) return EMPTY_PAIR;

			if (auto eq_pos = _cur_line.find_first_of('='); eq_pos != std::string::npos)
			{
				std::string Lstr = _cur_line.substr(0, eq_pos);
				std::string Rstr = _cur_line.substr(eq_pos + 1);
				if (Lstr.empty() || Rstr.empty()) return EMPTY_PAIR;
				trimFrontBackSpace(Lstr);
				trimFrontBackSpace(Rstr);

				return kv_ret{true,Lstr,Rstr};
			}

			return EMPTY_PAIR;
		}

	};



namespace fs = std::filesystem;
namespace ini_error = roughinireader::ns_reader_error;

//struct SectionInfo;
using key = std::string;
using raw_value = std::string;
using key_val_map = std::map<std::string, std::string>;

using section_name = std::string;

using section_map = std::map< section_name, std::unique_ptr<key_val_map> >;

struct SectionMap
{
	// section_map = std::map< section_name, std::unique_ptr<key_val_map> >;
	std::unique_ptr<section_map> m_sec_map;

	SectionMap(){
		m_sec_map = std::make_unique<section_map>();
	}
	SectionMap(const std::string& s_section_name) {
		m_sec_map = std::make_unique<section_map>();
		m_sec_map.get()->try_emplace(s_section_name , std::make_unique<key_val_map>());
	}
	SectionMap(std::string_view sv_section_name) {
		m_sec_map = std::make_unique<section_map>();
		m_sec_map.get()->try_emplace( std::string{ sv_section_name }, std::make_unique<key_val_map>() );
	}
	SectionMap(SectionMap&& _rhs) noexcept {
		m_sec_map = std::exchange(_rhs.m_sec_map,nullptr);
	}
	SectionMap(const SectionMap& that) = delete;
	SectionMap& operator=(const SectionMap& that) = delete;

	~SectionMap() {}
	//bool InsertPairToMap(const std::string& key,const std::string& val) const;

	section_map* GetSectionMap() const ;
	const section_map* GetConstSectionMap() const;
	const key_val_map* GetSectionKeyValMapCst(const std::string&) const;
};

// ================== INIReader ==================
constexpr int success = 0;
class INIReader
{
private:
	fs::path m_ini_path{ std::string_view{""} };
	std::unique_ptr<SectionMap> m_sections;

public:
	INIReader(std::string_view _ini_root_folder);
	INIReader(const fs::path& _ini_root_folder);
	INIReader(const INIReader& that) = delete;
	INIReader& operator=(const INIReader& that) = delete;
	~INIReader();

	

	std::expected<int, std::error_code> SetCurrentINIFileName(const fs::path&);
	std::expected<int, std::error_code> SetCurrentPath(const fs::path&);
	std::expected<int, std::error_code> SetCurrentPath(const fs::path&, const fs::path& );

	std::expected<int, std::error_code> ConstructSectionMap();

	const fs::path& GetCurrentINIPath();
	// do nothing when m_ini_path has .ini extension
	const fs::path& AppendPath(const fs::path&,bool spilt_before_append = false,bool spilt_after_append = false);
	// const fs::path& AppendPath(std::string_view);
	const section_map* GetSectionMapCst() const;
	section_map* GetSectionMap() const;
	// get key-val map of a specific section.Will return nullptr if no such section
	const key_val_map* GetSectionKeyValMapCst(const std::string& sec_name);
	// get rawtype val by given section and key
	std::string GetRawTypeVal(const std::string& sec_name,const std::string& key_name);

	int GetInt(const std::string& s_raw_val, int def_val);
	UINT32 GetUInt(const std::string& s_raw_val, UINT32 def_val);
	float GetFloat(const std::string& s_raw_val, float def_val);
};




}
