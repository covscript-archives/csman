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
#include <csman/common.hpp>

namespace csman {
	class pacman {
		using package_index = map_t<std::string, package>;
		global_config* config;
		package_index local;
		package_index remote;
		bool source_outdated(const std::string&);
		void fetch_source(const std::string& url, const std::string& path);
		void read_from_file(package_index&, const std::string&);
		void write_to_file(const package_index&, const std::string&);
	public:
		pacman() = delete;
		explicit pacman(global_config* cfg) : config(cfg)
		{
		    cfg->log.set_level("csman.packman.install", 128);
			read_from_file(local, config->packages_cache + path::delimiter + "packages.json");
		}
		/**
		 * Install a package
		 * @param name: ^[A-Za-z_](.[A-Za-z0-9_]){2}$
		 * @param version: stable | unstable | specific version: ^([0-9]\.){1,3}[0-9]$
		 * @param reinstall: uninstall package and reinstall
		 * @param fix_broken: install missing components
		 */
		void install(const std::string& name, const std::string& version, bool reinstall = false, bool fix_broken = false)
		{
			std::string sources_path = config->packages_cache + path::delimiter + "sources.json";
			if (source_outdated(sources_path)) {
				for (auto &url : config->sources_list) {
					try {
						fetch_source(url, sources_path);
					}
					catch (const http_host_unreachable&) {
					    config->log.touch("csman.packman.install", "HTTP host unreachable! Install terminated. Source Url: " + url);
						continue;
					}
					catch (const http_404_not_found&) {
                        config->log.touch("csman.packman.install", "HTTP 404 Not Found! Install terminated. Source Url: " + url);
						continue;
					}
					catch (...) {
                        config->log.touch("csman.packman.install", "Unknown Exception when calling packman::fetch_source! Install terminated. Source Url: " + url);
						throw;
					}
				}
                read_from_file(remote, sources_path);
			}
            if (remote.count(name) > 0)
            {

            } else {
                config->log.touch("csman.global", "Unknown package \"" + name + "\"");
                config->log.touch("csman.packman.install", "Unknown package \"" + name + "\"");
            }
		}
	};
}