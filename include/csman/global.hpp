#pragma once
/*
 * CovScript Manager: Global Functions
 * Licensed under Apache 2.0
 * Copyright (C) 2020-2021 Chengdu Covariant Technologies Co., LTD.
 * Website: https://covariant.cn/
 * Github:  https://github.com/chengdu-zhirui/
 */
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <memory>
#include <vector>
#include <string>
#include <regex>
#include <set>

#include <csman/exception.hpp>
#include <mozart++/core>

namespace csman {
	template<typename K, typename V> using map_t = std::unordered_map<K, V>;
	template<typename T> using set_t = std::set<T>;

	namespace directory {
		bool path_exist(const std::string &);

		bool create_dir(const std::string &);

		bool remove_dir(const std::string &);
	}

	namespace network {
		bool http_get(const std::string &, const std::string &, int);

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

		void initialize_val();

		void get_covscript_env();

		void read_config();

		void write_config();

		set_t<std::string> only_read_vars;
		struct Config_Data {
			struct line_data {
				std::string text;
				bool is_notes;

				line_data(const std::string &text, bool is_notes) : text(text),
                                                                      is_notes(is_notes) {}// 该行不是注释，则记录key；反之记录该行注释内容
			};

			std::vector<line_data> lines;
		} config_data;
	public:
		std::string ABI, STD, runtime_ver;
		map_t<std::string, std::string> vars;

		/*
		 * all variables are in "configure vars" :
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
				
                get_covscript_env();
				
                read_config();
			}
			catch (const std::exception &e) {
				std::cerr << e.what() << std::endl;
				throw e;
			}
		}

		~context(){
            if(vars.count("config_path") != 0) {
                std::ofstream ofs(vars["config_path"]);
                write_config();
                ofs.close();
            }
            else
                std::cout<<"Warning: your file for recording .csman_config updating failed, it may cause extremely problems while next last_update_time. Please try to repair it by using \"csman repair\""<<std::endl;
		}
	};

	using context_t = std::shared_ptr<context>;

	static context_t make_context()
	{
		return std::make_shared<context>();
	}
}