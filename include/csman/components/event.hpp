#pragma once
/*
* Covariant Script Manager: Event Log
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
#include "common.hpp"
#include "optional.hpp"
#include <mozart++/core>
#include <mozart++/fdstream>
#include <string>
#include <cstdio>

namespace csman {
	class event_log final {
		struct output_target {
			cov::optional<mpp::fdostream> os;
			FILE* redirected = nullptr;
			mpp::fd_type fd;
			output_target() : fd(fileno(stdout)) {}
			output_target(const std::string& path)
			{
				redirected = fopen(path.c_str(), "a");
				if (redirected != nullptr)
					fd = fileno(redirected);
				else
					fd = fileno(stdout);
			}
			~output_target()
			{
				os.reset();
				if (redirected)
					fclose(redirected);
			}
			void start()
			{
				os.emplace(fd);
			}
		};
		struct event_content {
		};
		map_t<std::string, event_content> event_idx;
	};
}