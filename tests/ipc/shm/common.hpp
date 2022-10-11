#ifndef TEST_HEADER_SHM
#define TEST_HEADER_SHM
#include <gtest/gtest.h>
#include <algorithm>
#include <thread>
#include <future>

#include "src/ipc/ipc.hpp"
#include "src/ipc/shm.hpp"


using namespace MQ;
using namespace SHM;

enum class LockType{
    Free, Read, Write
};

LockType test_rwlock(pthread_rwlock_t* lock);
LockType test_mutex(pthread_mutex_t* lock); // https://stackoverflow.com/questions/7686939/c-simple-return-value-from-stdthread


class SetupCoord: public ::testing::Test {
    protected:
        OptArgs args;
        struct Coord* coord;
        char* buff;
        std::string sem_path, shm_path;
        int fd_s;

                
        // Creates the coordination memory, inits the copy semaphore to 1
        // and locks the active lock on the first segment
        void SetUp() override;
        
        // Unlinks the semaphore and the shared_mem_files
        void TearDown() override;
};

class SemExists: public SetupCoord {
    protected:
        sem_t* sem_coord;

        // Creates the semaphore
        void SetUp() override;
        
        // Closes the semaphore file
        void TearDown() override;
};

class SemUnlocked: public SemExists{
    protected:
        // Frees the semaphore
        void SetUp() override;
};

class ReaderJoined: public SemUnlocked {
    protected:
        ShmReceiver* recv;
        // The reader has been created
        void SetUp() override;
};

#endif