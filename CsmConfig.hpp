//
// Created by Rend on 2020/9/19.
//
#pragma once

#include<set>
#include<vector>
#include<cstdio>
#include<cstring>
#include<fstream>
#include<iostream>
#include<algorithm>
#include "CsmBase.hpp"

class CsmJsonFile {
public:
    explicit CsmJsonFile(const std::string &_path);/*Reading JSON via constructor*/
    explicit CsmJsonFile(const Json::Value &_root);

    virtual ~CsmJsonFile() = default;

protected:
    std::string path;
    Json::Value root;

private:

    virtual void PreLaunching() {}

    virtual void EndLaunching() {}

    virtual void Initialize() {}

    virtual bool Validate() {}
};

namespace CsmConfig {
    class Config;

    class Sources;

    class Installed;

    bool CSMANInitial();
}
/*done*/
class CsmConfig::Config : CsmJsonFile {
private:
    void Initialize() override;

    bool Validate() override;

    void PreLaunching() override;

    void EndLaunching() override;

    ~Config() override;
};

class CsmConfig::Sources : CsmJsonFile {
public:

    std::vector<Json::Value> GetDepall(const std::vector<Json::Value> &vec_pac);

    std::vector<Json::Value> GetDepall(const std::vector<std::string> &vec_str);

    std::vector<Json::Value> GetDep(const Json::Value &pac);

    std::vector<Json::Value> GetDep(const std::string &name);

    std::vector<std::string>
    DepsName(const std::vector<Json::Value> &x); // turn vector of packages(JSON) into vector of pacnames(string)

private:

    void JsonMerge(Json::Value &a, Json::Value &b);

    void PreLaunching() override;

    std::set<std::string> vis;

    Json::Value GetPacJson(const std::string &name, const std::string &ver); // return a package INFO(JSON)

    std::vector<Json::Value>
    DFSDeps(const Json::Value &pac); // return a vecotr of packages(JSON), which means a dependencies of this package

    ~Sources() override = default;
};

class CsmConfig::Installed : CsmJsonFile {
public:
    std::vector<std::string> Absence(std::vector<std::string>Deps);

    bool Contains(const std::string &name, const std::string &ver);

    void Add(const std::string &name, const std::string &ver);

    void Del(const std::string &name, const std::string &ver);

private:
    virtual void PreLaunching() override;

    virtual void EndLaunching() override;

    virtual void Initialize() override;

    virtual bool Validate() override;
};

