//
// Created by Rend on 2020/10/16.
//
#include "Command.h"

CsmCmd::OptClass CsmCmd::GetOptType(const std::string &str) {
    if (str == "-a" || str == "--all")
        return CsmCmd::OptClass::all;
    else if (str == "-r" || str == "--redo" || str == "--reinstall")
        return CsmCmd::OptClass::redo;
    else if (str == "-h" || str == "--help")
        return CsmCmd::OptClass::help;
    else if (str == "-v" || str == "--version")
        return CsmCmd::OptClass::version;
    return CsmCmd::OptClass::err;
};

CsmCmd::ResultCode CsmCmd::Parse(const std::string &cmd, const std::vector<std::string> &args) {
    try {
        if (cmd == "install")
            return install(args);
        else if (cmd == "uninstall")
            return uninstall(args);
        else if (cmd == "check")
            return check(args);
        else if (cmd == "list")
            return list(args);
        else if (cmd == "run")
            return run(args);
    }
    catch (CsmBase::CsmErr e) {
        CsmBase::WriteErrLog(e);
        switch (e.errCode) {
            case CsmBase::CsmErr::ErrCodeClass::CsmanErr:
                return ResultCode::CsmanErr;
            case CsmBase::CsmErr::ErrCodeClass::ArgErr:
                return ResultCode::ArgErr;
            case CsmBase::CsmErr::ErrCodeClass::NetWorkErr: {
                std::cout << "csman:\twe suggest you using [csman] online, do you wanna interrupt now?[y/n]"
                          << std::endl;
                char c;
                std::cin >> c;
                if (c == 'y' || c == 'Y')
                    exit(0);
                return ResultCode::OK;
            }
        }
    }
    return ResultCode::OK;
};

CsmCmd::ResultCode CsmCmd::install(const std::vector<std::string> &args) {

}
CsmCmd::ResultCode CsmCmd::uninstall(const std::vector<std::string> &args){

}
CsmCmd::ResultCode CsmCmd::check(const std::vector<std::string> &args);
CsmCmd::ResultCode CsmCmd::list(const std::vector<std::string> &args);
CsmCmd::ResultCode CsmCmd::run(const std::vector<std::string> &args);