#include <iostream>
#include "ipc.hpp"
#include "shm.hpp"
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

namespace MQ { // Handles queue IPC method
    MQRead::MQRead(Method Method, Role role, OptArgs& args):
        buffer(2048),
        buffer_busy(false),
        writer_finished(false),
        buffs(2048){
        queue_name = "/" + args.passwd;
        lock_name = "/tmp/ocp.mq." + args.passwd + ".lock";

        auto op_l = open(&lock_name[0], O_CREAT | O_EXCL, 0660);
        if (op_l == -1){
            if (errno == EEXIST){
                std::cout << lock_name << " exists, if the previous process crashed" 
                << "run the cleaner" << std::endl;
            }
            throw FileError();
        }
        close(op_l); // I don't need it open

        struct mq_attr attr;
        attr.mq_msgsize = 2048;
        attr.mq_flags = 0;
        attr.mq_maxmsg = 5;
        mqd = mq_open(&queue_name[0], O_CREAT | O_RDONLY, 0660, &attr);

        if (mqd == (mqd_t) -1){
            throw FileError();
        }
    }

    MQRead::~MQRead(){
        unlink(&lock_name[0]);
        mq_close(mqd);
        mq_unlink(&queue_name[0]);
    }

    size_t MQRead::buff_size(){
        struct mq_attr attr;
        auto r = mq_getattr(mqd, &attr);
        if (r!= 0){ throw OwnError();}
        buffs = attr.mq_msgsize;
        return buffs;
    }

    /* Call after checking with ready(), will return a vector of bytes
        * Max read is equal to the return value of buff_size
        */
    std::vector<byte> MQRead::receive(size_t max_read){
        if (buffer_busy){
            std::vector<byte> retv = buffer;
            buffer_busy = false;
            return retv;
        }
        std::vector<byte> scratch, retvec;
        scratch.reserve(buffs);
        retvec.reserve(buffs);
        unsigned int prio;
        auto r = mq_receive(mqd, (char*)scratch.data(), buffs, &prio);
        if (r==-1){throw OwnError();}
        if (prio != 0){
            writer_finished = true;
            r = mq_receive(mqd, (char*)scratch.data(), buffs, &prio);
            if (r==-1){throw OwnError();}
            if (prio != 0){ throw OwnError();}
        }
        std::copy_n(scratch.data(), r, std::back_inserter(retvec));
        return retvec;
    }

    // Not implemented for receiver
    bool MQRead::send(std::vector<byte> payload){
        throw NotImplemented();
    }

    /* Returns true if messages are still expected, if false the reader 
        * process can shut down
        */
    bool MQRead::ready(){
        if (buffer_busy){
            return true;
        }
        struct mq_attr attr;
        auto r_at = mq_getattr(mqd, &attr);
        if (r_at!= 0){ throw OwnError();}

        if (attr.mq_curmsgs >= 2){
            return true;
        }

        if (writer_finished){
            return attr.mq_curmsgs > 0;
        }

        // Here I can assume exactly one message and writer still active as far as I know
        std::vector<byte> scratch(buffs);
        unsigned int prio;
        auto rr = mq_receive(mqd, (char*)scratch.data(), buffs, &prio);
        if (rr==-1){throw OwnError();}
        if (prio != 0){
            writer_finished = true;
            return false;
        } else {
            buffer.clear();
            std::copy_n(scratch.data(), rr, std::back_inserter(buffer));
            buffer_busy = true;
            return true;
        }
    }

    MQWrite::MQWrite(Method Method, Role role, OptArgs& args): finished(false){
        queue_name = "/" + args.passwd;

        struct mq_attr attr;
        attr.mq_msgsize = 2048;
        attr.mq_flags = 0;
        attr.mq_maxmsg = 5;
        mqd = mq_open(&queue_name[0], O_CREAT | O_WRONLY, 0660, &attr);

        if (mqd == (mqd_t) -1){
            throw FileError();
        }
        auto r = mq_getattr(mqd, &attr);
        if (r!=0){
            throw OwnError();
        }
        if (attr.mq_curmsgs != 0){
            throw FileError();
        }
    }

    size_t MQWrite::buff_size(){
        struct mq_attr attr;
        auto r = mq_getattr(mqd, &attr);
        if (r!=0){
            throw OwnError();
        }
        buffs = attr.mq_msgsize;
        return buffs;
    }

    std::vector<byte> MQWrite::receive(size_t max_read){
        throw NotImplemented();
    }

    bool MQWrite::send(std::vector<byte> payload){
        if (not ready()){
            return false;
        }
        unsigned int prio = 0;
        if (payload.size() == 0){
            prio = 1;
            finished = true;
        }
        int r = mq_send(mqd, (char*)payload.data(), payload.size(), prio);
        return r == 0;
    }

    bool MQWrite::ready(){
        return !finished;
    }
}

using namespace MQ;
using namespace SHM;

std::unique_ptr<IPC> IPCFactory::get_ipc(Method method, Role role, OptArgs& args){
    std::cout << "IPC factory" << std::endl;
    std::cout << method_repr(method) << " " << role_repr(role) << std::endl;
    switch (method){
        case Method::queue:
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
            break;
        case Method::shm:
            switch (role){
                case Role::receiver:
                    std::cout << "IPC factory" << std::endl;
                    return std::unique_ptr<IPC>(new SHM::ShmReceiver(method, role, args));
                    break;
                case Role::sender:
                    std::cout << "Creating shm sender" << std::endl;
                    return std::unique_ptr<IPC>(new SHM::ShmSender(method, role, args));
                    break;
                default:
                    break;
            }
            break;
        default:
            throw NotImplemented();
            break;
    }
}