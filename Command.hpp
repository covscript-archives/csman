//
// Created by Rend on 2020/8/13.
//
#pragma once

#include "CsmBase.hpp"

namespace CsmCmd{
    enum class ResultCode{
        CsmanErr,ArgErr,Warning,OK
    };
    enum class OptClass{
        err,
        all,
        redo,
        help,
        repair,
        version,
    };
    OptClass GetOptType(const std::string &str);
    ResultCode parser(const std::string &cmd,const std::vector<std::string> &args);
    ResultCode install(const std::string &name,const std::string ver);
    ResultCode uninstall(const std::string &name,const std::string ver);
    ResultCode check(const std::string &name,const std::string ver);
    ResultCode list(const std::string &name,const std::string ver);
    ResultCode run(const std::string &name,const std::string ver);
}