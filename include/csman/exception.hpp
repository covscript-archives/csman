#pragma once
/*
 * CovScript Manager: CLI Parser
 * Licensed under Apache 2.0
 * Copyright (C) 2020-2021 Chengdu Covariant Technologies Co., LTD.
 * Website: https://covariant.cn/
 * Github:  https://github.com/chengdu-zhirui/
 */
#include <cstring>

namespace csman {
    class exception {
    public:
        std::string type = "default", what = "", where = "", when = "";

        exception() = delete;

        exception(const std::string &what)
                : type(type), what(what) {};

        exception(const std::string &what, const std::string &where)
                : type(type), what(what), where(where) {};

        exception(const std::string &what, const std::string &where, const std::string &when)
                : type(type), what(what), where(where), when(when) {};

        ~exception() = default;
    };
    class logic_err : public exception{
    };
    class runtime_err : public exception{

    };
    class net_err : public exception{

    };
    class time_err : public exception{

    };
    class user_arg : public exception{

    };
    class data_arg : public exception{
    };

}