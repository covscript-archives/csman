#pragma once
#include <unordered_set>
#include <mozart++/core>
#include <vector>
#include <string>
#include <regex>

namespace csman {
	template<typename T>using set_t = std::unordered_set<T>;

	namespace directory {
		bool path_exist(const std::string &);

		bool create_dir(const std::string &);

		bool remove_dir(const std::string &);
	}

	namespace network {
		bool http_get(const std::string &,const std::string &, int);

		std::vector<char> http_get(const std::string &, int);
	}

	void str_split(std::vector<std::string> &, const std::string &);

	bool readline(std::ifstream &, std::vector<std::string> &);

	static bool is_abi(const std::string &str)
	{
		static const std::regex reg("^ABI[0-9]{4}[0-9A-F]{2}$");
		return std::regex_match(str, reg);
	}

	static bool is_std(const std::string &str)
	{
		static const std::regex reg("^STD[0-9]{4}[0-9A-F]{2}$");
		return std::regex_match(str, reg);
	}

	static bool is_ver(const std::string &str)
	{
		static const std::regex reg("^([0-9]+\\.){0,3}[0-9]+\\w*$");
		return std::regex_match(str, reg);
	}

	static bool is_pac(const std::string &str)
	{
		static const std::regex reg("^(\\w+\\.)+\\w+$");
		return std::regex_match(str, reg);
	}

	class context {
	private:
		void set_testing_var();
		void initialize_val();
		void get_covscript_env();
		void read_config();
		void write_config();
		set_t<std::string> only_read_vars;
		struct Config_Data {
			struct line_data {
				std::string text;
				bool is_comment;

				line_data(const std::string &text, bool is_comment) : text(text),
					is_comment(is_comment) {}// 该行不是注释，则记录key；反之记录该行注释内容
			};
			std::vector<line_data> lines;
		} config_data;
	public:
		std::string ABI, STD, runtime_ver;
		std::unordered_map<std::string, std::string> vars;
		/*
		 * all variables in "configure vars":
		 * home_path
		 * COVSCRIPT_HOME, CS_IMPORT_PATH, CS_DEV_PATH
		 * config_path, csman_path, pac_repo_path, sources_idx_path, max_reconnect_time
		 */

		void show(const std::string &key);

		void show_all();

		void set(const std::string &key, const std::string &val);

		context()
		{
			try {
				initialize_val();
//			get_covscript_env();
//			read_config();
				set_testing_var();
			}
			catch (std::exception e) {
				throw e;
			}
		}
	};
}