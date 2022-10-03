#ifndef UTILS_H_DEF
#define UTILS_H_DEF
#include <iostream>
#include "src/utils/types.h"
class ParsedRes
{
private:
    /* data */
public:
    ParsedRes(int argc, char** argv);
    ~ParsedRes();

    Role role;
    Method method;
    OptArgs optargs;
};

ParsedRes::ParsedRes(int argc, char** argv)
{
    std::cout << "Parsing the arguments" << std::endl;
    role = Role::sender;
    method = Method::queue;
    optargs = (OptArgs) {};
}

ParsedRes::~ParsedRes(){
    std::cout << "Destroying parsed arguments" << std::endl;
}
#endif