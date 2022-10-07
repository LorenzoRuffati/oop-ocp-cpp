#include <iostream>
#include "ipc.hpp"
#include "src/utils/utils.hpp"



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
        
        bool send(std::vector<byte> payload) final{
            std::cout << "Sending a vector " << payload.size() << " long" << std::endl;
            for (char i: payload) {
                std::cout << i <<'.';
            }
            std::cout << std::endl;
            return payload.size();
        }

        bool ready() final{
            return reads>0;
        }
    
    private:
        int reads;

};

MQRead::MQRead(Method Method, Role role, OptArgs& args):
    buffer_busy(false),
    writer_finished(false)
    {
    std::cout << "Create or open queue in read-only mode" << std::endl
                << "Create a lock file to assert the reader is already here" << std::endl;
}

MQRead::~MQRead(){
    std::cout << "Delete queue and lockfile" << std::endl;
}

size_t MQRead::buff_size(){
    std::cout << "Query the message queue for the buffer size" << std::endl;
    return 100;
}

/* Call after checking with ready(), will return a vector of bytes
    * Max read is equal to the return value of buff_size
    */
std::vector<byte> MQRead::receive(size_t max_read){
    if (!writer_finished){
        std::cout << "I can assume this is called when ready()" << std::endl
                << "If the buffer is full return the buffer" << std::endl
                << "If not read from the queue" << std::endl
                << "\t`writer_finished`: assign flag and read again" << std::endl
                << "\treturn" << std::endl;
        writer_finished = true;
    }
    return std::vector<byte>{'a','b'};
}

// Not implemented for receiver
bool MQRead::send(std::vector<byte> payload){
    throw NotImplemented();
}

/* Returns true if messages are still expected, if false the reader 
    * process can shut down
    */
bool MQRead::ready(){
    std::cout << "Check if there's content in the buffer, if so return true" << std::endl
                << "If not check how many messages" << std::endl
                << "\t0: if `writer_finished` return false, else true" << std::endl
                << "\t1: if `writer_finished` return true, else read the"<< std::endl 
                << "\t   message, if it's a `writer_finished` message" << std::endl 
                << "\t   return false, else true" << std::endl
                << "\t2+: return true" << std::endl;
    if (writer_finished){
        return false;
    }
    return true;
}

MQWrite::MQWrite(Method Method, Role role, OptArgs& args){
    std::cout << "Create or access the queue, " << std::endl
                << "If the queue is not empty fail" << std::endl;
}

size_t MQWrite::buff_size(){
    std::cout << "Query the message queue for the buffer size" << std::endl;
    return 100;
}

std::vector<byte> MQWrite::receive(size_t max_read){
    throw NotImplemented();
}

bool MQWrite::send(std::vector<byte> payload){
    if (payload.size() == 0){
        std::cout << "Signal the queue that sending process is finished" << std::endl;
    } else {
        std::cout << "Blocking write on the queue" << std::endl;
    }
}

bool MQWrite::ready(){
    std::cout << "Return true (possibly implement checks on the message queue health" << std::endl;
}

std::unique_ptr<IPC> IPCFactory::get_ipc(Method method, Role role, OptArgs& args){
    if (method == Method::queue){
        switch (role){
        case Role::receiver :
            return std::unique_ptr<IPC>(new MQRead(method, role, args));
            break;
        case Role::sender :
            return std::unique_ptr<IPC>(new MQWrite(method, role, args));
            break;
        default:
            break;
        }
    }
    return std::unique_ptr<IPC>(new MockIPC(args));
}