#ifndef ACTOR_H_DEF
#define ACTOR_H_DEF
#include <memory>
#include "src/ipc/ipc.hpp"
#include "src/utils/types.hpp"

class Actor {
public:
    virtual int execute(IPC& ipc)=0;
    virtual ~Actor() =default;
};

class ActorFactory
{
public:
    static std::unique_ptr<Actor> create(Role role, OptArgs& args);
};
#endif