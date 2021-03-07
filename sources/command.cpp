//
// Created by Rend on 2020/12/3.
//
#include"csman/pac_repo.hpp"
#include"csman/global.hpp"
#include"csman/http.hpp"
#include"csman/idx.hpp"
#include"zip.hpp"
#include<iostream>
#include<string>
#include<vector>
#include<set>

class parser {
private:
	context *cxt;   // 上下文，存取全局变量与信息
	idx_file idx;   // sources_idx，可下载包信息，负责依赖查询，支持查询
	pac_repo repo;  // pac_repo, 包仓库
	std::vector<std::string> args;  // 用户命令的参数，例如install xxx
	std::set<std::string> opt;  // 从args里分离出的可选参数，例如-r
	std::string predicate, object;  // 谓语，宾语

	void download_unzip_pac(const std::string &name,const std::string &ver,const std::string &url)
	{
		std::string dir_path = cxt->vars["pac_repo_path"] + "/" + name + "/" + ver + "/";
		std::string zip_path = dir_path + "pac.zip";
		try {
			http_get(url, zip_path, std::stoi(cxt->vars["max_reconnect_time"]));
			cov::zip_extract(zip_path, dir_path);
		}
		catch(std::exception &e) {
			throw e;
		}
	}

	void delete_pac(std::string& name, std::string& ver)
	{
		std::string pac_path(cxt->vars["pac_repo_path"] + '/' + name);
		if(!path_exist(pac_path))    //  pac_path 路径不正确
			return ;
		if(!remove_dir(pac_path))    //删除目标路径下所有文件
			throw std::runtime_error("uninstall pack" + name + " " + ver + " failed, please check whether it's using by other progress.");
	}

	const std::string HELP = "usage: csman <command> [objects] [args]\n"
	                         "\n"
	                         "Online commands:\n"
	                         "\tinstall\t\t\tInstall a package.\n"
	                         "\tuninstall\t\tUninstall a package.\n"
	                         "Offline commands:\n"
	                         "\thelp\t\t\tCheck the usage of csman.\n"
	                         "\tlist\t\t\tCheck your all packages' situation of CovScript.\n"
	                         "\tconfig\t\t\tManage your arguments of CovScript.\n"
	                         "\tversion\t\t\tCheck version of csman.\n"
	                         "\tcheckout\t\tChange your CovScript's runtime version.\n"
	                         "\n"
	                         "Go to our page for more support: http://123.com";

	const std::string VERSION = "csman 1.0";

	inline bool y_or_n()
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
	inline void opt_filter()   // 改
	{
		for (auto it = args.begin(); it != args.end(); it++) {
			if (it->size() == 2 && it->operator[](0) == '-')
				opt.insert(*it), args.erase(it);
		}
	}

	struct {
		std::string title;

		inline void first_sentence(const std::string &_title, const std::string &_content)
		{
			title = _title;
			std::cout << title << '\t' << _content << std::endl;
		}

		inline void content(const std::string &_content)
		{
			std::cout << '\t' << _content << std::endl;
		}
	} message;

public:
	parser(context *cxt, const std::vector<std::string> &a) : cxt(cxt), idx(cxt), args(a), repo(cxt) {}

	void parse()
	{
		try {
			opt_filter();
			predicate = args[0];
			object = args[1];
			if (predicate == "install")
				install();
			else if (predicate == "uninstall")
				uninstall();
			else if (predicate == "help")
				std::cout<<HELP;
			else if (predicate == "list")
				list();
			else if (predicate == "version")
				std::cout<<VERSION;
			else if (predicate == "config")
				config();
			else
				throw std::invalid_argument("syntax error!");
		}
		catch (std::exception &e) {
			throw e;
		}
	}

	void install()
	{
		std::string ver;
		if (args.size() <= 2) ver = idx.get_stable_ver(object);
		else if (args[2] == "stable") ver = idx.get_stable_ver(object);
		else if (args[2] == "unstable") ver = idx.get_unstable_ver(object);
		else if (!is_ver(args[2])) throw std::invalid_argument("wrong package version.");
		else ver = args[2];

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
				download_unzip_pac(x.name,x.ver,x.url);
				repo.update_install(x.name, x.ver, true);
			}
			message.first_sentence("csman: install", object + " " + ver + " and it's dependencies successfully.");
		}
		catch(std::exception &e) {
			throw e;
		}
	}

	void uninstall()
	{
		std::string ver;
		if (args.size() <= 2) ver = idx.get_stable_ver(object); //以下寻找包版本,均应替换为pac_repo的接口,例: pac_repo.get_current_runtime_ver()
		else if (args[2] == "stable") ver = idx.get_stable_ver(object);
		else if (args[2] == "unstable") ver = idx.get_unstable_ver(object);
		else if (!is_ver(args[2]))throw std::invalid_argument("wrong package version.");
		else ver = args[2];

		try {
			auto sup_set = idx.get_support_set(object, ver);
			message.first_sentence("csman:", "uninstalling " + object + " " + ver +
			                       " needs to uninstall these packages all because there are no other packages supported by them:");
			for(auto pac : sup_set) {
				if(idx.get_support_set(pac.name, pac.ver).size() != 0)
					sup_set.erase(pac);
				else
					message.content(pac.name + " " + pac.ver);
			}
			message.first_sentence("do you want to uninstall them all?", "[y/n]");

			if(!y_or_n()) { //否
				message.first_sentence("csman:", "operation interrupted by user.");
				return;
			}

			for(auto pac : sup_set) {
				message.content("uninstalling " + pac.name + " " + pac.ver + "...");
				delete_pac(pac.name, pac.ver);
				repo.update_uninstall(pac.name, pac.ver);
			}
			message.first_sentence("csman: uninstall", object + " " + ver + " and it's supports successfully.");
		}
		catch(std::exception &e) {
			throw e;
		}
	}

	void config()
	{
		if(object == "set") {
			if(args.size()==4) {
				try {
					cxt->set(args[3],args[4]);
				}
				catch(std::exception &e) {
					throw e;
				}
			}
			else throw std::invalid_argument("syntax error.");
		}
		else if(object == "show") {
			if(args.size()==2)
				cxt->show_all();
			else if(args.size()==3)
				cxt->show(args[2]);
			else throw std::invalid_argument("syntax error.");
		}
		else
			throw std::invalid_argument("syntax error.");
	}

	void list()
	{
		std::cout<<"Currently you have installed these packages:"<<std::endl;
		for(auto x : repo.local_pac) {
			std::cout<<x.first<<":"<<std::endl;
			for(auto v: x.second.ver) {
				std::cout<<v;
				if(v==x.second.available)
					std::cout<<"(current runtime using: CovScript "<<cxt->runtime_ver<<")";
				std::cout<<std::endl;
			}
		}
	}

	void checkout()
	{

	}
};

void REPL()
{
	while (true) {
		return;
	}
}

void cmd()
{

}

int main(int argc, char **argv)
{
	try {
		std::vector<std::string> args(&argv[1], argv + argc);

		context *cxt = new context();

		parser ps(cxt, args);

		ps.parse();

		return 0;
	}
	catch (std::exception &e) {
		std::cout << e.what();
	}
	return 0;
}