#ifndef IPC_H_DEF
#define IPC_H_DEF
#include <cstddef>
#include <vector>
#include <memory>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <errno.h>

#include "src/utils/types.hpp"
using byte = unsigned char;


class IPC {
public:
    virtual size_t buff_size() =0;
    virtual std::vector<byte> receive(size_t max_read) =0;
    // Send the vector, might block, if the vector is empty it'll take it as
    // a sign that the copy process ended
    virtual bool send(std::vector<byte>) =0;
    // For senders this will signal if we're ready to start sending, for 
    // receivers if there's data to be read
    virtual bool ready() =0;
    // Cleanup the resources and destroy the class
    virtual ~IPC() =default;
};


class IPCFactory {
public:
    // Generate an instance IPC based on the desired method, role and parameters
    static std::unique_ptr<IPC> get_ipc(Method method, Role role, OptArgs& args);
};

namespace MQ {
    class MQRead: public IPC{
        private:
            // Used to store message already read
            std::vector<byte> buffer;
            // If true the next read should read from buffer
            bool buffer_busy;
            // Becomes true when the writer sends an high priority message
            // stating that it finished sending. If it's true an empty queue
            // means the process received the whole file
            bool writer_finished;
            std::string queue_name;
            std::string lock_name;
            mqd_t mqd;
            size_t buffs;
        public:
            MQRead(Method Method, Role role, OptArgs& args);
            ~MQRead();

            size_t buff_size() final;

            /* Call after checking with ready(), will return a vector of bytes
            * Max read is equal to the return value of buff_size
            */
            std::vector<byte> receive(size_t max_read) final;

            // Not implemented for receiver
            bool send(std::vector<byte> payload) final;

            /* Returns true if messages are still expected, if false the reader 
            * process can shut down
            */
            bool ready() final;
    };

    class MQWrite: public IPC{
        private:
            std::string queue_name;
            mqd_t mqd;
            size_t buffs;
            bool finished;

        public:
            MQWrite(Method Method, Role role, OptArgs& args);

            size_t buff_size() final;

            std::vector<byte> receive(size_t max_read) final;

            bool send(std::vector<byte> payload) final;

            bool ready() final;
    };
}
#endif
