/*
 * CovScript Manager: OS Depended Functions Implementation
 * Licensed under Apache 2.0
 * Copyright (C) 2020-2021 Chengdu Covariant Technologies Co., LTD.
 * Website: https://covariant.cn/
 * Github:  https://github.com/chengdu-zhirui/
 */
#include <csman/command.hpp>

int main(int argc, char **argv)
{
	try {
		std::vector<std::string> args(&argv[1], argv + argc);



		csman::context *cxt = new csman::context();


		csman::parser ps(cxt, args);

		ps.parse();
	}
	catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}