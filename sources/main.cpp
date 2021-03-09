//
// Created by Administrator on 2021/3/8.
//
#include <csman/command.hpp>

void csman_main(csman::context* cxt, int argc, char* argv[])
{
    using namespace csman;
    try {

        std::vector<std::string> args(&argv[1], argv + argc);

        parser ps(cxt, args);
//
//		ps.parse();
    }
    catch (std::exception &e) {
        std::cout << e.what();
    }
}

int main(int argc, char **argv)
{
    using namespace csman;
    context cxt;
    csman_main(&cxt, argc, argv);
	return 0;
}