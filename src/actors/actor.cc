#include <iostream>
#include "actor.h"


class Sender: public Actor {
public:
    Sender(OptArgs& args){
        std::cout << "Creating sender" << std::endl;
    }

    int execute(IPC* ipc){
        std::cout << "Executing sender" << std::endl;
        size_t buff_s = ipc->buff_size();
        std::vector<byte> exvec{'a','b','c'};
        while (ipc->ready()){
            ipc->send(exvec);
        }
        return 0;        
    }
    ~Sender(){
        std::cout << "Destroying sender" << std::endl;
    }
};

Actor* ActorFactory::create(Role role, OptArgs& args){
    return new Sender(args);
}