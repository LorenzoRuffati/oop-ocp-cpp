#ifndef IPC_SHM_H_DEF
#define IPC_SHM_H_DEF
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "ipc.hpp"

namespace SHM{
    class ShmSender: public IPC {
        private:
            struct Coord* coord;
            char* buff;
            bool first_send;
            int idx;
            int fds;
            std::string path_shm;
        public:
            size_t buff_size() final;
            std::vector<byte> receive(size_t max_read) final;
            // Send the vector, might block, if the vector is empty it'll take it as
            // a sign that the copy process ended
            bool send(std::vector<byte>) final;
            // For senders this will signal if we're ready to start sending, for 
            // receivers if there's data to be read
            bool ready() final;
            
            ShmSender(Method Method, Role role, OptArgs& args);
            ~ShmSender();
    };

    class ShmReceiver: public IPC {
        private:
            struct Coord* coord;
            char* buff;
            bool finished, read_chunk;
            int idx;
            int fds;
        public:
            size_t buff_size() final;
            std::vector<byte> receive(size_t max_read) final;
            // Send the vector, might block, if the vector is empty it'll take it as
            // a sign that the copy process ended
            bool send(std::vector<byte>) final;
            // For senders this will signal if we're ready to start sending, for 
            // receivers if there's data to be read
            bool ready() final;
            
            ShmReceiver(Method Method, Role role, OptArgs& args);
            ~ShmReceiver();
    };

    struct CondVar{
        pthread_mutex_t lock;
        int var;
        pthread_cond_t cond;
        
        inline void change(int change){
            pthread_mutex_lock(&lock);
            var = var + change;
            pthread_cond_signal(&cond);
            pthread_mutex_unlock(&lock);
        }
        inline void wait_until_RETLOCK(int targ){
            pthread_mutex_lock(&lock);
            while (var != targ){
                pthread_cond_wait(&cond, &lock);
            }
        }
    };
    void condvar_init(CondVar*);
    void init_rwlock(pthread_rwlock_t*);

    struct Coord {
        size_t offset;
        size_t width;
        sem_t copy_sem;
        struct CondVar rdr_cnt;
        pthread_rwlock_t lcs_active[2];
        pthread_rwlock_t lcs_leaving[2];
        size_t act_size[2];
    };

    void UNSAFE_write_to_chunk(struct Coord* coord, int idx, std::vector<byte> payload, char* buff);
}

#endif
