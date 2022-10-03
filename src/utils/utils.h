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


class NotImplemented : public std::exception {
    public:
        char * what ();
};
#endif