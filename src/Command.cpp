//
// Created by Rend on 2020/10/16.
//
#include "Command.h"

CsmCommand::CsmCommand(Context *&_cxt, CsmConfig::Config *_c, CsmConfig::Sources *_s,
                       CsmConfig::Package *_p) : cxt(_cxt), config(_c),
                                                 sources(_s), package(_p) {
    CsmConfig::FileInitialize(cxt, config, sources, package);
}

CsmCommand::OptArg CsmCommand::GetOptType(const std::string &str) {
    if (str == "-c" || str == "--clean")
        return CsmCommand::OptArg::clean;
    else if (str == "-r" || str == "--redo")
        return CsmCommand::OptArg::redo;
    else if (str == "-f" || str == "--fix")
        return CsmCommand::OptArg::fix;
    else if (str == "-h" || str == "--help")
        return CsmCommand::OptArg::help;
    else if (str == "-v" || str == "--version")
        return CsmCommand::OptArg::version;
    return CsmCommand::OptArg::err;
};

CsmBase::CsmErr::ErrCodeClass CsmCommand::Parse(const std::string &cmd, std::vector<std::string> &args) {

    for (auto it = args.begin(); it != args.end(); it++) { //过滤选项options
        if (GetOptType(*it) != OptArg::err) {
            this->opts.insert((GetOptType(*it)));
            args.erase(it);
        }
    }

    try {
        if (cmd == "install")
            install(args);
        else if (cmd == "uninstall")
            uninstall(args);
        else if (cmd == "check")
            check(args);
        else if (cmd == "list")
            list(args);
        else if (cmd == "run")
            run(args);
    }
    catch (CsmBase::CsmErr e) {
        CsmBase::WriteErrLog(cxt, e);
        switch (e.errCode) {
            case CsmBase::CsmErr::ErrCodeClass::CsmanErr:
                return CsmBase::CsmErr::ErrCodeClass::CsmanErr;
            case CsmBase::CsmErr::ErrCodeClass::SyntaxErr:
                return CsmBase::CsmErr::ErrCodeClass::SyntaxErr;
            case CsmBase::CsmErr::ErrCodeClass::NetWorkErr: {
                std::cout << "csman:\twe suggest you using [csman] online, do you wanna interrupt now?[y/n]"
                          << std::endl;
                char c;
                std::cin >> c;
                if (c == 'y' || c == 'Y')
                    exit(0);
                return CsmBase::CsmErr::ErrCodeClass::OK;
            }
        }
    }
    return CsmBase::CsmErr::ErrCodeClass::OK;
};

void CsmCommand::install(std::vector<std::string> &args) {

    if (CsmBase::GetWordType(args[0]) != CsmBase::WordClass::PACNAME)
        throw CsmBase::CsmErr(CsmBase::CsmErr::ErrCodeClass::SyntaxErr, "Command.cpp->CsmCommand::install()",
                              "incorrect syntax of package's name from user.");
    if (args.size() < 2) // options参数补齐
        args[1] = "";


    switch (opts.size()) {
        case 0: {
            if (CsmBase::GetWordType(args[1]) != CsmBase::WordClass::VER)
                throw CsmBase::CsmErr(CsmBase::CsmErr::ErrCodeClass::SyntaxErr, "Command.cpp->CsmCommand::install()",
                                      "incorrect syntax of version from user.");

            std::vector<Json::Value> dep_json = this->sources->GetDep(args[0], args[1]);


            if (dep_json.size() != 0) {
                std::vector<std::string> dep_str = this->sources->DepsName(dep_json);
                std::cout << "csman: to install \"" << args[0] << "\", you need to install these dependencies before:"
                          << std::endl;
                for (auto x:dep_str)
                    std::cout << "\t" << x << std::endl;
                std::cout << "\tdo you wan to install them[y/n]?" << std::endl;
                char c;
                std::cin >> c;
                if (c == 'y' || c == 'Y') {
                    for (auto url:this->sources->DepsUrl(dep_json)) {
                        std::string name = CsmBase::GetNameVerFromURL(url),
                                path = cxt->CS_IMPORT_PATH + "/" + name;

                        std::ifstream ifs(path + "/" + name + ".zip");
                        if(ifs.is_open())
                            continue;

                        CsmBase::HttpRequestGet(url, path + "/" + name + ".zip", cxt->reconnectTime);
                        // 画根进度条嘛
                        CsmBase::Unzip(path + "/" + name + ".zip");
                    }
                } else
                    return; // 用户作死正常退出
            }


            break;
        }
        case 1: {
            if (opts.find(OptArg::fix) != opts.end()) {
                std::string url = this->sources->GetPacJson(args[0], args[1])["ContentUrl"].asString();
                std::string name = CsmBase::GetNameVerFromURL(url),
                        path = cxt->CS_IMPORT_PATH + "/" + name + "/" + name + ".zip";
                CsmBase::HttpRequestGet(url, path, cxt->reconnectTime);
                // 画根进度条嘛
                CsmBase::Unzip(path);
            }
            if (opts.find(OptArg::redo) != opts.end()) {

                if (CsmBase::GetWordType(args[1]) != CsmBase::WordClass::VER)
                    throw CsmBase::CsmErr(CsmBase::CsmErr::ErrCodeClass::SyntaxErr,
                                          "Command.cpp->CsmCommand::install()",
                                          "incorrect syntax of version from user.");

                std::vector<Json::Value> dep_json = this->sources->GetDep(args[0], args[1]);


                if (dep_json.size() != 0) {
                    std::vector<std::string> dep_str = this->sources->DepsName(dep_json);
                    std::cout << "csman: to reinstall \"" << args[0]
                              << "\", you need to reinstall these dependencies before:"
                              << std::endl;
                    for (auto x:dep_str)
                        std::cout << "\t" << x << std::endl;
                    std::cout << "\tdo you wan to reinstall them[y/n]?" << std::endl;
                    char c;
                    std::cin >> c;
                    if (c == 'y' || c == 'Y') {
                        for (auto url:this->sources->DepsUrl(dep_json)) {
                            std::string name = CsmBase::GetNameVerFromURL(url),
                                    path = cxt->CS_IMPORT_PATH + "/" + name;
                            if (std::remove(path.c_str()) != 0) {
                                std::cout
                                        << "csman: delete original packages' files failed. please make sure \"csman\" has permission to delete files."
                                        << std::endl;
                                throw CsmBase::CsmErr(CsmBase::CsmErr::ErrCodeClass::ProtectErr,
                                                      "Command.cpp->CsmCommand::install()",
                                                      "delete original files failed.");
                            }

                            CsmBase::HttpRequestGet(url, path + "/" + name + ".zip", cxt->reconnectTime);
                            // 画根进度条嘛
                            CsmBase::Unzip(path);
                        }
                    } else
                        return; // 用户作死正常退出
                }
            }

            break;
        }
        default:
            throw CsmBase::CsmErr(CsmBase::CsmErr::ErrCodeClass::SyntaxErr, "Command.cpp->CsmCommand::install()",
                                  "incorrect options from user.");
    }
}

void CsmCommand::uninstall(std::vector<std::string> &args) {

}

void CsmCommand::check(std::vector<std::string> &args) {

}

void CsmCommand::list(std::vector<std::string> &args) {

}

void CsmCommand::run(std::vector<std::string> &args) {

}