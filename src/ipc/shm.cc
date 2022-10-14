#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "shm.hpp"
#include "src/utils/utils.hpp"

namespace SHM{
    char* get_ptr_sector(char* base, size_t width, int idx){
        return base + (width*idx);
    }
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
    void UNSAFE_write_to_chunk(struct Coord* coord, int idx, std::vector<byte> payload, char* buff){
        char* beg = buff + (idx * coord->width);
        std::copy(payload.begin(), payload.end(), beg);
        coord->act_size[idx] = payload.size();
    }

    void condvar_init(CondVar* var){
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
        pthread_mutex_init(&(var->lock), &attr);
        var->var = 0;
        pthread_condattr_t cattr;
        pthread_condattr_init(&cattr);
        pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED);
        pthread_cond_init(&(var->cond), &cattr);
    }

    void init_rwlock(pthread_rwlock_t* lock){
        pthread_rwlockattr_t attr;
        pthread_rwlockattr_init(&attr);
        pthread_rwlockattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
        pthread_rwlock_init(lock, &attr);
    }
}