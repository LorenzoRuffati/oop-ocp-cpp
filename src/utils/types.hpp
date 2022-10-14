#ifndef TYPES_H_DEF
#define TYPES_H_DEF
#include <string>
enum class Role {
    sender,
    receiver,
};

std::string role_repr(Role);

enum class Method {
    queue,
    fifo,
    shm,
    socket,
    undefined,
};

std::string method_repr(Method);

typedef struct {
    std::string filename;
    std::string passwd;
    int readers;
    size_t width;
    //    
} OptArgs;
#endif