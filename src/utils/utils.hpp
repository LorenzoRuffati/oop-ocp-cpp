#ifndef UTILS_H_DEF
#define UTILS_H_DEF
#include <iostream>
#include "src/utils/types.hpp"
#include <boost/program_options.hpp>

class ParsedRes
{
private:
    /* data */
public:
    ParsedRes(int argc, char** argv);

    Role role;
    Method method;
    OptArgs optargs;
    bool valid;
};


class NotImplemented : public std::exception {
    public:
        char * what ();
};
#endif