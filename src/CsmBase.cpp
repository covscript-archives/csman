//
// Created by Rend on 2020/10/11.
//

#include "CsmBase.h"

// Implementation of extern variables
CsmBase::Warning WarningQueue;

/* Context */
RuntimeInfo::RuntimeInfo() {
    mpp::process_builder builder;
    builder
            .command("cs")
            .arguments(std::vector<std::string>{"-v"})
            .merge_outputs(true);
    auto p = builder.start();
    /*缺少：进程启动失败，还未安装covscript*/
    auto &out = p.out();
    std::regex regVersion("Version: ([0-9\\.]+)"),
            regSTD("STD Version: ([0-9]{4}[0-9A-F]{2})"),
            regABI("ABI Version: ([0-9]{4}[0-9A-F]{2})"),
            regAPI("API Version: ([0-9]{4}[0-9A-F]{2})"),
            regBuild("Build ([0-9]+)");
    std::string _build, _version;
    while (out) {
        std::string line;
        std::getline(out, line);
        std::smatch std_match;

        if (std::regex_search(line, std_match, regSTD))
            this->STD = std_match[1];
        else if (std::regex_search(line, std_match, regABI))
            this->ABI = std_match[1];
        else if (std::regex_search(line, std_match, regAPI))
            /*do nothing*/;
        else if (std::regex_search(line, std_match, regVersion))
            _version = std_match[1];

        if (std::regex_search(line, std_match, regBuild)) {
            _build = std_match[1];
            continue;
        }
    }
    this->version = _version + _build;
    if (this->ABI.empty() || this->STD.empty() || this->version.empty())
        throw ;
}
Context::Context(const std::string &_COVSRIPT_HOME, const std::string &_CS_IMPORT_PATH,
                 const std::string &_CS_DEV_PATH, const std::string &_csmanPath,
                 const std::string &_platform, const std::string &_homePath,
                 const int &_reconnectTime) : COVSRIPT_HOME(_COVSRIPT_HOME),
                                              CS_IMPORT_PATH(_CS_IMPORT_PATH),
                                              CS_DEV_PATH(_CS_DEV_PATH),
                                              csmanPath(_csmanPath),
                                              platform(_platform),
                                              homePath(_homePath),
                                              reconnectTime(_reconnectTime),
                                              currentRuntime() {

}

// ________________
// tools of CsmBase
// ________________

void CsmBase::Warning::add(const std::string &str)
{
	this->message.push(str);
}

void CsmBase::Warning::printAll()
{
	std::cout << "csman:" << std::endl;
	while (!this->message.empty()) {
		std::cout << "\twarning:\t" << this->message.front() << std::endl;
		this->message.pop();
	}
}

bool CsmBase::IsABI(const std::string &str)
{
	static const std::regex reg("^ABI[0-9]{4}[0-9A-F]{2}$");
	return std::regex_match(str, reg);
}

bool CsmBase::IsSTD(const std::string &str)
{
	static const std::regex reg("^STD[0-9]{4}[0-9A-F]{2}$");
	return std::regex_match(str, reg);
}

bool CsmBase::IsGeneric(const std::string &str)
{
	return !str.compare("Generic");
}

bool CsmBase::IsVersion(const std::string &str)
{
	static const std::regex reg("^([0-9]+\\.){0,3}[0-9]+\\w*$");
	return std::regex_match(str, reg);
}

bool CsmBase::IsPackage(const std::string &str)
{
	static const std::regex reg("^(\\w+\\.)+\\w+$");
	return std::regex_match(str, reg);
}

bool CsmBase::IsArgument(const std::string &str)
{
	static const std::regex reg("^-[a-z]$");
	return std::regex_match(str, reg);
}

CsmBase::WordClass CsmBase::GetWordType(const std::string &str)
{
	if (CsmBase::IsABI(str)) return WordClass::ABI;
	else if (CsmBase::IsSTD(str)) return WordClass::STD;
	else if (CsmBase::IsVersion(str)) return WordClass::VER;
	else if (CsmBase::IsPackage(str)) return WordClass::PACNAME;
	else if (CsmBase::IsArgument(str)) return WordClass::ARG;
	return WordClass::ERR;
}

bool CsmBase::Unzip(const std::string &path)
{
	zipper::Unzipper unzipper(path);
	unzipper.extract();
	unzipper.close();
	return true;
}

std::string CsmBase::Nowtime()
{
	time_t now = time(0);
	std::string res = "";
	res += "\tlocaltime:\t";
	res += ctime(&now);
	res += "\tUTCtime:\t";
	res += asctime(gmtime(&now));
	return res;
}

std::vector<std::string> CsmBase::StrToVec(const std::string str)
{
	std::string buf = "";
	std::vector<std::string> res;
	for (char c:str) {
		if (c == ' ') {
			res.emplace_back(buf);
			buf = "";
		}
		else buf += c;
	}
	return res;
}

std::string CsmBase::GetNameVerFromURL(const std::string &url)
{
	std::string res = "";
	auto it = url.end() - 1;
	for (; *it == '/'; it--);
	it -= 4; // ".zip"
	for (; *it != '/'; it--)
		res.push_back(*it);
	reverse(res.begin(), res.end());
	return res;
}

bool CsmBase::HttpRequestGet(const std::string &url, const std::string &path, int reconnectTime)
{
	CURL *curl;
	FILE *fp;
	remove(path.c_str());
	fp = fopen(path.c_str(), "wb");
	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &CurlWriteFileBin);
	while (curl_easy_perform(curl) != CURLE_OK && reconnectTime-- > 0);
	if (reconnectTime <= 0) {
		remove(path.c_str());
		return false;
	}
	return true;
}

std::vector<char> CsmBase::HttpRequestGet(const std::string &url, int reconnectTime)
{
	std::vector<char> response;
	CURL *curl;
	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &CurlWriteCharBuff);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	while (curl_easy_perform(curl) != CURLE_OK && reconnectTime-- > 0)
		response.clear();
	curl_easy_cleanup(curl);
	return response;
}