//
// Created by Rend on 2020/12/6.
//
#pragma once
#include <mozart++/process>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <string>
#include <regex>
#include <set>

#include "file_io.hpp"
#include "dir.hpp"

bool is_abi(const std::string &str)
{
	static const std::regex reg("^ABI[0-9]{4}[0-9A-F]{2}$");
	return std::regex_match(str, reg);
}

bool is_std(const std::string &str)
{
	static const std::regex reg("^STD[0-9]{4}[0-9A-F]{2}$");
	return std::regex_match(str, reg);
}

bool is_ver(const std::string &str)
{
	static const std::regex reg("^([0-9]+\\.){0,3}[0-9]+\\w*$");
	return std::regex_match(str, reg);
}

bool is_pac(const std::string &str)
{
	static const std::regex reg("^(\\w+\\.)+\\w+$");
	return std::regex_match(str, reg);
}

class context {
private:
    void set_testing_var();
	void initialize_val();
	void get_covscript_env();
	void read_config();
	void write_config();
	std::set<std::string> only_read_vars;
	struct Config_Data {
		struct line_data {
			std::string text;
			bool is_comment;

			line_data(const std::string &text, bool is_comment) : text(text),
				is_comment(is_comment) {}// 该行不是注释，则记录key；反之记录该行注释内容
		};
		std::vector<line_data> lines;
	} config_data;
public:
	std::string ABI, STD, runtime_ver;
	std::unordered_map<std::string, std::string> vars;
	/*
	 * all variables in "configure vars":
	 * home_path
	 * COVSCRIPT_HOME, CS_IMPORT_PATH, CS_DEV_PATH
	 * config_path, csman_path, pac_repo_path, sources_idx_path, max_reconnect_time
	 */

	void show(const std::string &key);

	void show_all();

	void set(const std::string &key, const std::string &val);

	context(){
		try {
			initialize_val();
//			get_covscript_env();
//			read_config();
            set_testing_var();
		}
		catch (std::exception e) {
			throw e;
		}
	}
};

void context::set_testing_var() {
    vars["config_path"] = "../misc/.csman_config";
    vars["pac_repo_path"] = "../misc/pac_repo";
    vars["sources_idx_path"] = "../misc/sources_idx";
}
void context::initialize_val()
{

	vars["home_path"] =
#ifdef __linux__
	    std::getenv("HOME");
#elif _WIN32
	    std::getenv("USERPROFILE");
#elif __APPLE__
	    std::getenv(""); // 改
#endif
	const std::string &home = vars["home_path"];

	/*CovScript var*/
	if (std::getenv("COVSCRIPT_HOME") != nullptr) {
		vars["COVSCRIPT_HOME"] = std::getenv("COVSCRIPT_HOME");
		only_read_vars.insert("COVSCRIPT_HOME");
	}
	else {
		vars["COVSCRIPT_HOME"] =
#ifdef __linux__
		    "/usr/share/covscript/";
#elif _WIN32
		    home + "\\Documents\\CovScript\\";
#elif __APPLE__
		    "/Application/CovScript.app/Contents/";
#endif
	}
//**********************
	if (std::getenv("CS_IMPORT_PATH") != nullptr) {
		vars["CS_IMPORT_PATH"] = std::getenv("CS_IMPORT_PATH");
		only_read_vars.insert("CS_IMPORT_PATH");
	}
	else {
		vars["CS_IMPORT_PATH"] =
#ifdef __linux__
		    "";
#elif _WIN32
		    home + " ";
#elif __APPLE__
		    "";
#endif
	}
//**********************
	if (std::getenv("CS_DEV_PATH") != nullptr) {
		vars["CS_DEV_PATH"] = std::getenv("CS_DEV_PATH");
		only_read_vars.insert("CS_DEV_PATH");
	}
	else {
		vars["CS_DEV_PATH"] =
#ifdef __linux__
		    "";
#elif _WIN32
		    home + "";
#elif __APPLE__
		    "";
#endif
	}
	/*csman client var*/
	vars["config_path"] = home + "/.csman_config";
	vars["csman_path"] = home + "/.csman/";
	vars["pac_repo_path"] = vars["COVSCRIPT_HOME"] + "/pac_repo/";
	vars["sources_idx_path"] = vars["csman_path"] + "/sources_idx";
	vars["max_reconnect_time"] = "5";
}
void context::get_covscript_env()
{
	mpp::process_builder builder;
	builder.command("cs")
	.arguments(std::vector<std::string> {"-v"})
	.merge_outputs(true);
	auto p = builder.start();
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
	this->runtime_ver = _version + _build;
	if (this->ABI.empty() || this->STD.empty() || this->runtime_ver.empty())
		throw std::runtime_error("CovScript has not installed yet or maybe it has been broken.");
	return;
}
void context::read_config()
{
	const std::string &config_path = vars["config_path"];

	std::ifstream ifs(config_path);
	if (!path_exist(config_path)) {
		std::ofstream ofs(config_path);
		ofs.close();
		return;
	}

	std::vector<std::string> args;
	std::string text;
	while (std::getline(ifs, text)) {
		str_split(args,text);
		if (vars.count(args[0]) > 0) { // 如果先前初始化vars时添加过此key，则key为合法key
			vars[args[0]] = args[2]; // "key" "=" "var"
			config_data.lines.push_back(Config_Data::line_data(args[0], false));
		}
		else if(text[0]=='#')
			config_data.lines.push_back(Config_Data::line_data(text,true));
		else throw std::runtime_error("format of .csman_config is incorrect.");
	}
	return;
}
void context::write_config()
{
	std::ofstream ofs(vars["config_path"]);
	for(auto &l:config_data.lines) {
		if(l.is_comment)
			ofs<<l.text<<std::endl;
		else
			ofs<<l.text+" = "+vars[l.text]<<std::endl;
	}
	ofs.close();
}
void context::show_all()
{
	for(auto &x: vars)
		std::cout<<x.first<<" = "<<x.second<<std::endl;
}
void context::show(const std::string &key)
{
	if(vars.count(key)>0)
		std::cout<<key<<" = "<<vars[key]<<std::endl;
	else
		std::cout<<"no configure variable named \""<<key<<"\"!"<<std::endl;
}
void context::set(const std::string &key, const std::string &val)
{
	if(vars.count(key)>0 || only_read_vars.find(key)==only_read_vars.end()) {
		vars[key] = val;
		std::cout<<"set "<<key<<" as \""<<val<<"\" successfully."<<std::endl;
	}
	std::cout << "no configure variable named \"" << key << "\"!" << std::endl;
}