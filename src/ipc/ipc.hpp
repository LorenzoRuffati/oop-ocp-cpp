#ifndef IPC_H_DEF
#define IPC_H_DEF
#include <cstddef>
#include <vector>
#include <memory>

#include "src/utils/types.h"
using byte = unsigned char;


class IPC {
public:
    virtual size_t buff_size() =0;
    virtual std::vector<byte> receive(size_t max_read) =0;
    virtual int send(std::vector<byte>) =0;
    // For senders this will signal if we're ready to start sending, for 
    // receivers if there's data to be read
    virtual int ready() =0;
    // Cleanup the resources and destroy the class
    virtual ~IPC() =default;
};


class IPCFactory {
public:
    // Generate an instance IPC based on the desired method, role and parameters
    static std::unique_ptr<IPC> get_ipc(Method method, Role role, OptArgs& args);
};
#endif
