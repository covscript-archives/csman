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

namespace CsmConfig{
    class CsmJsonFile;
    class Config;
    class Sources;
    class Installed;
    bool CSMANInitial();
}

class CsmConfig::CsmJsonFile{
    public:
    std::string path;
    Json::Value root;
    virtual void PreLaunching(){}
    virtual void EndLaunching(){}
    CsmJsonFile(const std::string &_path);/*Reading JSON*/
};
class CsmConfig::Installed: CsmJsonFile{
public:
    bool ContiansPackage(const std::string &name,const Json::Value &pac);
    bool ContainsRuntime(const std::string &name,const Json::Value &pac);
    void AddPackage(const std::string &name,const Json::Value &pac);
    void DelPackage(const std::string &name,const Json::Value &pac);
    void AddRuntime(const std::string &name,const Json::Value &pac);
    void DelRuntime(const std::string &name,const Json::Value &pac);
};
class CsmConfig::Config : CsmJsonFile{
public:
    void InitialConfig();
    bool ValidateConfig();
    virtual void PreLaunching();
    virtual void EndLaunching();
    ~Config();
};
class CsmConfig::Sources : CsmJsonFile{
public:
    Json::Value Package(const std::string &name);
    void JsonMerge(Json::Value &a, Json::Value &b);
    void PreLaunching();
};

void CsmConfig::CSMANInitial() {

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
    /*launching libcurl*/
    curl_global_init(CURL_GLOBAL_ALL);
    if(config.root["CurrentRuntime"].isNull() || config.root["CurrentRuntime"].empty()){  //初次使用，需要指定版本

    }
}