//
// Created by Rend on 2020/9/25.
//

#include "CsmConfig.h"

// ___________________________
// Definition of ‘CsmJsonFile’
//____________________________
CsmJsonFile::CsmJsonFile(const std::string &_path) : path(_path) {
    PreLaunching();

    Json::CharReaderBuilder builder;
    JSONCPP_STRING jsonErrs;
    Json::Value root;
    std::ifstream ifs(path);
    if (!ifs.is_open()) {
        ifs.close();
        throw CsmBase::CsmErr(CsmBase::CsmErr::ErrCodeClass::CsmanErr,
                              "CsmConfig.cpp->CsmJsonFile",
                              "Opening " + path + " as [ifstream type] failed.");
    } else {
        if (!Json::parseFromStream(builder, ifs, &root, &jsonErrs)) {
            ifs.close();
            throw CsmBase::CsmErr(CsmBase::CsmErr::ErrCodeClass::CsmanErr,
                                  "CsmConfig.cpp->CsmJsonFile",
                                  "Opening " + path + " as [json file] failed.");
        }
    }

    EndLaunching();
}

CsmJsonFile::CsmJsonFile(const Json::Value &_root) : root(_root) {
    PreLaunching();
    EndLaunching();
}

// ______________________
// Definition of ’Sources
// ______________________
// Sources包依赖功能中，将Json::Value视为图的节点单位
// 也许所有函数都可以换成指针，不过测试过了，二轮再换吧我没把握
/*done*/
void CsmConfig::Sources::JsonMerge(Json::Value &a, Json::Value &b) {
    if (!a.isObject() || !b.isObject()) return;
    for (const auto &key : b.getMemberNames()) {
        if (a[key].empty())
            a[key] = b[key];
        else
            JsonMerge(a[key], b[key]);
    }
}

/*done*/
void CsmConfig::Sources::PreLaunching() {
    /*Verify Connection to csman.info*/
    std::vector<char> request = CsmBase::HttpRequestGet("http://csman.info/csman.json", reconnectTime);
    if (request.empty()) {
        /*I WANNA RED WORDS HERE!!!!*/
        std::cout << "csman:\tcan not connect to server, some download function may work incorrectly."
                  << std::endl;
        throw CsmBase::CsmErr(CsmBase::CsmErr::ErrCodeClass::NetWorkErr,
                              "CsmConfig.cpp->CsmConfig::Sources::PreLaunching()",
                              "downloading [csman.json] failed from https://csman.info.");
    }

    /*Parse Json from csman.info*/
    Json::CharReaderBuilder builder;
    JSONCPP_STRING jsonErrs;
    Json::Value root;
    /*[csman.json]*/
    std::unique_ptr<Json::CharReader> jsonReader(builder.newCharReader());
    if (!jsonReader->parse(request.data(), request.data() + request.size(), &root, &jsonErrs)) {
        throw CsmBase::CsmErr(CsmBase::CsmErr::ErrCodeClass::CsmanErr,
                              "CsmConfig.cpp->CsmConfig::Sources::PreLaunching()",
                              "parsing [csman.json] as [json file] failed.");
    }
    /*[version validation]*/
    if (root["Version"] != csmanVersion) {
        throw CsmBase::CsmErr(CsmBase::CsmErr::ErrCodeClass::CsmanErr,
                              "CsmConfig.cpp->CsmConfig::Sources::PreLaunching()",
                              "current version \"" + csmanVersion +
                              "\" is not available, please try to update [csman] via \"csman update csman\".");
    }

    /*get Generic.json and Platform.json*/
    std::string baseUrl = root["BaseUrl"].asString();
    Json::Value genericRoot, platformRoot;
    {
        /*[Generic.json]*/
        request = CsmBase::HttpRequestGet(baseUrl + "/Generic.json", reconnectTime);
        if (request.empty()) {
            /*I WANNA RED WORDS HERE!!!!*/
            std::cout << "csman:\tcan not connect to server, some download-function may work incorrectly."
                      << std::endl;
            throw CsmBase::CsmErr(CsmBase::CsmErr::ErrCodeClass::NetWorkErr,
                                  "CsmConfig.cpp->CsmConfig::Sources::PreLaunching()",
                                  "downloading [Generic.json] failed from csman.info.");
        }
        if (!jsonReader->parse(request.data(), request.data() + request.size(), &genericRoot, &jsonErrs)) {
            throw CsmBase::CsmErr(CsmBase::CsmErr::ErrCodeClass::CsmanErr,
                                  "CsmConfig.cpp->CsmConfig::Sources::PreLaunching()",
                                  "parsing [Generic.json] as [json file] failed.");
        }
        /*[Linux_GCC_AMD64.json]*/
        if (platform == "linux") {
            request = CsmBase::HttpRequestGet(baseUrl + "/Linux_GCC_AMD64.json", reconnectTime);
            if (request.empty()) {
                /*I WANNA RED WORDS HERE!!!!*/
                std::cout << "csman:\tcan not connect to server, some download function may work incorrectly."
                          << std::endl;
                throw CsmBase::CsmErr(CsmBase::CsmErr::ErrCodeClass::NetWorkErr,
                                      "CsmConfig.cpp->CsmConfig::Sources::PreLaunching()",
                                      "downloading [Linux_GCC_AMD64.json] failed from https://csman.info.");
            }
            if (!jsonReader->parse(request.data(), request.data() + request.size(), &platformRoot, &jsonErrs)) {
                throw CsmBase::CsmErr(CsmBase::CsmErr::ErrCodeClass::CsmanErr,
                                      "CsmConfig.cpp->CsmConfig::Sources::PreLaunching()",
                                      "parsing [Linux_GCC_AMD64.json] as [json file] failed.");
            }
            /*[macOS_clang_AMD64.json]*/
        } else if (platform == "MacOS") {
            request = CsmBase::HttpRequestGet(baseUrl + "/macOS_clang_AMD64.json", reconnectTime);
            if (request.empty()) {
                /*I WANNA RED WORDS HERE!!!!*/
                std::cout << "csman:\tcan not connect to server, some download function may work incorrectly."
                          << std::endl;
                throw CsmBase::CsmErr(CsmBase::CsmErr::ErrCodeClass::NetWorkErr,
                                      "CsmConfig.cpp->CsmConfig::Sources::PreLaunching()",
                                      "downloading [macOS_clang_AMD64.json] failed from https://csman.info.");
            }
            if (!jsonReader->parse(request.data(), request.data() + request.size(), &platformRoot, &jsonErrs)) {
                throw CsmBase::CsmErr(CsmBase::CsmErr::ErrCodeClass::CsmanErr,
                                      "CsmConfig.cpp->CsmConfig::Sources::PreLaunching()",
                                      "parsing [macOS_clang_AMD64.json] as [json file] failed.");
            }
            /*[Win32_MinGW-w64_AMD64.json]*/
        } else if (platform == "WIN") {
            request = CsmBase::HttpRequestGet(baseUrl + "/Win32_MinGW-w64_AMD64.json", reconnectTime);
            if (request.empty()) {
                /*I WANNA RED WORDS HERE!!!!*/
                std::cout << "csman:\tcan not connect to server, some download function may work incorrectly."
                          << std::endl;
                throw CsmBase::CsmErr(CsmBase::CsmErr::ErrCodeClass::NetWorkErr,
                                      "CsmConfig.cpp->CsmConfig::Sources::PreLaunching()",
                                      "downloading [Win32_MinGW-w64_AMD64.json] failed from https://csman.info.");
            }
            if (!jsonReader->parse(request.data(), request.data() + request.size(), &platformRoot, &jsonErrs)) {
                throw CsmBase::CsmErr(CsmBase::CsmErr::ErrCodeClass::CsmanErr,
                                      "CsmConfig.cpp->CsmConfig::Sources::PreLaunching()",
                                      "parsing [Win32_MinGW-w64_AMD64.json] as [json flile] failed.");
            }
        }
    }

    JsonMerge(genericRoot, platformRoot);

    this->path = csmanPath + "/source.json";
    std::ofstream ofs(this->path);
    ofs << genericRoot;
    ofs.close();
}

/*done*/
Json::Value CsmConfig::Sources::GetPacJson(const std::string &name, const std::string &ver) {

    static Json::Value stable, unstable;

    /*firstly find in ABI*/
    if (!root[currentRuntime.ABI][name].isNull() && !root[currentRuntime.ABI][name].empty()) {
        if (!ver.empty()) {
            if (!root[currentRuntime.ABI][name][ver].isNull() && !root[currentRuntime.ABI][name][ver].empty() &&
                root[currentRuntime.ABI][name][ver].isObject())
                return root[currentRuntime.ABI][name][ver];
        }
        stable = root[currentRuntime.ABI][name]["Stable"];
        unstable = root[currentRuntime.ABI][name]["Unstable"];
        if (!stable.isNull() && !stable.empty() && stable.isString())
            return root[currentRuntime.ABI][name][stable.asString()];
        else if (!unstable.isNull() && !unstable.empty() && unstable.isString()) {
            WarningQueue.add(name + " " + unstable.asString() + " is not stable.");
            return root[currentRuntime.ABI][name][stable.asString()];
        }
    }
    /*secondly find in STD*/
    if (!root[currentRuntime.STD][name].isNull() && !root[currentRuntime.STD][name].empty()) {
        if (!ver.empty()) {
            if (!root[currentRuntime.STD][name][ver].isNull() && !root[currentRuntime.STD][name][ver].empty() &&
                !root[currentRuntime.STD][name][ver].isObject())
                return root[currentRuntime.ABI][name][ver];
        }
        stable = root[currentRuntime.STD][name]["Stable"];
        unstable = root[currentRuntime.STD][name]["Unstable"];
        if (!stable.isNull() && !stable.empty() && stable.isString())
            return root[currentRuntime.STD][name][stable.asString()];
        else if (!unstable.isNull() && !unstable.empty() && unstable.isString()) {
            WarningQueue.add(name + " " + unstable.asString() + " is not stable.");
            return root[currentRuntime.STD][name][stable.asString()];
        }
    }
    /*thirdly find in Generic*/
    if (!root["Generic"][name].isNull() && !root["Generic"][name].empty()) {
        if (!ver.empty()) {
            if (!root["Generic"][name][ver].isNull() && !root["Generic"][name][ver].empty() &&
                !root["Generic"][name][ver].isObject())
                return root["Generic"][name][ver];
        }

        stable = root["Generic"][name]["Stable"];
        unstable = root["Generic"][name]["Unstable"];
        if (!stable.isNull() && !stable.empty() && stable.isString())
            return root["Generic"][name][stable.asString()];
        else if (!unstable.isNull() && !unstable.empty() && unstable.isString()) {
            WarningQueue.add(name + " " + unstable.asString() + " is not stable.");
            return root["Generic"][name][stable.asString()];
        }
    }

    return Json::Value::null;
}

/*done*/
std::vector<Json::Value> CsmConfig::Sources::GetDepall(const std::vector<Json::Value> &vec_pac) {
    vis.clear();
    std::vector<Json::Value> res;
    for (auto pac:vec_pac) {
        static std::vector<Json::Value> r = DFSDeps(pac);
        res.reserve(res.size() + r.size());
        res.insert(res.end(), r.begin(), r.end());
    }
    return res;
}

/*done*/
std::vector<Json::Value> CsmConfig::Sources::GetDepall(const std::vector<std::string> &vec_str) {
    vis.clear();
    std::vector<Json::Value> res;
    static Json::Value pac;
    for (auto str:vec_str) {
        for (int i = str.size() - 1; i >= 0; i--)
            if (str[i] == '_')
                pac = GetPacJson(str.substr(0, i - 1), str.substr(i + 1, str.size() - 1));
        static std::vector<Json::Value> r = DFSDeps(pac);
        res.reserve(res.size() + r.size());
        res.insert(res.end(), r.begin(), r.end());
    }
    return res;
}

/*done*/
std::vector<Json::Value> CsmConfig::Sources::GetDep(const Json::Value &pac) {
    vis.clear();
    return DFSDeps(pac);
};

/*done*/
std::vector<Json::Value> CsmConfig::Sources::GetDep(const std::string &name) {
    vis.clear();
    return DFSDeps(GetPacJson(name, ""));
}

/*done*/
std::vector<Json::Value> CsmConfig::Sources::DFSDeps(const Json::Value &pac) {
    std::vector<Json::Value> res;
    if (vis.find(CsmBase::GetNameFromURL(pac["ContentUrl"].asString())) != vis.end())
        return res;
    this->vis.insert(CsmBase::GetNameFromURL(pac["ContentUrl"].asString()));
    res.push_back(pac);
    for (auto d : pac["Dependencies"].getMemberNames()) {
        static std::string v = pac["Dependencies"][d].asString();
        // d依赖名（包） v版本
        static std::vector<Json::Value> s = DFSDeps(
                GetPacJson(d, v));
        // s d依赖（包）的依赖集合
        res.reserve(res.size() + s.size());
        res.insert(res.end(), s.begin(), s.end());
    }
    return res;
}

/*done*/
std::vector<std::string> CsmConfig::Sources::DepsName(const std::vector<Json::Value> &vec_pac) {
    std::vector<std::string> res(vec_pac.size());
    for (auto d:vec_pac) {
        res.push_back(CsmBase::GetNameFromURL(d["ContentUrl"].asString()));
    }
    return res;
};

// ______________________
// Definition of ’Config‘
// ______________________
void CsmConfig::Config::Initialize() {
    Json::Value root;
    root["CsmanPath"] = homePath + "/.csman";
    root["MaxReconnectTime"] = 3;

    root["CurrentRuntime"]["Version"] = currentRuntime.version;
    root["CurrentRuntime"]["ABI"] = currentRuntime.ABI;
    root["CurrentRuntime"]["STD"] = currentRuntime.STD;
    std::ifstream ifs(homePath + "/.csman_config.json");
    if (!ifs.is_open()) {
        ifs.close();
        throw CsmBase::CsmErr(CsmBase::CsmErr::ErrCodeClass::CsmanErr,
                              "CsmConfig.cpp->CsmConfig::Config::Initialize()",
                              "open [.csman_config.json] failed when initialing.");
    }
    ifs >> root;
    ifs.close();
}

bool CsmConfig::Config::Validate() {
    Json::CharReaderBuilder builder;
    JSONCPP_STRING jsonErrs;
    Json::Value rt;
    std::ifstream ifs(this->path);
    bool ISOPEN = ifs.is_open();
    ifs.close();

    if (!ISOPEN) {
        throw CsmBase::CsmErr(CsmBase::CsmErr::ErrCodeClass::CsmanErr,
                              "CsmConfig.cpp->CsmConfig::Config::Validate()",
                              "Loading [.csman_config.json] as [ifstream type] failed.");
    } else {
        if (!Json::parseFromStream(builder, ifs, &rt, &jsonErrs)) {
            ifs.close();
            throw CsmBase::CsmErr(CsmBase::CsmErr::ErrCodeClass::CsmanErr,
                                  "CmsConfig.cpp->CsmConfig::Config::Validate()",
                                  "Loading [.csman_config.json] as [json file] failed.");
        }
    }


    if (rt["CsmanPath"].isNull() || rt["CsmanPath"].empty() || !rt["CsmanPath"].isString()) return false;
    if (rt["MaxReconnectTime"].isNull() || rt["MaxReconnectTime"].empty() || !rt["MaxReconnectTime"].isInt())
        return false;
    if (rt["CurrentRuntime"].isNull() || rt["CurrentRuntime"].empty() || !rt["CurrentRuntime"].isString()) return false;
    else {
        if (rt["CurrentRuntime"]["Version"].isNull() || rt["CurrentRuntime"]["Version"].empty() ||
            !rt["CurrentRuntime"]["Version"].isString())
            return false;
        if (rt["CurrentRuntime"]["ABI"].isNull() || rt["CurrentRuntime"]["ABI"].empty() ||
            !rt["CurrentRuntime"]["ABI"].isString())
            return false;
        if (rt["CurrentRuntime"]["STD"].isNull() || rt["CurrentRuntime"]["STD"].empty() ||
            !rt["CurrentRuntime"]["STD"].isString())
            return false;
    }

    return true;
}

void CsmConfig::Config::PreLaunching() {
    std::ifstream ifs(homePath + "/.csman_config.json");
    bool ISOPEN = ifs.is_open();
    ifs.close();

    if (!ISOPEN)  //第一次使用需要初始化config
        Initialize();
    else if (!Validate()) { // 有配置文件但config损坏需要初始化
        std::cout << "csman:\tyour configure file is broken, would you like to rebuild it? [y/n]" << std::endl;
        char c;
        std::cin >> c;
        if (c == 'y' || c == 'Y')
            Initialize();
        else
            throw CsmBase::CsmErr(CsmBase::CsmErr::ErrCodeClass::ProtectErr,
                                  "CmsConfig.cpp->CsmConfig::Config::PreLaunching()",
                                  "[.csman_config.json] is broken and the sucker seemed not happy to rebuild it.");
    }
}

void CsmConfig::Config::EndLaunching() {
    csmanPath = root["CsmanPath"].asString();
    reconnectTime = root["MaxReconnectTime"].asInt();
    currentRuntime.version = root["CurrentRuntime"]["Version"].asString();
    currentRuntime.ABI = root["CurrentRuntime"]["ABI"].asString();
    currentRuntime.STD = root["CurrentRuntime"]["STD"].asString();
}

CsmConfig::Config::~Config() {
    if (root.empty() || root.isNull()) {
        CsmBase::WriteErrLog(CsmBase::CsmErr(CsmBase::CsmErr::ErrCodeClass::CsmanErr,
                                             "CsmConfig.hpp->CsmConfig::Config::~Config():",
                                             "configure data is empty while saving [.csman_config], maybe it has been broken when which was in memory."));
        return;
    }
    std::ofstream ofs(homePath + "/.csman_config.json");
    ofs << root;
    ofs.close();
}


// __________________________
// Definition of ’Installed‘
// __________________________
/*done*/
void CsmConfig::Installed::PreLaunching() {
    path = csmanPath + "/.csman_installed.json";
    std::ifstream ifs(path);
    bool INSTALLED_EXIST = ifs.is_open();
    ifs.close();

    if (!INSTALLED_EXIST)  //第一次使用需要初始化installed
        Initialize();
    else if (!Validate()) { // 有配置文件但config损坏需要初始化
        std::cout << "csman:\tyour catalogue of installed file is broken, would you like to rebuild it? [y/n]"
                  << std::endl;
        char c;
        std::cin >> c;
        if (c == 'y' || c == 'Y')
            Initialize();
        else
            throw CsmBase::CsmErr(CsmBase::CsmErr::ErrCodeClass::CsmanErr,
                                  "CsmConfig.cpp->CsmConfig::Installed::PreLaunching()",
                                  "[.csman_config.json] was broken.");
    }
}

/*done*/
bool CsmConfig::Installed::Validate() {
    Json::CharReaderBuilder builder;
    JSONCPP_STRING jsonErrs;
    Json::Value rt;
    std::ifstream ifs(this->path);
    bool ISOPEN = ifs.is_open();
    ifs.close();

    if (!ISOPEN) {
        throw CsmBase::CsmErr(CsmBase::CsmErr::ErrCodeClass::CsmanErr,
                              "Config.hpp->CsmConfig::Installed::Validate()",
                              "Loading [.csman_isntalled.json] as [ifstream type] failed.");
    } else {
        if (!Json::parseFromStream(builder, ifs, &rt, &jsonErrs)) {
            ifs.close();
            throw CsmBase::CsmErr(CsmBase::CsmErr::ErrCodeClass::CsmanErr,
                                  "Config.hpp->CsmConfig::Installed::Validate()",
                                  "Loading [.csman_isntalled.json] as [json file] failed.");
        }
    }

    for (auto x:rt)
        if (!x.isArray())
            return false;

    return true;
}

/*flag*/
void CsmConfig::Installed::Initialize() {

}

/*done*/
std::vector<std::string> CsmConfig::Installed::Absence(std::vector<std::string> Deps) {
    std::vector<std::string> res;
    for (auto d:Deps) {
        for (int i = d.size() - 1; i >= 0; --i)
            if (d[i] == '_') {
                if (!this->Contains(d.substr(0, i - 1), d.substr(i + 1, d.size() - 1)))
                    res.push_back(d);
            }
    }
    return res;
}

/*done*/
bool CsmConfig::Installed::Contains(const std::string &name, const std::string &ver) {
    if (this->root[name].isNull() || this->root[name].empty())
        return false;
    return this->root[name].isMember(ver);
}

void CsmConfig::Installed::Add(const std::string &name, const std::string &ver) {
    root[name].append(ver);
    return;
}

void CsmConfig::Installed::Del(const std::string &name, const std::string &ver) {
    root[name].removeMember(ver);
    return;
}

bool CsmConfig::FileInitialize(CsmJsonFile *config, CsmJsonFile *sources, CsmJsonFile *installed) {
    try{
        config = new Config(homePath+"/.csman_config.json");
        sources = new Sources(csmanPath+"sources.json");
        installed = new Installed(csmanPath+"installed.json");
    }
    catch (CsmBase::CsmErr e) {
        CsmBase::WriteErrLog(e);
        switch(e.errCode){
            case CsmBase::CsmErr::ErrCodeClass::CsmanErr: break;
            case CsmBase::CsmErr::ErrCodeClass::ArgErr: break;
            case CsmBase::CsmErr::ErrCodeClass::ProtectErr: break;
            case CsmBase::CsmErr::ErrCodeClass::NetWorkErr: break;
        }
        return false;
    }
    return true;
}