#pragma once
/*
* Covariant Script Manager: Exception
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

namespace csman {
	class exception : public std::exception {
		std::string mWhat;
	public:
		exception() = delete;

		exception(const std::string &name, const std::string &what) noexcept: mWhat(name + ": " + what) {}

		exception(const exception &) = default;

		exception(exception &&) noexcept = default;

		~exception() override = default;

		exception &operator=(const exception &) = default;

		exception &operator=(exception &&) = default;

		const char *what() const noexcept override
		{
			return this->mWhat.c_str();
		}
	};

	class http_host_unreachable final : public exception {
	public:
		http_host_unreachable(const std::string& url) : exception("HTTP Host Unreachable", url) {}
	};

	class http_404_not_found final : public exception {
	public:
		http_404_not_found(const std::string& url) : exception("HTTP Page Not Found", url) {}
	};
}