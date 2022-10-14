#include "types.hpp"


std::string role_repr(Role r){
    switch (r)
    {
    case Role::receiver:
        return "RECEIVER";
        break;
    case Role::sender:
        return "SENDER";
        break;
    
    default:
        break;
    }
}

std::string method_repr(Method m){
    switch (m)
    {
    case Method::shm:
        return "SHARED";
        break;
    case Method::queue:
        return "QUEUE";
        break;
    case Method::fifo:
        return "FIFO";
        break;
    case Method::socket:
        return "SOCKET";
        break;
    
    default:
        break;
    }
}