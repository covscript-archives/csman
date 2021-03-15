//
// Created by Administrator on 2021/3/8.
//
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