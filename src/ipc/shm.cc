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
        return coord->width;
    }

    std::vector<byte> ShmSender::receive(size_t max_read){
        throw NotImplemented();
    }

    // Send the vector, might block, if the vector is empty it'll take it as
    // a sign that the copy process ended
    bool ShmSender::send(std::vector<byte> src){
        if (!first_send){
            pthread_rwlock_wrlock(&(coord->lcs_active[idx]));
            pthread_rwlock_unlock(&(coord->lcs_leaving[!idx]));
        }
        std::copy(src.begin(), src.end(), get_ptr_sector(buff, coord->width, idx));
        coord->act_size[idx] = src.size();
        pthread_rwlock_wrlock(&(coord->lcs_leaving[idx]));
        idx = !idx;
        pthread_rwlock_unlock(&(coord->lcs_active[!idx]));
        first_send = false;
        return src.size()>0;
    }

    // For senders this will signal if we're ready to start sending, for 
    // receivers if there's data to be read
    bool ShmSender::ready(){
        throw NotImplemented();
    }
    
    ShmSender::ShmSender(Method Method, Role role, OptArgs& args):
        idx(0), first_send(true){
        
        std::cout << "Passwd: " << args.passwd <<std::endl
                  << "Width: " << args.width << std::endl
                  << "Readers: " << args.readers <<std::endl;

        std::string path_sem = "/ocp.shm.sem." + args.passwd;
        path_shm = "/ocp.shm.mem." + args.passwd;
        fds = shm_open(path_shm.data(), O_CREAT | O_EXCL | O_RDWR, 0660);
        if (fds == -1){
            throw FileError();
        }

        size_t pg_sz = sysconf(_SC_PAGE_SIZE);
        size_t cd_sz = sizeof(struct Coord);
        size_t n = cd_sz/pg_sz;
        size_t off = pg_sz*(1+n);

        ftruncate(fds, off + 2*args.width);

        coord = (struct Coord*) mmap(NULL, sizeof(struct Coord), 
                                    PROT_READ | PROT_WRITE, 
                                    MAP_SHARED, 
                                    fds, 0);
        if (coord == (void*)-1){
            perror("Error in mmapping coord");
            throw OwnError();
        }
        buff = (char*) mmap(NULL, 2*args.width, PROT_READ | PROT_WRITE, MAP_SHARED, fds, off);
        if (buff == (char*)-1){
            perror("Error in mmapping buff");
            throw OwnError();
        }
        
        coord->offset = off;
        coord->width = args.width;
        condvar_init(&(coord->rdr_cnt));
        init_rwlock(&(coord->lcs_active[0]));
        init_rwlock(&(coord->lcs_active[1]));
        init_rwlock(&(coord->lcs_leaving[0]));
        init_rwlock(&(coord->lcs_leaving[1]));
        sem_init(&(coord->copy_sem), 1, args.readers);
        coord->act_size[0]=0;
        coord->act_size[1]=0;
        pthread_rwlock_wrlock(&(coord->lcs_active[0]));

        auto sem_coord = sem_open((char*) path_sem.data(), O_CREAT|O_RDWR, 0666, 0);
        sem_post(sem_coord);
        coord->rdr_cnt.wait_until_RETLOCK(args.readers);
        pthread_mutex_unlock(&(coord->rdr_cnt.lock));
        sem_close(sem_coord);
        sem_unlink(path_sem.data());
    }

    ShmSender::~ShmSender(){
        if (first_send){
            pthread_rwlock_unlock(&(coord->lcs_active[0]));
        } else {
            pthread_rwlock_unlock(&(coord->lcs_leaving[!idx]));
        }
        coord->rdr_cnt.wait_until_RETLOCK(0);
        munmap(buff, coord->width * 2);
        munmap(coord, sizeof(struct Coord));
        close(fds);
        shm_unlink(path_shm.data());
    }

    size_t ShmReceiver::buff_size(){
        return coord->width;
    }

    std::vector<byte> ShmReceiver::receive(size_t max_read){
        // Precondition: call after ready, holds lock on active[idx] (unleass read_flag is set)
        if (finished || read_chunk){
            //std::cout << "Error in flags" << std::endl;
            throw OwnError();
        }
        std::vector<byte> vec;
        vec.reserve(coord->act_size[idx]);
        //std::cout << "Reserved" << std::endl;
        std::copy_n(get_ptr_sector(buff, coord->width, idx), coord->act_size[idx], std::back_inserter(vec));
        //std::cout << "Copied" << std::endl;
        read_chunk = true;
        pthread_rwlock_rdlock(&(coord->lcs_leaving[idx]));
        pthread_rwlock_unlock(&(coord->lcs_active[idx]));
        //std::cout << "Unlocked" << std::endl;
        return vec;
    }

    // Send the vector, might block, if the vector is empty it'll take it as
    // a sign that the copy process ended
    bool ShmReceiver::send(std::vector<byte>){
        throw NotImplemented();
    }

    // For senders this will signal if we're ready to start sending, for 
    // receivers if there's data to be read
    bool ShmReceiver::ready(){
        if (finished){ return false; }
        if (!read_chunk) { return true; }
        // idx 
        idx = !idx;
        pthread_rwlock_rdlock(&(coord->lcs_active[idx]));
        pthread_rwlock_unlock(&(coord->lcs_leaving[!idx]));
        read_chunk = false;
        size_t size = coord->act_size[idx];
        finished = (size == 0);
        if (finished){
            pthread_rwlock_unlock(&(coord->lcs_active[idx]));
        }
        return size > 0;
    }
    
    ShmReceiver::ShmReceiver(Method Method, Role role, OptArgs& args):
        idx(1), finished(false), read_chunk(true), buff(nullptr), coord(nullptr){
        
        std::string path_sem = "/ocp.shm.sem." + args.passwd;
        sem_t* sem_coord = sem_open((char*) path_sem.data(), O_CREAT | O_RDWR, 0660, 0);
        if (sem_coord == NULL) {std::cout << "error semaphore" << std::endl; throw FileError();}

        int r = sem_wait(sem_coord);
        if (r!=0){throw OwnError();}
        std::cout << "Semaphore unlocked" << std::endl;
        sem_post(sem_coord);
        sem_close(sem_coord);
        //std::cout<< "Passed the semaphore" << std::endl;
        
        std::string path_mem = "/ocp.shm.mem." + args.passwd;
        fds = shm_open((char*)path_mem.data(), O_RDWR, 0);
        if (fds < 0){ throw FileError();}
        coord = (struct Coord*) mmap(NULL, sizeof(struct Coord), 
                                    PROT_READ | PROT_WRITE, 
                                    MAP_SHARED, 
                                    fds, 0);
        if (coord == NULL){coord = nullptr; throw FileError();}
        //std::cout<< "Mapped coordination" << std::endl;

        buff = (char*) mmap(NULL, 2*coord->width, PROT_READ | PROT_WRITE, MAP_SHARED, fds, coord->offset);
        if (buff == NULL){buff = nullptr; throw FileError();}
        //std::cout<< "Mapped buffer" << std::endl;

        int try_cp = sem_trywait(&(coord->copy_sem));
        if (try_cp != 0){ throw FileError();}
        
        int rl = pthread_rwlock_rdlock(&(coord->lcs_leaving[1]));
        //std::cout<< "Locked leaving 1: " << rl << " " << errno << std::endl;
        /*std::cout << "Reader setup: " << std::endl
                  << "\t" << static_cast<void*>(coord) << std::endl
                  << "\t" << static_cast<void*>(buff) << std::endl
                  << "\t" << idx << std::endl
                  << "\t" << fds << std::endl;*/

        coord->rdr_cnt.change(1);
        //std::cout << "Increased reader count";
    }

    ShmReceiver::~ShmReceiver(){
        if (coord!= nullptr && buff != nullptr){
            if (!finished){
                if (read_chunk){
                    pthread_rwlock_unlock(&(coord->lcs_leaving[idx]));
                } else {
                    pthread_rwlock_unlock(&(coord->lcs_active[idx]));
                }
            }
            coord->rdr_cnt.change(-1);
            //std::cout << " Decreased reader count";
            munmap(buff, 2*coord->width);
            munmap(coord, sizeof(struct Coord));
            close(fds);
        } else if (coord != nullptr) {
            munmap(coord, sizeof(struct Coord));
            close(fds);
        }
    }

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