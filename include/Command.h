//
// Created by Rend on 2020/8/13.
//
// Command.h 只需要将用户真实参数递交到CsmConfig，其余的由CsmConfig自行实现
// （比方版本号为空，那么就是“”，不需要自行翻译成“Stable”）
#pragma once

#include <set>
#include <cstdio>
#include <CsmBase.h>
#include <CsmConfig.h>

class CsmCommand {
public:
    CsmCommand(Context *&_cxt, CsmConfig::Config *_c, CsmConfig::Sources *_s, CsmConfig::Package *_p);

    CsmBase::CsmErr::ErrCodeClass Parse(const std::string &cmd, std::vector<std::string> &args); /*done*/

private:
    Context *cxt;
//    CsmJsonFile *config,*sources,*package;
    CsmConfig::Config *config;
    CsmConfig::Sources *sources;
    CsmConfig::Package *package;

    enum class OptArg {
        err, clean, redo, fix, help, version,
    };
    std::set<OptArg> opts;
    OptArg GetOptType(const std::string &str); /*done*/
    void install(std::vector<std::string> &args);

    void uninstall(std::vector<std::string> &args);

    void check(std::vector<std::string> &args);

    void list(std::vector<std::string> &args);

    void run(std::vector<std::string> &args);
};