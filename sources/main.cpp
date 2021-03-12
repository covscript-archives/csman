//
// Created by Administrator on 2021/3/8.
//
#include <csman/command.hpp>

int main(int argc, char **argv)
{
	try {
		std::vector<std::string> args(&argv[1], argv + argc);

		csman::context_t cxt = csman::make_context();

		csman::parser ps(cxt.get(), args);

		ps.parse();
	}
	catch (const std::exception &e) {
		std::cout << e.what();
	}
	return 0;
}