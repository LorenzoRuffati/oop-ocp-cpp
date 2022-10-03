#include <iostream>
#include "ipc.h"
#include "src/utils/utils.h"



class MockIPC: public IPC{
    public:
        MockIPC(OptArgs& args):
            reads(3){
            std::cout << "Creating IPC" << std::endl;
        }

        size_t buff_size() override{
            return 100;
        }

        std::vector<byte> receive(size_t max_read) override{
            throw NotImplemented();
        }
        
        int send(std::vector<byte> payload) override{
            std::cout << "Sending a vector " << payload.size() << " long" << std::endl;
            for (byte i: payload) {
                std::cout << i;
            }
            std::cout << std::endl;
            reads -= 1;
            return payload.size();
        }

        int ready() override{
            return reads>0;
        }
        // Cleanup the resources and destroy the class
        ~MockIPC() override{
            std::cout << "Destroying IPC interface" << std::endl;
        };
    
    private:
        int reads;

};


IPC* IPCFactory::get_ipc(Method method, Role role, OptArgs& args){
    return new MockIPC(args);

}