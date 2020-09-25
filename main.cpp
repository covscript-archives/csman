#include "CsmSource.hpp"
#include "CsmBase.hpp"
#include "Command.hpp"
#include <stdio.h>
int main(int argc, char **argv) {
    curl_global_init(CURL_GLOBAL_ALL);  //launching libcurl
    Json::CharReaderBuilder builder;
    JSONCPP_STRING jsonErrs;
    Json::Value root;
    std::ifstream ifs("/mnt/c/Users/Rend/Desktop/.csman_config.json");
    Json::parseFromStream(builder, ifs, &root, &jsonErrs);
//    JSONCPP_STRING jsonErrs;
//    Json::Value x,y;
//    std::ifstream ifs;
//    ifs.open("/mnt/c/Users/Rend/Desktop/source.json");
//    if(!Json::parseFromStream(builder, ifs, &x, &jsonErrs))
//        std::cout<<"fuck";
//
//    std::cout<<x["Generic"][""]["cs.runtime"]["Dependencies"]<<std::endl;
//    if(x["Generic"][""]["cs.runtime"]["Dependencies"].empty())
//        std::cout<<"empty()"<<std::endl;
//    if(x["Generic"][""]["cs.runtime"]["aa"].isNull())
//        std::cout<<"NULL"<<std::endl;
//    Csmcmd::UninstallPackage(x,"","","");
//    CsmBase::HttpRequestGet("https://csman.info/sources/Generic.json",
//                                 "/mnt/c/Users/Rend/Desktop/test.josn",5)

//    int sb = 5 ;
//    CsmSource::DownloadSource("/home/rend","20.2.2","linux",sb);
    return 0;
}