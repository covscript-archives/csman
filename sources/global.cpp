/**
 * CovScript Manager: Global Functions Implementation
 * Licensed under Apache 2.0
 * Copyright (C) 2020-2021 Chengdu Covariant Technologies Co., LTD.
 * Website: https://covariant.cn/
 * Github:  https://github.com/chengdu-zhirui/
 */
#include <mozart++/process>
#include <csman/global.hpp>
#include <iostream>
#include <fstream>

void csman::str_split(std::vector<std::string> &args, const std::string &s)
{
	std::string buf;
	args.clear();
	for (auto c : s) {
		if (std::isspace(c)) {
			if (!buf.empty()) {
				args.push_back(buf);
				buf.clear();
			}
		}
		else
			buf += c;
	}
	if (!buf.empty())
		args.push_back(buf);
}

bool csman::readline(std::ifstream &ifs, std::vector<std::string> &args)
{
	std::string s;
	if (!std::getline(ifs, s))
		return false;
	str_split(args, s);
	return true;
}

void csman::context::set_testing_var()
{
	vars["config_path"] = "../misc/.csman_config";
	vars["pac_repo_path"] = "../misc/pac_repo";
	vars["sources_idx_path"] = "../misc/sources_idx";
}

void csman::context::initialize_val()
{

	const char* home_ptr =
#ifdef MOZART_PLATFORM_LINUX
	    std::getenv("HOME");
#elif defined(MOZART_PLATFORM_WIN32)
	    std::getenv("USERPROFILE");
#elif defined(MOZART_PLATFORM_DARWIN)
	    std::getenv("HOME"); // 改
#endif

	if (home_ptr == nullptr)
		throw std::runtime_error("Get EnvVar Error: HOME");

	vars["home_path"] = home_ptr;

	std::string home = home_ptr;

	/*CovScript var*/
	if (std::getenv("COVSCRIPT_HOME") != nullptr) {
		vars["COVSCRIPT_HOME"] = std::getenv("COVSCRIPT_HOME");
		only_read_vars.insert("COVSCRIPT_HOME");
	}
	else {
		vars["COVSCRIPT_HOME"] =
#ifdef MOZART_PLATFORM_LINUX
		    "/usr/share/covscript/";
#elif defined(MOZART_PLATFORM_WIN32)
		    home + "\\Documents\\CovScript\\";
#elif defined(MOZART_PLATFORM_DARWIN)
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
#ifdef MOZART_PLATFORM_LINUX
		    "";
#elif defined(MOZART_PLATFORM_WIN32)
		    home + " ";
#elif defined(MOZART_PLATFORM_DARWIN)
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
#ifdef MOZART_PLATFORM_LINUX
		    "";
#elif defined(MOZART_PLATFORM_WIN32)
		    home + "";
#elif defined(MOZART_PLATFORM_DARWIN)
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

void csman::context::get_covscript_env()
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

void csman::context::read_config()
{
	const std::string &config_path = vars["config_path"];

	std::ifstream ifs(config_path);
	if (!directory::path_exist(config_path)) {
		std::ofstream ofs(config_path);
		ofs.close();
		return;
	}

	std::vector<std::string> args;
	std::string text;
	while (std::getline(ifs, text)) {
		str_split(args, text);
		if (vars.count(args[0]) > 0) { // 如果先前初始化vars时添加过此key，则key为合法key
			vars[args[0]] = args[2]; // args[0]:key args[1]:'=' args[2]:var
			config_data.lines.push_back(Config_Data::line_data(args[0], false));
		}
		else if (text[0] == '#')
			config_data.lines.push_back(Config_Data::line_data(text, true));
		else throw std::runtime_error("format of .csman_config is incorrect.");
	}
	return;
}

void csman::context::write_config()
{
	std::ofstream ofs(vars["config_path"]);
	for (auto &l:config_data.lines) {
		if (l.is_notes)
			ofs << l.text << std::endl;
		else
			ofs << l.text + " = " + vars[l.text] << std::endl;
	}
	ofs.close();
}

void csman::context::show_all()
{
	for (auto &x: vars)
		std::cout << x.first << " = " << x.second << std::endl;
}

void csman::context::show(const std::string &key)
{
	if (vars.count(key) > 0)
		std::cout << key << " = " << vars[key] << std::endl;
	else
		std::cout << "no configure variable named \"" << key << "\"!" << std::endl;
}

void csman::context::set(const std::string &key, const std::string &val)
{
	if (vars.count(key) > 0 || only_read_vars.find(key) == only_read_vars.end()) {
		vars[key] = val;
		std::cout << "set " << key << " as \"" << val << "\" successfully." << std::endl;
	}
	std::cout << "no configure variable named \"" << key << "\"!" << std::endl;
}