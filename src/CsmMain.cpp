#include <CsmBase.h>
#include <CsmConfig.h>
//#include "Command.hpp"



int main(int argc, char **argv) {

    Json::CharReaderBuilder builder;
    JSONCPP_STRING jsonErrs;
    Json::Value root;
    std::ifstream ifs("/mnt/c/Users/Rend/Desktop/installed.json");
    Json::parseFromStream(builder, ifs, &root, &jsonErrs);

    std::cout << root["cs.runtime"].isMember("3.3.3.12");

    return 0;
}