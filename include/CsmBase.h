//
// Created by Rend on 2020/9/6.
//
#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <regex>
#include <curl/curl.h>
#include <json/json.h>
#include <mozart++/process>
#include <zipper/unzipper.h>

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

class RuntimeInfo {
public:
    std::string ABI, STD, version;

    RuntimeInfo();
};

class Context {
public:
    RuntimeInfo currentRuntime;
    std::string COVSRIPT_HOME, CS_IMPORT_PATH, CS_DEV_PATH, csmanPath, homePath, platform, csmanVersion;
    int reconnectTime;

    Context(const std::string &_COVSRIPT_HOME, const std::string &_CS_IMPORT_PATH,
            const std::string &_CS_DEV_PATH, const std::string &_csmanPath,
            const std::string &_platform, const std::string &_csmanVersion,
            const int &_reconnectTime);
};
//libcurl的WriteFunction 是分数据包次数写的。。。会被回调很多次，所以考虑重复动作和继续上一次的写入什么的。。。
size_t CurlWriteCharBuff(char *buffer, size_t size, size_t count, std::vector<char> *data) {
    size_t recv_size = size * count;
    data->reserve(recv_size);
    for (size_t i = 0; i < recv_size; ++i)
        data->push_back(buffer[i]);
    return recv_size;
}

size_t CurlWriteFileBin(char *buffer, size_t size, size_t count, FILE *data) {
    size_t recv_size = fwrite(buffer, size, count, data);
    return recv_size;
}

namespace CsmBase {

    enum class WordClass {
        STD, ABI, PACNAME, VER, ARG, ERR
    };

    bool IsABI(const std::string &str);

    bool IsSTD(const std::string &str);

    bool IsGeneric(const std::string &str);

    bool IsVersion(const std::string &str);

    bool IsPackage(const std::string &str);

    bool IsArgument(const std::string &str);    //目前小写
    WordClass GetWordType(const std::string &str);

    bool Unzip(const std::string &path);

    std::vector<std::string> StrToVec(const std::string str);

    std::string Nowtime();

    bool HttpRequestGet(const std::string &url, const std::string &path, int reconnectTime);

    std::vector<char> HttpRequestGet(const std::string &url, int reconnectTime);


    class Warning {
    private:
        std::queue<std::string> message;
    public:
        void add(const std::string &str);

        void printAll();
    };

    class CsmErr {
    public:
        enum class ErrCodeClass {
            ProtectErr, CsmanErr, SyntaxErr, NetWorkErr, OK
        };
        ErrCodeClass errCode;
        std::string title, content;

        CsmErr(ErrCodeClass _errCode, const std::string &_title, const std::string &_content) : errCode(_errCode),
                                                                                                title(_title),
                                                                                                content(_content) {
            if (*(title.end() - 1) == '\n')title.erase(title.end() - 1);
            title = "=====\t" + title;
            title += "\t=====\n";
        };
    };

    void WriteErrLog(const Context *cxt, const CsmErr &err) {
        std::ofstream ofs( cxt->csmanPath +"/error_log.txt", std::ios::app);
        ofs << err.title << Nowtime() << "\n" << err.content << std::endl;
        ofs.close();
    }

    std::string GetNameVerFromURL(const std::string &url);
}

extern CsmBase::Warning WarningQueue;