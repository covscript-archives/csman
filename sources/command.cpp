/*
 * CovScript Manager: CLI Parser
 * Licensed under Apache 2.0
 * Copyright (C) 2020-2021 Chengdu Covariant Technologies Co., LTD.
 * Website: https://covariant.cn/
 * Github:  https://github.com/chengdu-zhirui/
 */
#include <csman/command.hpp>

namespace csman {
	void parser::download_unzip_pac(const std::string &name, const std::string &ver, const std::string &url)
	{
		std::string dir_path = str::to_dir_path(cxt->vars["pac_repo_path"] + delimiter + name + delimiter + ver);
		std::string zip_path = str::to_file_path(dir_path + "pac.zip");

		try {
			network::http_get(url, zip_path, std::stoi(cxt->vars["max_reconnect_time"]));
			cov::zip_extract(zip_path, dir_path);
			sys::file::remove(zip_path);
		}
		catch (const std::exception &e) {
			throw e;
		}
	}

	void parser::delete_pac(std::string &name, std::string &ver)
	{
		std::string pac_path(str::to_dir_path(cxt->vars["pac_repo_path"] + delimiter + name));
		if (!sys::exist(pac_path))    //  pac_path 路径不正确
			return;
		if (!sys::dir::remove(pac_path))    //删除目标路径下所有文件
			throw std::runtime_error("uninstall pack " + name + " " + ver +
			                         " failed, please check whether it's using by other progress.");
	}

	bool parser::y_or_n()   // 也许可以扔针对命令行的组件里
	{

		static char c = '#';
		std::cout << '>';
		while (c != 'y' && c != 'Y' && c != 'n' && c != 'N') {
			std::cout << "please type \'y\' or \'n\'." << std::endl;
			std::cin >> c;
		}
		return (c == 'y' || c == 'Y');
	}

	/*从args分离opt的filter, 分离后args将不带有opt参数*/
	void parser::opt_filter()     // 改
	{

		for (auto it = args.begin(); it != args.end(); it++) {
			if (it->operator[](0) == '-')
				opt.insert(*it), args.erase(it);
		}
	}

	void parser::install_covscript(){
	    if(args.size()<=2) // install covscript
	        ;
	    else if(csman::str::is_ver(args[2])) // install covscript x.x.x.x
	        ;
	    else if(args.size()>3)
	        throw std::invalid_argument("syntax error.");
	    else
	        throw std::invalid_argument("no such version of CovScript.");
	}

	parser::parser(context *_context, const std::vector<std::string> &arguments) : cxt(_context), args(arguments) {}

	void parser::parse()
	{
		try {
			opt_filter();

			if (args.size() == 0)
				predicate = "help";
			if (args.size() > 0) {
				predicate = args[0];
				if (args.size() > 1)
					object = args[1];
			}

			if (predicate == "help") {
				std::cout << HELP;
				return;
			}
			else if (predicate == "version") {
				std::cout << VERSION;
				return;
			}

			repo = pac_repo(cxt);
			repo.read_pac_list(cxt->vars["pac_repo_path"]+delimiter+"list");

			if (predicate == "list")
				list();
			else if (predicate == "config") // 可以预先指定covscript相关目录，可以允许未安装covscript情况下使用该命令
				config();

            cxt->get_covscript_env();
			idx = idx_file(cxt);
			if (predicate == "install")
				install();
			else if (predicate == "uninstall")
				uninstall();
			else
				throw std::invalid_argument("syntax error!");
		}
		catch (const std::exception &e) {
			throw e;
		}
	}

	void REPL()
	{
		while (true) {
			return;
		}
	}

	void cmd()
	{

	}
}

namespace csman {
	void parser::install()
	{
		std::string ver;
		if(csman::str::weak_equal(args[1],"covscript")) install_covscript(); // install covscript ....
		else if (args.size() <= 2) ver = idx.get_stable_ver(object); // install pac
		else if (args[2] == "stable") ver = idx.get_stable_ver(object); // install pac stable
		else if (args[2] == "unstable") ver = idx.get_unstable_ver(object); // install pac unstable
		else if (csman::str::is_ver(args[2])) ver = args[2]; // install pac x.x.x.x
		else throw std::invalid_argument("wrong package version.");

		try {
			// 获取依赖
			auto dep_set = idx.get_depend_set(object, ver);
			message.first_sentence("csman:", "installing " + object + " " + ver +
			                       " needs to install these packages all because of dependencies:");
			for (auto x: dep_set)
				message.content(x.name + " " + x.ver);
			message.first_sentence("do you want to install them all?", "[y/n]");
			// 是否安装?
			if (!y_or_n()) { // 否
				message.first_sentence("csman:", "operation interrupted by user.");
				return;
			}
			// 是 进入安装
			for (auto x : dep_set) {
				message.content("installing " + x.name + " " + x.ver + "...");
				download_unzip_pac(x.name, x.ver, x.url);
				repo.update_install(x.name, x.ver, true);
			}
			message.first_sentence("csman: install", object + " " + ver + " and it's dependencies successfully.");
		}
		catch (const std::exception &e) {
			throw e;
		}
	}

	void parser::uninstall()
	{
		std::string ver;
		if (args.size() <= 2)
			ver = idx.get_stable_ver(object); //以下寻找包版本,均应替换为pac_repo的接口,例: pac_repo.get_current_runtime_ver()
		else if (args[2] == "stable") ver = idx.get_stable_ver(object);
		else if (args[2] == "unstable") ver = idx.get_unstable_ver(object);
		else if (!csman::str::is_ver(args[2]))throw std::invalid_argument("wrong package version.");
		else ver = args[2];

		try {
			auto sup_set = idx.get_support_set(object, ver);
			message.first_sentence("csman:", "uninstalling " + object + " " + ver +
			                       " needs to uninstall these packages all because there are no other packages supported by them:");
			for (auto pac : sup_set) {
				if (idx.get_support_set(pac.name, pac.ver).size() != 0)
					sup_set.erase(pac);
				else
					message.content(pac.name + " " + pac.ver);
			}
			message.first_sentence("do you want to uninstall them all?", "[y/n]");

			if (!y_or_n()) { //否
				message.first_sentence("csman:", "operation interrupted by user.");
				return;
			}

			for (auto pac : sup_set) {
				message.content("uninstalling " + pac.name + " " + pac.ver + "...");
				delete_pac(pac.name, pac.ver);
				repo.update_uninstall(pac.name, pac.ver);
			}
			message.first_sentence("csman: uninstall", object + " " + ver + " and it's supports successfully.");
		}
		catch (const std::exception &e) {
			throw e;
		}
	}

	void parser::config()
	{
		if (object == "set") {
			if (args.size() == 4) {
				try {
					cxt->set(args[3], args[4]);
				}
				catch (const std::exception &e) {
					throw e;
				}
			}
			else throw std::invalid_argument("syntax error.");
		}
		else if (object == "show") {
			if (args.size() == 2)
				cxt->show_all();
			else if (args.size() == 3)
				cxt->show(args[2]);
			else throw std::invalid_argument("syntax error.");
		}
		else
			throw std::invalid_argument("syntax error.");
	}

	void parser::list()
	{
		std::cout << "Currently you have installed these packages:" << std::endl;
		for (auto x : repo.local_pac) {
			std::cout << x.first << ":" << std::endl;
			for (auto v: x.second.ver) {
				std::cout << v;
				if (v == x.second.available)
					std::cout << "(current runtime using: CovScript " << cxt->runtime_ver << ")";
				std::cout << std::endl;
			}
		}
	}

	void parser::checkout()
	{

	}
}