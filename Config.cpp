//
// Created by Rend on 2020/9/25.
//
#include "Config.hpp"
/*Definition of ‘CsmJsonFile’*/
CsmConfig::CsmJsonFile::CsmJsonFile(const std::string &_path):path(_path){

    PreLaunching();

    Json::CharReaderBuilder builder;
    JSONCPP_STRING jsonErrs;
    Json::Value root;
    std::ifstream ifs(path);
    if (!ifs.is_open()) {
        ifs.close();
        throw std::ios_base::failure("Opening "+ path +" as ifstream failed.");
    } else {
        if (!Json::parseFromStream(builder, ifs, &root, &jsonErrs)) {
            ifs.close();
            throw std::ios_base::failure("Opening "+ path + " as json failed.");
        }
    }

    EndLaunching();

}
    /*Definition of ’Installed‘*/
    bool CsmConfig::Installed::ContiansPackage(const std::string &name,const Json::Value &pac){
        return root["Package"].isMember(pac.getMemberNames()[0]);
    }
    bool CsmConfig::Installed::ContainsRuntime(const std::string &name,const Json::Value &pac){
        return root["Runtime"].isMember(pac.getMemberNames()[0]);
    }
    void CsmConfig::Installed::AddPackage(const std::string &name,const Json::Value &pac){
        root["Package"][name].append(pac);
        return;
    }
    void CsmConfig::Installed::DelPackage(const std::string &name,const Json::Value &pac){
        root["Package"][name].removeMember(pac.getMemberNames()[0]);
        return ;
    }
    void CsmConfig::Installed::AddRuntime(const std::string &name,const Json::Value &pac){
        root["Runtime"][name].append(pac);
        return;
    }
    void CsmConfig::Installed::DelRuntime(const std::string &name,const Json::Value &pac){
        root["Runtime"][name].removeMember(pac.getMemberNames()[0]);
        return ;
    }
/*Definition of ’Config‘*/
void CsmConfig::Config::InitialConfig(){
    //搞个默认模板config给写上去
    Json::Value root;
    root["CsmanPath"] = homePath+"/.csman";
    root["MaxReconnectTime"] = 3;
    root["CurrentRuntime"]["Version"] = nullptr;
    root["CsmanPath"]["ABI"] = nullptr;
    root["CsmanPath"]["STD"] = nullptr;
    std::ifstream ifs(homePath+"/.csman_config.json");
    if(!ifs.is_open())
        throw CsmBase::CsmErr("","open \".csman_config.json\" failed when initialing.");
    ifs>>root;
    ifs.close();
}
bool CsmConfig::Config::ValidateConfig() {
    Json::CharReaderBuilder builder;
    JSONCPP_STRING jsonErrs;
    Json::Value root;
    std::ifstream ifs(homePath+"/.csman_config.json");;

    if (!ifs.is_open()) {
        throw CsmBase::CsmErr("ValidateConfig()","Loading [.csman_config.json] as ifstream failed.");
        return false;
    } else {
        if (!Json::parseFromStream(builder, ifs, &root, &jsonErrs)) {
            ifs.close();
            throw CsmBase::CsmErr("ValidateConfig()","Loading [.csman_config.json] as json failed.");
            return false;
        }
    }
    ifs.close();

    if(root["CsmanPath"].isNull() || root["CsmanPath"].empty()) return false;
    if(root["MaxReconnectTime"].isNull() || root["MaxReconnectTime"].empty() || !root["MaxReconnectTime"].isInt()) return false;
    if(root["CurrentRuntime"].isNull() || root["CurrentRuntime"].empty()) return false;
    else{
        if(root["CurrentRuntime"]["Version"].isNull() || root["CurrentRuntime"]["Version"].empty()) return false;
        if(root["CurrentRuntime"]["ABI"].isNull() || root["CurrentRuntime"]["ABI"].empty()) return false;
        if(root["CurrentRuntime"]["STD"].isNull() || root["CurrentRuntime"]["STD"].empty()) return false;
    }
//    currentRuntime = root["CurrentRuntime"].asString(); 不在此处读cruntime

    return true;
}
void CsmConfig::Config::PreLaunching(){
    std::ifstream ifs(homePath+"/.csman_config.json");
    bool CONFIG_EXIST = ifs.is_open();
    ifs.close();

    if(!CONFIG_EXIST)  //第一次使用需要初始化config
        InitialConfig();
    else if(!ValidateConfig()){ // 有配置文件但config损坏需要初始化
        std::cout<<"csman:\t配置文件已损坏，如想继续需重置配置文件。\n是否以默认方式重置配置文件？[y/n]"<<std::endl;
        std::string str;
        std::cin>>str;
        if(str[0]=='y')
            InitialConfig();
        else throw CsmBase::CsmErr("class CsmConfig()","[.csman_config.json] was broken.");
    }
}
void CsmConfig::Config::EndLaunching(){
    csmanPath = root["CsmanPath"].asString();
    reconnectTime = root["MaxReconnectTime"].asInt();
    currentRuntime = root["CurrentRuntime"].asString();
}
CsmConfig::Config::~Config(){
    if (root.empty() || root.isNull()) {
        CsmBase::WriteErrLog(CsmBase::CsmErr("CsmConfig.hpp->~CsmConfig():",
                                             "configure data is empty while saving [.csman_config], maybe it has been broken in memory."));
        return;
    }
    std::ofstream ofs(homePath + "/.csman_config.json");
    ofs << root;
    ofs.close();
}

/*Definition of ’Sources‘*/
Json::Value CsmConfig::Sources::Package(const std::string &str) {
    for(auto cat:root.getMemberNames()){
        if(root[cat].isMember(str))
            return root[cat][str];
    }
    return Json::Value::null;
}
void CsmConfig::Sources::JsonMerge(Json::Value &a, Json::Value &b) {
    if (!a.isObject() || !b.isObject()) return;
    for (const auto &key : b.getMemberNames()) {
        if (a[key].empty())
            a[key] = b[key];
        else
            JsonMerge(a[key], b[key]);
    }
}
void CsmConfig::Sources::PreLaunching(){
    /*Verify Connection to csman.info*/
    std::string request = CsmBase::HttpRequestGet("http://csman.info/csman.json", reconnectTime);
    if (request.empty()) {
        /*I WANNA RED WORDS HERE!!!!*/
        std::cout << "csman:\tcan not connect to server, some download function may work incorrectly."
                  << std::endl;
        throw CsmBase::CsmErr("","downloading [csman.json] failed from csman.info.");
    }

    /*Parse Json from csman.info*/
    Json::CharReaderBuilder builder;
    JSONCPP_STRING jsonErrs;
    Json::Value root;
    /*[csman.json]*/
    std::unique_ptr<Json::CharReader> jsonReader(builder.newCharReader());
    if (!jsonReader->parse(request.c_str(), request.c_str() + request.length(), &root, &jsonErrs)) {
        throw CsmBase::CsmErr("","parsing [csman.json] failed.");
    }
    /*[version validation]*/
    if (root["Version"] != csmanVersion) {
        throw CsmBase::CsmErr("","version is wrong.");
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
            throw "downloading [Generic.json] failed from csman.info.";
        }
        if (!jsonReader->parse(request.c_str(), request.c_str() + request.length(), &genericRoot, &jsonErrs)) {
            throw CsmBase::CsmErr("","parsing [Generic.json] failed.");
        }
        /*[Linux_GCC_AMD64.json]*/
        if (platform == "linux") {
            request = CsmBase::HttpRequestGet(baseUrl + "/Linux_GCC_AMD64.json", reconnectTime);
            if (request.empty()) {
                /*I WANNA RED WORDS HERE!!!!*/
                std::cout << "csman:\tcan not connect to server, some download function may work incorrectly."
                          << std::endl;
                throw CsmBase::CsmErr("","downloading [Linux_GCC_AMD64.json] failed from csman.info.");
            }
            if (!jsonReader->parse(request.c_str(), request.c_str() + request.length(), &platformRoot, &jsonErrs)) {
                throw CsmBase::CsmErr("","parsing [Linux_GCC_AMD64.json] failed.");
            }
            /*[macOS_clang_AMD64.json]*/
        } else if (platform == "MacOS") {
            request = CsmBase::HttpRequestGet(baseUrl + "/macOS_clang_AMD64.json", reconnectTime);
            if (request.empty()) {
                /*I WANNA RED WORDS HERE!!!!*/
                std::cout << "csman:\tcan not connect to server, some download function may work incorrectly."
                          << std::endl;
                throw CsmBase::CsmErr("","downloading [macOS_clang_AMD64.json] failed from csman.info.");
            }
            if (!jsonReader->parse(request.c_str(), request.c_str() + request.length(), &platformRoot, &jsonErrs)) {
                throw CsmBase::CsmErr("","parsing [macOS_clang_AMD64.json] failed.");
            }
            /*[Win32_MinGW-w64_AMD64.json]*/
        } else if (platform == "WIN") {
            request = CsmBase::HttpRequestGet(baseUrl + "/Win32_MinGW-w64_AMD64.json", reconnectTime);
            if (request.empty()) {
                /*I WANNA RED WORDS HERE!!!!*/
                std::cout << "csman:\tcan not connect to server, some download function may work incorrectly."
                          << std::endl;
                throw CsmBase::CsmErr("","downloading [Win32_MinGW-w64_AMD64.json] failed from csman.info.");
            }
            if (!jsonReader->parse(request.c_str(), request.c_str() + request.length(), &platformRoot, &jsonErrs)) {
                throw CsmBase::CsmErr("","parsing [Win32_MinGW-w64_AMD64.json] failed.");
            }
        }
    }
    JsonMerge(genericRoot, platformRoot);
    std::ofstream ofs;

    ofs.open(csmanPath + "/source.json");
    ofs << genericRoot;
    ofs.close();
}