#include <iostream>
#include <fstream>
#include <iostream>
#include <algorithm>
#include "actor.hpp"


class Sender: public Actor {
    private:
        std::ifstream in_stream;
    public:
        Sender(OptArgs& args):
            in_stream(args.filename, std::ios::in | std::ios::binary)
            {
            std::cout << "Creating sender" << std::endl;
        }

        int execute(IPC& ipc) final{
            size_t buff_s = ipc.buff_size();
            std::vector<byte> buff{};
            buff.reserve(buff_s);

            bool finished = false;
            std::vector<byte> vectr;
            vectr.reserve(buff_s);

            while (!finished){
                in_stream.read((char*)buff.data(), buff_s);
                std::copy_n(buff.data(), in_stream.gcount(), std::back_inserter(vectr));
                if (in_stream){
                    std::cout << "Read full\n";
                    // Read all characters
                } else if (in_stream.eof()){
                    // Read only some bytes
                    std::cout << "Read some "<< "\n";
                    finished = true;
                } else {
                    throw std::exception();
                }
                std::cout << vectr.size() << std::endl;
                ipc.send(vectr);
                vectr.clear();
            }
            ipc.send(buff); // Sending an empty buffer to signal that the process ended
            return 0;        
        }
};

class Receiver: public Actor {
    private:
        std::ofstream out_stream;
    public:
        Receiver(OptArgs& args):
            out_stream(args.filename, std::ios::out | std::ios::trunc | std::ios::binary){
        }

        int execute(IPC& ipc) final {
            // 1. Open the file in write mode, truncating it
            // 2.
            size_t buff_size = ipc.buff_size();
            // 3
            std::vector<byte> bytes;
            while (ipc.ready()){
                bytes = ipc.receive(buff_size);
                //out_stream << bytes.;
            }
        }
};

std::unique_ptr<Actor> ActorFactory::create(Role role, OptArgs& args){
    return std::unique_ptr<Actor>(new Sender(args));
}