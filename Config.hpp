//
// Created by Rend on 2020/8/13.
//
#pragma once

#include<vector>
#include<cstdio>
#include<cstring>
#include<fstream>
#include<iostream>
#include<algorithm>

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

namespace Config {

    Json::Value LoadConfig();
    bool SaveConfig();
    bool ContiansPackage(const std::string &name,const Json::Value &pac);
    bool ContainsRuntime(const std::string &name,const Json::Value &pac);
    void AddPackage(const std::string &name,const Json::Value &pac);
    void DelPackage(const std::string &name,const Json::Value &pac);
    void AddRuntime(const std::string &name,const Json::Value &pac);
    void DelRuntime(const std::string &name,const Json::Value &pac);

    void InitialConfig() {
        Json::Value root;
        std::ofstream ofs(homePath+"/.csman_config.json");
        root["CsmanPath"] = csmanPath = homePath + "/.csman/";
        ofs<<root;
    }

    bool ValidateConfig();

    bool CSMANInitial();

};
bool Config::ContiansPackage(const std::string &name,const Json::Value &pac){
    return config["Package"].isMember(pac.getMemberNames()[0]);
}
bool Config::ContainsRuntime(const std::string &name,const Json::Value &pac){
    return config["Runtime"].isMember(pac.getMemberNames()[0]);
}
void Config::AddPackage(const std::string &name,const Json::Value &pac){
    config["Package"][name].append(pac);
    return;
}
void Config::DelPackage(const std::string &name,const Json::Value &pac){
    config["Package"][name].removeMember(pac.getMemberNames()[0]);
    return ;
}
void Config::AddRuntime(const std::string &name,const Json::Value &pac){
    config["Runtime"][name].append(pac);
    return;
}
void Config::DelRuntime(const std::string &name,const Json::Value &pac){
    config["Runtime"][name].removeMember(pac.getMemberNames()[0]);
    return ;
}
