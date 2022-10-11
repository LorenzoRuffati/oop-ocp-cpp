#include "shm.hpp"

namespace SHM{
    size_t ShmSender::buff_size(){

    }

    std::vector<byte> ShmSender::receive(size_t max_read){

    }

    // Send the vector, might block, if the vector is empty it'll take it as
    // a sign that the copy process ended
    bool ShmSender::send(std::vector<byte>){

    }

    // For senders this will signal if we're ready to start sending, for 
    // receivers if there's data to be read
    bool ShmSender::ready(){

    }
    
    ShmSender::ShmSender(Method Method, Role role, OptArgs& args){

    }

    ShmSender::~ShmSender(){

    }

    size_t ShmReceiver::buff_size(){

    }

    std::vector<byte> ShmReceiver::receive(size_t max_read){

    }

    // Send the vector, might block, if the vector is empty it'll take it as
    // a sign that the copy process ended
    bool ShmReceiver::send(std::vector<byte>){

    }

    // For senders this will signal if we're ready to start sending, for 
    // receivers if there's data to be read
    bool ShmReceiver::ready(){

    }
    
    ShmReceiver::ShmReceiver(Method Method, Role role, OptArgs& args){

    }
    ShmReceiver::~ShmReceiver(){

    }
}