//
// Created by Rend on 2020/10/16.
//
#include "Command.hpp"

CsmCmd::OptClass CsmCmd::GetOptType(const std::string &str) {
    if (str == "-a" || str == "--all")
        return CsmCmd::OptClass::all;
    else if (str == "-r" || str == "--redo" || str == "--reinstall")
        return CsmCmd::OptClass::redo;
    else if (str == "-h" || str == "--help")
        return CsmCmd::OptClass::help;
    else if (str == "-" || str == "--repair")
        return CsmCmd::OptClass::repair;
    else if (str == "-v" || str == "--version")
        return CsmCmd::OptClass::version;
    return CsmCmd::OptClass::err;
};

CsmCmd::ResultCode CsmCmd::parser(const std::string &cmd, const std::vector<std::string> &args) {
    try {
        if (cmd == "install")
            return install(args[1], args[2]);
        else if (cmd == "uninstall")
            return uninstall(args[1], args[2]);
        else if (cmd == "check")
            return check(args[1], args[2]);
        else if (cmd == "list")
            return list(args[1], args[2]);
        else if (cmd == "run")
            return run(args[1], args[2]);
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
            case CsmBase::CsmErr::ErrCodeClass::OK:
                return ResultCode::OK;
        }
    }
};