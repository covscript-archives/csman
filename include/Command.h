//
// Created by Rend on 2020/8/13.
//
#pragma once

#include <CsmBase.h>

namespace CsmCmd {
	enum class ResultCode {
		CsmanErr,ArgErr,Warning,OK
	};
	enum class OptClass {
		err,
		all,
		redo,
		help,
		version,
	};
	OptClass GetOptType(const std::string &str);
	ResultCode Parse(const std::string &cmd,const std::vector<std::string> &args);
	ResultCode install(const std::vector<std::string> &args);
	ResultCode uninstall(const std::vector<std::string> &args);
	ResultCode check(const std::vector<std::string> &args);
	ResultCode list(const std::vector<std::string> &args);
	ResultCode run(const std::vector<std::string> &args);
}