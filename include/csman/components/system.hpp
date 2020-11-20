#pragma once
/*
* Covariant Script Manager System APIs Header
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
#include <exception>
#include <string>
#include <vector>

namespace csman {
	namespace sys {
		std::string get_env(const std::string&);

		std::vector<char> http_get(const std::string &url);

		std::string http_get_string(const std::string &url);

		void http_get_file(const std::string &url, const std::string &path);
	}
}