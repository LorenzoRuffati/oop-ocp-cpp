#include "common.hpp"
#include "src/utils/utils.hpp"

using namespace SHM;

class BaseSendTest: public ::testing::Test{
    protected:
        OptArgs args;
        std::string sem_path, shm_path;
        void SetUp() override{
            args.filename = "test";
            args.passwd = "shm_testing";
            sem_path = "/ocp.shm.sem." + args.passwd;
            shm_path = "/ocp.shm.mem." + args.passwd;
            args.readers = 1;
        }
};


class CleanBaseSend: public BaseSendTest{
    protected:
        void SetUp() override{
            BaseSendTest::SetUp();
            shm_unlink(shm_path.data());
            sem_unlink(sem_path.data());
        }
};

class ShmExists: public BaseSendTest{
    protected:
        void SetUp() override{
            BaseSendTest::SetUp();
            int fd_sh = shm_open(shm_path.data(), O_CREAT, 0660);
            close(fd_sh);
        }
};

class SemExistsSndr: public CleanBaseSend{
    protected:
        sem_t* sem;
        void SetUp() override{
            sem = sem_open(sem_path.data(), O_CREAT| O_RDWR, 0660);
        }

        void TearDown() override{
            sem_close(sem);
        }
};

class SenderCreated: public CleanBaseSend{
    protected:
        struct Coord* coord;
        char* buff;
        int fd_s;
        ShmSender* sndr;
        size_t width;

        void SetUp() override{
            sndr = new ShmSender(Method::shm, Role::sender, args);
            fd_s = shm_open((char*)shm_path.data(), O_RDWR, 0666);
            ASSERT_NE(fd_s, -1);

            coord = (struct Coord*) mmap(NULL, sizeof(struct Coord), 
                                        PROT_READ | PROT_WRITE, 
                                        MAP_SHARED, 
                                        fd_s, 0);
            ASSERT_NE(coord, (void*)-1);

            buff = (char*) mmap(NULL, 2* coord->width, 
                                PROT_READ | PROT_WRITE, 
                                MAP_SHARED, 
                                fd_s, coord->offset);
            ASSERT_NE(buff, (void*)-1);
            width = coord->width;
        }

        void TearDown() override {
            munmap(buff, 2*width);
            munmap(coord, sizeof(struct Coord));
            close(fd_s);
        }
};
/*
Tests to write:
+ Creation
    + [x] Memory map exists already
    + [ ] Semaphore pre-exists
+ Send
    + [ ] Wrapping around
    + [ ] Check locks after 
+ Destructor
    + [ ] Gets unlocked
    + [ ] Doesn't get stuck if there's no reader anymore already
*/

TEST_F(ShmExists, testfileexists){
    ShmSender* sndr;
    ASSERT_THROW(sndr = new ShmSender(Method::shm, Role::sender, args),
                FileError);
}

TEST_F(SemExistsSndr, test_creat){
    ShmSender* sndr = new ShmSender(Method::shm, Role::sender, args)
}