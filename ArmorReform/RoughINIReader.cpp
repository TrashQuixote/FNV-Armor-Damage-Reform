#include "RoughINIReader.h"


namespace roughinireader {
using namespace roughinireader::utility;

INIReader::INIReader(std::string_view _ini_path_sv)
{
	m_sections = std::make_unique<SectionMap>();

	if (_ini_path_sv.empty()) {
		m_ini_path = fs::path{ "" };
		return;
	}

	auto l_ini_path = fs::path{ _ini_path_sv };
	if (l_ini_path.extension() == ".ini") {
		m_ini_path = l_ini_path;
		return;
	}

	if (l_ini_path.has_parent_path()) m_ini_path = l_ini_path.parent_path();
	else m_ini_path = fs::path{ "" };
}

INIReader::INIReader(const fs::path& _ini_root_folder)
{
	m_sections = std::make_unique<SectionMap>();

	if (_ini_root_folder.empty()) {
		m_ini_path = fs::path{ "" };
		return;
	}

	if (_ini_root_folder.extension() == ".ini") {
		m_ini_path = _ini_root_folder;
		return;
	}

	if (_ini_root_folder.has_parent_path()) m_ini_path = _ini_root_folder.parent_path();
	else m_ini_path = fs::path{ "" };
}

INIReader::~INIReader() {}


#define EMPTY_FILENAME_ERROR std::unexpected{ ini_error::make_error_code(ini_error::read_errc::filename_empty) }
#define EMPTY_PATH_ERROR std::unexpected{ ini_error::make_error_code(ini_error::read_errc::path_is_empty) }
#define NO_FILENAME_ERROR std::unexpected{ ini_error::make_error_code(ini_error::read_errc::path_without_filename) }
#define NOT_INI_FILE std::unexpected{ ini_error::make_error_code(ini_error::read_errc::not_ini_file) }
#define OPEN_FILE_FAILED std::unexpected{ ini_error::make_error_code(ini_error::read_errc::open_file_failed) }
#define PATH_NOT_EXIST std::unexpected{ ini_error::make_error_code(ini_error::read_errc::path_not_exist) }


	std::expected<int, std::error_code> INIReader::SetCurrentINIFileName(const fs::path& _new_filename)
	{
		if (_new_filename.empty()) return EMPTY_FILENAME_ERROR;
		if (_new_filename.extension() != ".ini") return NOT_INI_FILE;

		if (m_ini_path.extension() == ".ini") {
			m_ini_path.replace_filename(_new_filename);
			return success;
		}
		if (*m_ini_path.string().rbegin() != '\\') m_ini_path += '\\';
		m_ini_path.replace_filename(_new_filename);

		return success;
	}


	std::expected<int, std::error_code> INIReader::SetCurrentPath(const fs::path& _new_filefolder)
	{
		if (_new_filefolder.empty()) return EMPTY_PATH_ERROR;
		m_ini_path.clear();
		m_ini_path = _new_filefolder;

		return success;
	}


	std::expected<int, std::error_code> INIReader::SetCurrentPath(const fs::path& _new_filefolder, const fs::path& _new_filename)
	{
		auto p_ec = SetCurrentPath(_new_filefolder);
		if (!p_ec.has_value()) return p_ec;

		return SetCurrentINIFileName(_new_filename);
	}

	std::expected<int, std::error_code> INIReader::ConstructSectionMap()
	{
		if (m_ini_path.empty()) return EMPTY_PATH_ERROR;
		if (!m_ini_path.has_filename()) return NO_FILENAME_ERROR;
		if (m_ini_path.extension() != ".ini") return NOT_INI_FILE;
		if (!fs::exists(m_ini_path)) return PATH_NOT_EXIST;

		m_sections->GetSectionMap()->clear();

		std::ifstream fin;
		fin.open(m_ini_path, std::ios::in);
		if (fin.is_open() == false) return OPEN_FILE_FAILED;

		std::string buffer;

		auto sec_iter = m_sections->GetSectionMap()->end();
		while (std::getline(fin, buffer))
		{
			trimComment(buffer);
			if (buffer.empty()) continue;

			if (isSection(buffer)) { // match a section
				trimFrontBackSpace(buffer);
				trimToSectionName(buffer);
				auto pair = m_sections->GetSectionMap()->try_emplace(buffer, std::make_unique<key_val_map>());
				if (!pair.second || pair.first == m_sections->GetSectionMap()->end()) continue;
				sec_iter = pair.first;				// use in next loop to insert key value

				continue;
			}
			else if (sec_iter != m_sections->GetSectionMap()->end()) { // failed to match a section
				auto ret = toKeyValuePair(buffer);
				if (!ret.success) continue;
				sec_iter->second->try_emplace(ret.key, ret.value);
			}

		}
		return success;
	}

	const fs::path& INIReader::GetCurrentINIPath()
	{
		return m_ini_path;
	}

	const fs::path& INIReader::AppendPath(const fs::path& _append, 
										  bool spilt_before_append, 
										  bool spilt_after_append)
	{
		if ( m_ini_path.extension() == ".ini" ) return m_ini_path;

		if (spilt_before_append && !m_ini_path.empty()  && *m_ini_path.string().rbegin() != '\\') m_ini_path += '\\';
		m_ini_path += _append;
		if (spilt_after_append && !m_ini_path.empty() && *m_ini_path.string().rbegin() != '\\') m_ini_path += '\\';
		return m_ini_path;
	}

	const section_map* INIReader::GetSectionMapCst() const
	{
		return m_sections.get()->m_sec_map.get();
	}

	section_map* INIReader::GetSectionMap() const
	{
		return m_sections.get()->m_sec_map.get();
	}

	const key_val_map* INIReader::GetSectionKeyValMapCst(const std::string& sec_name)
	{
		const auto* sec_map = m_sections.get()->GetConstSectionMap();
		if (const auto& iter = sec_map->find(sec_name);iter != sec_map->end())
		{
			return iter->second.get();
		}
		return nullptr;
	}


	std::string INIReader::GetRawTypeVal(const std::string& sec_name, const std::string& key_name)
	{
		if (const auto* kv_map = GetSectionKeyValMapCst(sec_name)){
			if (const auto& iter = kv_map->find(key_name);iter != kv_map->end()){
				return iter->second;
			}
		}
		return "";
	}

	int INIReader::GetInt(const std::string& s_raw_val, int def_val){
		int ret = def_val;
		try {
			ret = std::stoi(s_raw_val);
		}
		catch (...) {
			ret = def_val;
		}
		return ret;
	}

	UINT32 INIReader::GetUInt(const std::string& s_raw_val, UINT32 def_val){
		UINT32 ret = def_val;
		try {
			ret = static_cast<UINT32>(std::stol(s_raw_val));
		}
		catch (...) {
			ret = def_val;
		}
		return ret;
	}

	float INIReader::GetFloat(const std::string& s_raw_val, float def_val){
		float ret = def_val;
		try {
			ret = std::stof(s_raw_val);
		}
		catch (...) {
			ret = def_val;
		}
		return ret;
	}

};

namespace roughinireader {
	section_map* SectionMap::GetSectionMap() const { return m_sec_map.get(); }
	const section_map* SectionMap::GetConstSectionMap() const { return m_sec_map.get(); }

	const key_val_map* SectionMap::GetSectionKeyValMapCst(const std::string& sec_name) const{
		if (const auto& iter = m_sec_map->find(sec_name); iter != m_sec_map->end()){
			return iter->second.get();
		}
		return nullptr;
	}
}
