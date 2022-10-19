#ifndef UTILS_H_DEF
#define UTILS_H_DEF
#include <iostream>
#include <errno.h>
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


class OwnError : public std::exception {};

class NotImplemented : public OwnError {
    public:
        char * what ();
};

class FileError: public OwnError {
};

#endif