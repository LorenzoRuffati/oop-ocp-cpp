#ifndef TYPES_H_DEF
#define TYPES_H_DEF
enum class Role {
    sender,
    receiver,
};

enum class Method {
    queue,
    fifo,
    shm,
    socket,
};

typedef struct {
    std::string filename;
    //    
} OptArgs;
#endif