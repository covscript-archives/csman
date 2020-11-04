//
// Created by Rend on 2020/9/25.
//
#include<CsmConfig.h>

// ___________________________
// Definition of ‘CsmJsonFile’
//____________________________
CsmJsonFile::CsmJsonFile(Context *&_cxt, const std::string &_path) : cxt(_cxt), path(_path) {
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

CsmJsonFile::CsmJsonFile(Context *&_cxt, const Json::Value &_root) : cxt(_cxt), root(_root) {
    PreLaunching();
    EndLaunching();
}

// ______________________
// Definition of ’Sources
// ______________________
// Sources包依赖功能中，将Json::Value视为图的节点单位
// 也许所有函数都可以换成指针，不过测试过了，二轮再换吧我没把握

void CsmConfig::Sources::JsonMerge(Json::Value &a, Json::Value &b) {
    if (!a.isObject() || !b.isObject()) return;
    for (const auto &key : b.getMemberNames()) {
        if (a[key].empty())
            a[key] = b[key];
        else
            JsonMerge(a[key], b[key]);
    }
}


void CsmConfig::Sources::PreLaunching() {
    /*Verify Connection to csman.info*/
    std::vector<char> request = CsmBase::HttpRequestGet("http://csman.info/csman.json", cxt->reconnectTime);
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
    if (root["Version"] != cxt->csmanVersion) {
        throw CsmBase::CsmErr(CsmBase::CsmErr::ErrCodeClass::CsmanErr,
                              "CsmConfig.cpp->CsmConfig::Sources::PreLaunching()",
                              "current version \"" + cxt->csmanVersion +
                              "\" is not available, please try to update [csman] via \"csman update csman\".");
    }

    /*get Generic.json and Platform.json*/
    std::string baseUrl = root["BaseUrl"].asString();
    Json::Value genericRoot, platformRoot;
    {
        /*[Generic.json]*/
        request = CsmBase::HttpRequestGet(baseUrl + "/Generic.json", cxt->reconnectTime);
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
        if (cxt->platform == "linux") {
            request = CsmBase::HttpRequestGet(baseUrl + "/Linux_GCC_AMD64.json", cxt->reconnectTime);
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
        } else if (cxt->platform == "MacOS") {
            request = CsmBase::HttpRequestGet(baseUrl + "/macOS_clang_AMD64.json", cxt->reconnectTime);
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
        } else if (cxt->platform == "WIN") {
            request = CsmBase::HttpRequestGet(baseUrl + "/Win32_MinGW-w64_AMD64.json", cxt->reconnectTime);
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

    this->path = cxt->csmanPath + "/source.json";
    std::ofstream ofs(this->path);
    ofs << genericRoot;
    ofs.close();
}

Json::Value CsmConfig::Sources::GetPacJson(const std::string &name, const std::string &ver) {

    static Json::Value stable, unstable;

    /*firstly find in ABI*/
    Json::Value &abiPac = root[cxt->currentRuntime.ABI][name];
    if (!abiPac.isNull() && !abiPac.empty()) {
        if (!ver.empty()) {
            if (!abiPac["Version"][ver].isNull() && !abiPac["Version"][ver].empty() &&
                abiPac["Version"][ver].isObject())
                return abiPac["Version"][ver];
        }
        stable = abiPac["Stable"];
        unstable = abiPac["Unstable"];
        if (!stable.isNull() && !stable.empty() && stable.isString())
            return abiPac["Version"][stable.asString()];
        else if (!unstable.isNull() && !unstable.empty() && unstable.isString()) {
            WarningQueue.add(name + " " + unstable.asString() + " is not stable.");
            return abiPac["Version"][unstable.asString()];
        }
    }
    /*secondly find in STD*/
    Json::Value &stdPac = root[cxt->currentRuntime.STD][name];
    if (!stdPac.isNull() && !stdPac.empty()) {
        if (!ver.empty()) {
            if (!stdPac["Version"][ver].isNull() && !stdPac["Version"][ver].empty() &&
                stdPac["Version"][ver].isObject())
                return stdPac["Version"][ver];
        }
        stable = stdPac["Stable"];
        unstable = stdPac["Unstable"];
        if (!stable.isNull() && !stable.empty() && stable.isString())
            return stdPac["Version"][stable.asString()];
        else if (!unstable.isNull() && !unstable.empty() && unstable.isString()) {
            WarningQueue.add(name + " " + unstable.asString() + " is not stable.");
            return stdPac["Version"][unstable.asString()];
        }
    }
    /*thirdly find in Generic*/
    Json::Value &genPac = root["Generic"][name];
    if (!genPac.isNull() && !genPac.empty()) {
        if (!ver.empty()) {
            if (!genPac["Version"][ver].isNull() && !genPac["Version"][ver].empty() &&
                genPac["Version"][ver].isObject())
                return genPac["Version"][ver];
        }
        stable = genPac["Stable"];
        unstable = genPac["Unstable"];
        if (!stable.isNull() && !stable.empty() && stable.isString())
            return genPac["Version"][stable.asString()];
        else if (!unstable.isNull() && !unstable.empty() && unstable.isString()) {
            WarningQueue.add(name + " " + unstable.asString() + " is not stable.");
            return genPac["Version"][unstable.asString()];
        }
    }
}

std::vector<Json::Value> CsmConfig::Sources::GetDep(const std::vector<Json::Value> &vec_pac) {
    vis.clear();
    std::vector<Json::Value> res;
    for (auto pac:vec_pac) {
        static std::vector<Json::Value> r = DFSDeps(pac);
        res.reserve(res.size() + r.size());
        res.insert(res.end(), r.begin(), r.end());
    }
    return res;
}

std::vector<Json::Value> CsmConfig::Sources::GetDep(const std::vector<std::string> &vec_str) {
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


std::vector<Json::Value> CsmConfig::Sources::GetDep(const Json::Value &pac) {
    vis.clear();
    return DFSDeps(pac);
};

std::vector<Json::Value> CsmConfig::Sources::GetDep(const std::string &name,const std::string &ver) {
    vis.clear();
    return DFSDeps(GetPacJson(name, ver));
}


std::vector<Json::Value> CsmConfig::Sources::DFSDeps(const Json::Value &pac) {
    std::vector<Json::Value> res;
    std::string nameVer = CsmBase::GetNameVerFromURL(pac["ContentUrl"].asString());
    if (vis.find(nameVer) != vis.end()) // 当前包找过了
        return res; // 空json值有问题吗？
    this->vis.insert(nameVer);
    res.push_back(pac);
    for (auto d : pac["Dependencies"].getMemberNames()) {
        static std::string v = pac["Dependencies"][d].asString();
        // d依赖（包）名 v版本
        static std::vector<Json::Value> s = DFSDeps(
                GetPacJson(d, v));
        // s d依赖（包）的依赖集合
        res.reserve(res.size() + s.size());
        res.insert(res.end(), s.begin(), s.end());
    }
    return res;
}

std::vector<std::string> CsmConfig::Sources::DepsUrl(const std::vector<Json::Value> &vec_pac) {
    std::vector<std::string> res(vec_pac.size());
    for (auto d:vec_pac)
        res.push_back( d["ContentUrl"].asString());
    return res;
};

std::vector<std::string> CsmConfig::Sources::DepsName(const std::vector<Json::Value> &vec_pac) {
    std::vector<std::string> res(vec_pac.size());
    for (auto d:vec_pac)
        res.push_back(CsmBase::GetNameVerFromURL(d["ContentUrl"].asString()));
    return res;
};

// ______________________
// Definition of ’Config‘
// ______________________
void CsmConfig::Config::Initialize() {
    Json::Value root;
    root["CsmanPath"] = cxt->homePath + "/.csman";
    root["MaxReconnectTime"] = 3;

    std::ifstream ifs(cxt->homePath + "/.csman_config.json");
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
    std::ifstream ifs(cxt->homePath + "/.csman_config.json");
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
    cxt->csmanPath = root["CsmanPath"].asString();
    cxt->reconnectTime = root["MaxReconnectTime"].asInt();
}

CsmConfig::Config::~Config() {
    if (root.empty() || root.isNull()) {
        CsmBase::WriteErrLog(cxt, CsmBase::CsmErr(CsmBase::CsmErr::ErrCodeClass::CsmanErr,
                                                  "CsmConfig.hpp->CsmConfig::Config::~Config():",
                                                  "configure data is empty while saving [.csman_config], maybe it has been broken when which was in memory."));
        return;
    }
    std::ofstream ofs(cxt->homePath + "/.csman_config.json");
    ofs << root;
    ofs.close();
}


// __________________________
// Definition of ’Package‘
// __________________________

void CsmConfig::Package::PreLaunching() {
    path = cxt->csmanPath + "/.csman_installed.json";
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
                                  "CsmConfig.cpp->CsmConfig::Package::PreLaunching()",
                                  "[.csman_config.json] was broken.");
    }
}

bool CsmConfig::Package::Validate() {
    Json::CharReaderBuilder builder;
    JSONCPP_STRING jsonErrs;
    Json::Value rt;
    std::ifstream ifs(this->path);
    bool ISOPEN = ifs.is_open();
    ifs.close();

    if (!ISOPEN) {
        throw CsmBase::CsmErr(CsmBase::CsmErr::ErrCodeClass::CsmanErr,
                              "Config.hpp->CsmConfig::Package::Validate()",
                              "Loading [.csman_isntalled.json] as [ifstream type] failed.");
    } else {
        if (!Json::parseFromStream(builder, ifs, &rt, &jsonErrs)) {
            ifs.close();
            throw CsmBase::CsmErr(CsmBase::CsmErr::ErrCodeClass::CsmanErr,
                                  "Config.hpp->CsmConfig::Package::Validate()",
                                  "Loading [.csman_isntalled.json] as [json file] failed.");
        }
    }

    for (auto x:rt)
        if (!x.isArray())
            return false;

    return true;
}

void CsmConfig::Package::Initialize() {

}

std::vector<std::string> CsmConfig::Package::Absence(std::vector<std::string> Deps) {
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

bool CsmConfig::Package::Contains(const std::string &name, const std::string &ver) {
    if (this->root[name].isNull() || this->root[name].empty())
        return false;
    return this->root[name].isMember(ver);
}

void CsmConfig::Package::Add(const std::string &name, const std::string &ver) {
    for (int i = 0; i < root[name].size(); ++i)
        if (ver == root[name][i].asString())
            return;
    root[name].append(ver);
}

void CsmConfig::Package::Del(const std::string &name, const std::string &ver) {
    for (int i = 0; i < root[name].size(); ++i)
        if (ver == root[name][i].asString()) {
            root[name].removeIndex(i, &this->lastDelete);
            return;
        }
}

bool CsmConfig::FileInitialize(Context *&cxt, CsmJsonFile *config, CsmJsonFile *sources, CsmJsonFile *package) {
    try {
        config = new Config(cxt, cxt->homePath + "/.csman_config.json");
        sources = new Sources(cxt, cxt->csmanPath + "sources.json");
        package = new Package(cxt, cxt->csmanPath + "package.json");
    }
    catch (CsmBase::CsmErr e) {
        CsmBase::WriteErrLog(cxt, e);
        switch (e.errCode) {
            case CsmBase::CsmErr::ErrCodeClass::CsmanErr:
                break;
            case CsmBase::CsmErr::ErrCodeClass::SyntaxErr:
                break;
            case CsmBase::CsmErr::ErrCodeClass::ProtectErr:
                break;
            case CsmBase::CsmErr::ErrCodeClass::NetWorkErr:
                break;
        }
        return false;
    }
    return true;
}
