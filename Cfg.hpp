//
// Created by Rend on 2020/9/19.
//
#pragma once

#include<vector>
#include<cstdio>
#include<cstring>
#include<fstream>
#include<iostream>
#include<algorithm>
#include "CsmBase.hpp"

#if defined(__linux) || defined(__linux__) || defined(linux)
#include<sys/stat.h>
#include<unistd.h>
#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#include<io.h>
    #include<direct.h>
#elif defined(__APPLE__) || defined(__MACH__)
    #include<sys/stat.h>
    #include<unistd.h>
#endif

namespace Cfg{
    class CsmJsonFile;
    class Config;
    class Source;
    class Installed;
    bool CSMANInitial();
}

extern Cfg::Config config;
extern Cfg::Source source;
extern Cfg::Installed installed;

class Cfg::CsmJsonFile{
    public:
    std::string path;
    Json::Value root;
    virtual void PreLaunching(){}
    virtual void EndLaunching(){}
    CsmJsonFile(const std::string &_path):path(_path){

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
};
class Cfg::Installed: CsmJsonFile{
public:
    bool ContiansPackage(const std::string &name,const Json::Value &pac){
        return root["Package"].isMember(pac.getMemberNames()[0]);
    }
    bool ContainsRuntime(const std::string &name,const Json::Value &pac){
        return root["Runtime"].isMember(pac.getMemberNames()[0]);
    }
    void AddPackage(const std::string &name,const Json::Value &pac){
        root["Package"][name].append(pac);
        return;
    }
    void DelPackage(const std::string &name,const Json::Value &pac){
        root["Package"][name].removeMember(pac.getMemberNames()[0]);
        return ;
    }
    void AddRuntime(const std::string &name,const Json::Value &pac){
        root["Runtime"][name].append(pac);
        return;
    }
    void DelRuntime(const std::string &name,const Json::Value &pac){
        root["Runtime"][name].removeMember(pac.getMemberNames()[0]);
        return ;
    }
};
class Cfg::Config : CsmJsonFile{
public:
    void InitialConfig(){
        //搞个模板config给写上去
    }
    bool ValidateConfig() {
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
    void PreLaunching(){
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
            else throw CsmBase::CsmErr("class Config()","[.csman_config.json] was broken.");
        }
    }
    void EndLaunching(){
        csmanPath = root["CsmanPath"].asString();
        reconnectTime = root["MaxReconnectTime"].asInt();
        currentRuntime = root["CurrentRuntime"].asString();
    }
    ~Config(){
        if (root.empty() || root.isNull()) {
            CsmBase::WriteErrLog(CsmBase::CsmErr("Config.hpp->~Config():",
                                 "configure data is empty while saving [.csman_config], maybe it has been broken."));
            return;
        }
        std::ofstream ofs(homePath + "/.csman_config.json");
        ofs << root;
        ofs.close();
    }
};
class Cfg::Source : CsmJsonFile{
public:
    Json::Value PacInfo(const std::string &name){
        for(auto cat:root.getMemberNames()){
            if(root[cat].isMember(name))
                return root[cat][name];
        }
        return Json::Value::null;
    }
    void JsonMerge(Json::Value &a, Json::Value &b) {
        if (!a.isObject() || !b.isObject()) return;
        for (const auto &key : b.getMemberNames()) {
            if (a[key].empty())
                a[key] = b[key];
            else
                JsonMerge(a[key], b[key]);
        }
    }
    void PreLaunching(){
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
};

bool Cfg::CSMANInitial() {

#if defined(__linux) || defined(__linux__) || defined(linux)
    homePath = getenv("HOME");
    platform = "Linux";
#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
    homePath = getenv("USERPROFILE");
        platform = "WIN";
#elif defined(__APPLE__) || defined(__MACH__)
        homePath = getenv("HOME");
        platform = "MacOS";
#endif

    config = Cfg::Config(homePath+".csman_config.json");

    source = Cfg::Source(csmanPath+"source.json");

    if(config.root["CurrentRuntime"].isNull() || config.root["CurrentRuntime"].empty()){  //初次使用，需要指定版本

    }
}