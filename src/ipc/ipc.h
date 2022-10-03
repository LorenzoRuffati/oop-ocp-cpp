#include <cstddef>
#include <vector>

#include "src/utils/types.h"
using byte = unsigned char;


class IPC {
    virtual size_t buff_size();
    virtual std::vector<byte> receive(size_t max_read);
    virtual int send(std::vector<byte>);
    // For senders this will signal if we're ready to start sending, for 
    // receivers if there's data to be read
    virtual int ready();
    // Cleanup the resources and destroy the class
    virtual ~IPC();
};


class IPCFactory {
public:
    // Generate an instance IPC based on the desired method, role and parameters
    static IPC get_ipc(Method method, Role role, OptArgs args);
};
