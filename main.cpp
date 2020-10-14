#include "CsmBase.hpp"
//#include "Config.hpp"
//#include "Command.hpp"

const std::string csmanVersion = "200930";
std::string homePath,csmanPath, platform;
int reconnectTime;

int main(int argc, char **argv) {

	/*CsmConfig::CSMANinital();
	try{
		CsmConfig::Config config;
		CsmConfig::Sources sources;
		CsmConfig::Installed installed;
	}
	catch(CsmBase::CsmErr err){
		CsmBase::WriteErrLog(err);
		return 0;
	}*/

	std::vector<char>str = CsmBase::HttpRequestGet("https://csman.info/sources/Generic.json",5);

	Json::CharReaderBuilder builder;
    JSONCPP_STRING jsonErrs;
    Json::Value root;
    /*[csman.json]*/
    std::unique_ptr<Json::CharReader> jsonReader(builder.newCharReader());
    jsonReader->parse(str.data(), str.data()+str.size(), &root, &jsonErrs);
    std::cout<<root;

    return 0;
}