//
// Created by Rend on 2020/9/6.
//
#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include "curl/curl.h"
#include "json/json.h"
#include "zipper/unzipper.h"

extern std::string homePath,csmanPath, platform, currentRuntime;
extern const std::string csmanVersion = "20.09.30";
extern int reconnectTime;

size_t CurlWriteString(char *buffer, size_t size, size_t count, std::string *data) {
    size_t recv_size = size * count;
    data->append(buffer);
    return recv_size;
}
size_t CurlWriteFileBin(char *buffer, size_t size, size_t count, FILE *data) {
    size_t recv_size = fwrite(buffer,size,count,data);
    return recv_size;
}

namespace CsmBase{
    /*
int HttpStatusCode(const std::string &url){
    CURL *curl = curl_easy_init();
    CURLcode res;
    int statusCode;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl,CURLOPT_TCP_KEEPALIVE,1);
    curl_easy_setopt(curl, CURLOPT_NOBODY,1);
    res = curl_easy_perform(curl);
    if(res == CURLE_OK)
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &statusCode);
    curl_easy_cleanup(curl);
    return statusCode;
}*/
    bool HttpRequestGet(const std::string &url, const std::string &path,int reconnectTime){
        CURL *curl;
        FILE *fp;
        remove(path.c_str());
        fp = fopen(path.c_str(),"wb");
        curl = curl_easy_init();
        curl_easy_setopt(curl,CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl,CURLOPT_WRITEDATA,fp);
        curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION, &CurlWriteFileBin);
        while(curl_easy_perform(curl)!=CURLE_OK && reconnectTime-- > 0)
            ;
        if(reconnectTime <= 0){
            remove(path.c_str());
            return false;
        }
        return true;
    }
    //WriteFunction 是分数据包次数写的。。。会被回调很多次，所以考虑重复动作和继续上一次的写入什么的。。。
    std::string HttpRequestGet(const std::string &url,int reconnectTime) {
        std::string response;
        CURL *curl;
        curl = curl_easy_init();
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl,CURLOPT_TCP_KEEPALIVE,1);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &CurlWriteString);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        while(curl_easy_perform(curl)!=CURLE_OK && reconnectTime-- > 0)
            response.clear();
        curl_easy_cleanup(curl);
        return response;
    }
    bool Unzip(const std::string& path) {
        zipper::Unzipper unzipper(path);
        unzipper.extract();
        unzipper.close();
        return true;
    }
    std::string Nowtime() {
        time_t now = time(0);
        std::string res = "";
        res += "\tlocaltime:\t";
        res += ctime(&now);
        res += "\tUTCtime:\t";
        res += asctime(gmtime(&now));
        return res;
    }
    class CsmErr{
    public:
        std::string title,content;
        CsmErr(const std::string &_title,const std::string &_content) : title(_title), content(_content){
            if (*(title.end() - 1) == '\n')title.erase(title.end() - 1);
            title = "=====\t" + title;
            title += "\t=====\n";
        };
    };

    void WriteErrLog(const CsmErr &err) {
        std::ofstream ofs(csmanPath + "/error_log.txt", std::ios::app);
        ofs << err.title << Nowtime() << "\n" << err.content << std::endl;
        ofs.close();
    }
}