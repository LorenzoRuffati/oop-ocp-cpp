#include "src/utils/utils.h"

ParsedRes::ParsedRes(int argc, char** argv)
{
    std::cout << "Parsing the arguments" << std::endl;
    role = Role::sender;
    method = Method::queue;
    optargs = (OptArgs) {};
}

char * NotImplemented::what () {
    return "Not implemented";
}