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
#include <forward_list>
#include <string>
#include <cstdio>

namespace csman {
	class event_log final {
		class output_target {
			cov::optional<mpp::fdostream> os;
			FILE* redirected = nullptr;
			mpp::fd_type fd;
		public:
			output_target() : fd(mpp::get_fd(stderr)) {
                os.emplace(fd);
			}
			output_target(const std::string& path)
			{
				redirected = fopen(path.c_str(), "a");
				if (redirected != nullptr) {
                    fd = mpp::get_fd(redirected);
                    os.emplace(fd);
                }
			}
			~output_target()
			{
				os.reset();
				if (redirected)
					fclose(redirected);
			}
			bool available() const
            {
			    return os.has_value();
            }
			std::ostream& get()
            {
			    return os.value();
            }
		};
		struct event_content {
		    bool has_stdout = false;
            std::uint8_t level = 255;
		    std::forward_list<output_target> output_list;
		};
		map_t<std::string, event_content> contents;
        mpp::event_emitter emitter;
	public:
        std::uint8_t log_level = 0;
	    template<typename listener_t>
	    void add_listener(const std::string& name, listener_t&& func)
        {
	        emitter.on(name, std::forward<listener_t>(func));
        }
        void log_to_stdout(const std::string& name)
        {
            auto &c = contents[name];
            if (!c.has_stdout) {
                c.output_list.emplace_front();
                c.has_stdout = true;
            }
        }
        bool log_to_file(const std::string& name, const std::string& path)
        {
            auto &c = contents[name];
            c.output_list.emplace_front(path);
            if (!c.output_list.front().available())
            {
                c.output_list.pop_front();
                return false;
            } else
                return true;
        }
        template<typename...ArgsT>
        void touch(const std::string& name, const std::string& msg, ArgsT&&...args)
        {
            if (contents.count(name) > 0)
            {
                auto &c = contents[name];
                if (c.level >= log_level)
                {
                    for (auto& out:c.output_list) {
                        out.get() << "Log Name: " << name << ", Log Level = " << c.level << ", Message: " << msg
                                  << std::endl;
                    }
                }
            }
            emitter.emit(name, msg, std::forward<ArgsT>(args)...);
        }
        void set_level(const std::string& name, std::uint8_t level)
        {
            contents[name].level = level;
        }
    };
}