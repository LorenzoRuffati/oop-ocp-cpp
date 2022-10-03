#ifndef ACTOR_H_DEF
#define ACTOR_H_DEF
#include "src/ipc/ipc.h"
#include "src/utils/types.h"

class Actor {
public:
    virtual int execute(IPC* ipc)=0;
    virtual ~Actor(){}
};

class ActorFactory
{
public:
    static Actor* create(Role role, OptArgs& args);
};
#endif