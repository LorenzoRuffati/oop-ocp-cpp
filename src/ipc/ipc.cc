#include <iostream>
#include "ipc.h"
#include "src/utils/utils.h"



class MockIPC: public IPC{
    public:
        MockIPC(OptArgs& args):
            reads(3){
            std::cout << "Creating IPC" << std::endl;
        }

        size_t buff_size() final{
            return 100;
        }

        std::vector<byte> receive(size_t max_read) final{
            throw NotImplemented();
        }
        
        int send(std::vector<byte> payload) final{
            std::cout << "Sending a vector " << payload.size() << " long" << std::endl;
            for (byte i: payload) {
                std::cout << i;
            }
            std::cout << std::endl;
            reads -= 1;
            return payload.size();
        }

        int ready() final{
            return reads>0;
        }
    
    private:
        int reads;

};


IPC* IPCFactory::get_ipc(Method method, Role role, OptArgs& args){
    return new MockIPC(args);

}