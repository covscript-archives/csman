//
// Created by Rend on 2020/8/13.
//
#pragma once

#include <map>
#include <string>
#include <vector>
#include <cstring>
#include <iostream>
#include <functional>

#include "CsmBase.hpp"

//main.cpp只调用该文件内方法


namespace CsmCmd {

    bool InstallPackageFromCat(const std::string &cat,const std::string &packageName, std::string version, const int &reconnectTime);
    bool InstallPackage(const std::string &packageName,
                        std::string version,
                        const int &reconnectTime);

    bool UninstallPackage(const std::string &packageName,
                          std::string version);

    std::map<std::string, std::function<void(std::vector<std::string>)> > instruction = {
            {std::string("install"),   [](const std::vector<std::string> &args) {

                if (WordType(args[0]) == WordClass::PACNAME) {
                    if (args.size() == 1) {   //install package

                    } else if (WordType(args[1]) == WordClass::ARG) {  //install package -arg -arg..

                    } else if (WordType(args[1]) == WordClass::VER) {
                        if (args.size() == 2) {   //install package x.x.x

                        } else if (WordType(args[2]) == WordClass::ARG) {  //install package x.x.x -arg -arg..

                        }
                    }
                }
                std::cout << "Wrong syntax. Use \"csman help\" to view usages." << std::endl;
                return;


                //check existence
                //download
                //unzip
                //done
            }},
            {std::string("uninstall"), [](const std::vector<std::string> &args) {
                //check existence
                //delete
                //done
            }},
            {std::string("list"),      [](const std::vector<std::string> &args) {
                //show
                //done
            }},
            {std::string("run"),       [](const std::vector<std::string> &args) {
                //check existence
                //run
                //done
            }},
    };
}




bool CsmCmd::InstallPackageFromCat(const std::string &cat, const std::string &packageName, std::string version, const int &reconnectTime){
    if(!Config::source[cat].isMember(packageName))
        return false;
    else{
        if (version == "") {    //未指定版本号，找最大版本，还能再改
            version = Config::source[cat][packageName]["Version"].getMemberNames()[0];
            for (auto &ver: Config::source[cat][packageName]["Version"].getMemberNames())
                if (version > ver)
                    version = ver;
        } else if (version == "Stable" || version == "Unstable") {   // unstable 不能用么？
            version = Config::source[cat][packageName][version].asString();
        } else {   //指定版本号却找不到，是否只会出现在用户行为中？不只的话，还需要写错误日志
            if(!Config::source[cat][packageName]["Version"].isMember(version))
                return false;
        }

        /*handling dependencies*/
        if (!Config::source[cat][packageName]["Version"][version]["Dependencies"].isNull() &&
            !Config::source[cat][packageName]["Version"][version]["Dependencies"].empty()) {
            const Json::Value &dep = Config::source[cat][packageName]["Version"][version]["Dependencies"];
            for (auto depPacName : dep.getMemberNames()) {
                if (!CsmCmd::InstallPackage(depPacName, dep[depPacName].asString(),
                                            reconnectTime))    // some dependencies installing failed.
                    return false;
            }
        }

        /*all dependencies done, download self*/
        if (!CsmBase::HttpRequestGet(Config::source[cat][packageName]["Version"][version]["ContentUrl"].asString(),
                                      Config::csmanPath + "/packages/" + packageName + "/" + version,
                                     reconnectTime)) {
            remove((Config::csmanPath + "/packages/" + packageName + "/" + version).c_str());
            throw CsmBase::CsmErr("Command.hpp->InstallPackageFromCat():","downloading package [" + packageName + version +
                                                                          "] in \""+cat+"\" category from source.json failed.");
        }

        return true;
    }
}

bool CsmCmd::InstallPackage(const std::string &packageName, std::string version, const int &reconnectTime) {

    return CsmCmd::InstallPackageFromCat("Generic", packageName, version, reconnectTime) ||
           CsmCmd::InstallPackageFromCat(Config::config["CurrentRuntime"]["ABI"].asString(), packageName, version, reconnectTime) ||
           CsmCmd::InstallPackageFromCat(Config::config["CurrentRuntime"]["STD"].asString(), packageName, version, reconnectTime);
}

bool CsmCmd::UninstallPackage(const std::string &packageName,
                              std::string version) {
}