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

#include <covscript_impl/system.hpp>
#include <csman/exception.hpp>
#include <mozart++/core>

#ifdef MOZART_PLATFORM_LINUX
#define delimiter '/'
#elif defined(MOZART_PLATFORM_WIN32)
#define delimiter '\\'
#elif defined(MOZART_PLATFORM_DARWIN)
#define delimiter '/'
#endif

namespace csman {
	template<typename K, typename V> using map_t = std::unordered_map<K, V>;
	template<typename T> using set_t = std::set<T>;

	namespace sys {
		inline bool exist(const std::string &path)
		{
			return cs_impl::file_system::exists(path);
		}
		namespace dir {
			inline bool create(const std::string& path)
			{
				return cs_impl::file_system::mkdir_p(path);
			}
			using cs_impl::file_system::remove;
			std::vector<std::string> scan(std::string);
		}
		namespace file {
			bool create(std::string);
			using cs_impl::file_system::remove;
			std::vector<std::string> scan(std::string);
		}
	}

	namespace network {
		bool http_get(const std::string &, std::string, int);
		std::vector<char> http_get(const std::string &, int);
	}

	namespace str {
		bool is_abi(const std::string &);
		bool is_std(const std::string &);
		bool is_ver(const std::string &);
		bool is_pac(const std::string &);
		bool weak_equal(const std::string &, const std::string &);
		std::vector<std::string> split(const std::string &);
		std::vector<std::string> split(const std::string &, const std::string &);
		bool readline(std::ifstream &, std::vector<std::string> &);
		bool path_char(const char &);
		bool is_path(const std::string &);
		bool path_compare(std::string, std::string );
		std::string to_dir_path(const std::string &);
		std::string to_file_path(const std::string &);
	}
}
namespace csman {
	template<typename K, typename V> using map_t = std::unordered_map<K, V>;
	template<typename T> using set_t = std::set<T>;
	class context {
	private:

		void initialize_val();

		void get_covscript_env();

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
		std::string home_path;
	public:
		std::string ABI, STD, runtime_ver;
		map_t<std::string, std::string> vars;

		/*
		 * all variables are in "configure vars" :
		 * COVSCRIPT_HOME, CS_IMPORT_PATH, CS_DEV_PATH
		 * config_path, csman_path, pac_repo_path, sources_idx_path, max_reconnect_time
		 */

		void show(const std::string &key);
		void show_all();
		void set(const std::string &key, const std::string &val);
		void read_config(); // 需要手动调用read
		void write_config(); // 析构会自动调用write，可提前手动调用

		context()
		{
			try {
				initialize_val();

				get_covscript_env();
			}
			catch (const std::exception &e) {
				std::cerr << e.what() << std::endl;
				throw e;
			}
		}

		~context()
		{
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