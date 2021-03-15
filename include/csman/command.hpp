#pragma once
/*
 * CovScript Manager: CLI Parser
 * Licensed under Apache 2.0
 * Copyright (C) 2020-2021 Chengdu Covariant Technologies Co., LTD.
 * Website: https://covariant.cn/
 * Github:  https://github.com/chengdu-zhirui/
 */
#include <csman/global.hpp>
#include <csman/pac_repo.hpp>
#include <csman/idx_file.hpp>
#include <zip.hpp>
#include <iostream>

namespace csman {
    class parser {
    private:
        context *cxt = nullptr;   // 上下文，存取全局变量与信息
        idx_file idx;   // sources_idx，可下载包信息，负责依赖查询，支持查询
        pac_repo repo;  // pac_repo，包仓库
        std::vector<std::string> args;  // 用户命令的参数，例如install xxx
        set_t<std::string> opt;  // 从args里分离出的可选参数，例如-r
        std::string predicate, object;  // 谓语，宾语

        void download_unzip_pac(const std::string &name, const std::string &ver, const std::string &url);
        void delete_pac(std::string &name, std::string &ver);

        const std::string HELP = "\nusage: csman <command> [objects] [args]\n"
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
                                 "Go to our page for more support: http://csman.info\n";

        const std::string VERSION = "csman 1.0";

        bool y_or_n();
        /*从args分离opt的filter, 分离后args将不带有opt参数*/
        void opt_filter();
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
        parser(context *, const std::vector<std::string> &);
        void parse();
        void install();
        void uninstall();
        void config();
        void list();
        void checkout();
    };
    void REPL();
    void cmd();
}