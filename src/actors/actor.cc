#include <iostream>
#include "actor.hpp"


class Sender: public Actor {
public:
    Sender(OptArgs& args){
        std::cout << "Creating sender" << std::endl;
    }

    int execute(IPC& ipc) final{
        std::cout << "Executing sender" << std::endl;
        size_t buff_s = ipc.buff_size();
        std::vector<byte> exvec{'a','b','c'};
        while (ipc.ready()){
            ipc.send(exvec);
        }
        return 0;        
    }
};

std::unique_ptr<Actor> ActorFactory::create(Role role, OptArgs& args){
    return std::unique_ptr<Actor>(new Sender(args));
}