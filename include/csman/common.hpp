#pragma once
/*
* Covariant Script Manager Common Header
*
* Licensed under the Covariant General Public License,
* Version 1.1 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* https://covariant.cn/licenses/LICENSE-1.1
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* Copyright (C) 2020 Chengdu Covariant Technologies Co., LTD.
* Email:   dev@covariant.cn
* GitHub:  https://github.com/chengdu-zhirui
* Website: http://csman.info/
*/
#include "components/exception.hpp"
#include "components/version.hpp"
#include "components/system.hpp"
#include "components/event.hpp"
#include <mozart++/optional>
#include <string>
#include <vector>

namespace csman {
// Type Definitions
	struct package_info;

// Environment Constants
	namespace env {
		constexpr char covscript_home[] = "COVSCRIPT_HOME";
		constexpr char cs_import_path[] = "CS_IMPORT_PATH";
		constexpr char cs_dev_path[] = "CS_DEV_PATH";
	}

// Path Constants
	namespace path {
#ifdef MOZART_PLATFORM_WIN32
		constexpr char separator = '\\';
		constexpr char delimiter = ';';
#else
		constexpr char separator = '/';
		constexpr char delimiter = ':';
#endif
		constexpr char bin_suffix[] = "bin";
		constexpr char lib_suffix[] = "lib";
		constexpr char import_suffix[] = "imports";
		constexpr char include_suffix[] = "include";
		constexpr char packages_suffix[] = "packages";
		constexpr char generic_source[] = "/sources/Generic.json";
#if defined(MOZART_PLATFORM_WIN32)
		constexpr char platform_source[] = "/sources/Win32_MinGW-w64_AMD64.json";
#elif defined(MOZART_PLATFORM_LINUX)
		constexpr char platform_source[] = "/sources/Linux_GCC_AMD64.json";
#elif defined(MOZART_PLATFORM_DARWIN)
		constexpr char platform_source[] = "/sources/macOS_clang_AMD64.json";
#else
#error CSMAN: Platform Not Supported!
#endif
	}

	struct global_config {
		std::string covscript_home;
		std::string packages_cache;
		std::vector<std::string> sources_list = {"http://csman.info"};
		event_log log;
		global_config()
		{
		    log.set_level("csman.global", 255);
		    log.log_to_stdout("csman.global");
			covscript_home = sys::get_env(env::covscript_home);
			packages_cache = covscript_home + path::separator + path::packages_suffix;
			// TODO: read config file
		}
	};

	/**
	 * State: Stable or Unstable
	 * STD: Latest stable version following CovScript Standard
	 * ABI: Latest stable version following CovScript Interpreter ABI
	 * Ver: Specific package version
	 */
	enum class dep_type {
		state, std, abi, ver
	};

	struct package_dependency {
		dep_type type = dep_type::ver;
		std::string name;
		std::string deps;
	};

	struct package_content {
		package_info* parent = nullptr;
		std::vector<std::string> deps;
		std::string remote_url;
		std::string local_path;
		cov::optional<std::string> std_opt;
		cov::optional<std::string> abi_opt;
	};

	struct package {
		std::string name;
		std::string desc;
		std::string author;
		cov::optional<std::string> stable_ver;
		cov::optional<std::string> unstable_ver;
		map_t<std::string, package_content> contents;
	};
}