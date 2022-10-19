#include "common.hpp"

#include <random>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <vector>

std::vector<byte> write_random(size_t num){
    // First create an instance of an engine.
    std::random_device rnd_device;
    // Specify the engine and distribution.
    std::mt19937 mersenne_engine {rnd_device()};  // Generates random integers
    std::uniform_int_distribution<byte> dist {0, 255};
    
    auto gen = [&dist, &mersenne_engine](){
                   return dist(mersenne_engine);
               };

    std::vector<byte> vec(num);
    generate(begin(vec), end(vec), gen);
    return vec;
}

void test_rw_thread(std::promise<LockType> &&p, pthread_rwlock_t* l){
    //sleep(1);
    //std::cout << "Testing lock" << std::endl;
    int wrl = pthread_rwlock_trywrlock(l);
    if (wrl == 0){
        pthread_rwlock_unlock(l);
        p.set_value(LockType::Free);
        return;
    }
    int rdl = pthread_rwlock_tryrdlock(l);
    if (rdl == 0){
        pthread_rwlock_unlock(l);
        p.set_value(LockType::Read);
        return;
    }
    p.set_value(LockType::Write);
    return;
}

LockType test_rwlock(pthread_rwlock_t* lock){
    std::promise<LockType> p;
    auto f = p.get_future();
    std::thread t(&test_rw_thread, std::move(p), lock);
    t.join();
    return f.get();
}


void SetupCoord::SetUp(){
    args.filename = "/tmp/example";
    args.passwd = "shm_testing";
    sem_path = "/ocp.shm.sem." + args.passwd;
    shm_path = "/ocp.shm.mem." + args.passwd;

    fd_s = shm_open((char*)shm_path.data(), O_CREAT | O_RDWR, 0666);
    ASSERT_NE(fd_s, -1);
    
    size_t pg_sz = sysconf(_SC_PAGE_SIZE);
    size_t cd_sz = sizeof(struct Coord);
    size_t n = cd_sz/pg_sz;
    size_t off = pg_sz*(1+n);

    ftruncate(fd_s, off + 2*1024);
    coord = (struct Coord*) mmap(NULL, sizeof(struct Coord), 
                                PROT_READ | PROT_WRITE, 
                                MAP_SHARED, 
                                fd_s, 0);
    std::cout << static_cast<void*>(coord) << std::endl;
    if (coord == (void*)-1){
        perror("Coordination mmap");
    }
    buff = (char*) mmap(NULL, 2*1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd_s, off);
    std::cout << static_cast<void*>(buff) << std::endl;
    if (buff == (char*)-1){
        perror("Buffer mmap");
    }

    coord->offset = off;
    coord->width = 1024;
    condvar_init(&(coord->rdr_cnt));
    init_rwlock(&(coord->lcs_active[0]));
    init_rwlock(&(coord->lcs_active[1]));
    init_rwlock(&(coord->lcs_leaving[0]));
    init_rwlock(&(coord->lcs_leaving[1]));
    sem_init(&(coord->copy_sem), 1, 1);
    coord->act_size[0]=0;
    coord->act_size[1]=0;
    pthread_rwlock_wrlock(&(coord->lcs_active[0]));
}

void SemExists::SetUp(){
    SetupCoord::SetUp();
    sem_coord = sem_open((char*) sem_path.data(), O_CREAT|O_EXCL|O_RDWR, 0666);
}

void SemUnlocked::SetUp(){
    SemExists::SetUp();
    sem_post(sem_coord);
}

void ReaderJoined::SetUp(){
    SemUnlocked::SetUp();
    recv = new ShmReceiver(Method::shm, Role::receiver, args);

}

void SetupCoord::TearDown(){
    std::cout << "Teardown" << sem_path << " " << shm_path << std::endl;

    sem_unlink((char*)sem_path.data());
    close(fd_s);
    munmap(buff, 2*coord->width);
    munmap(coord, sizeof(struct Coord));
    shm_unlink((char*)shm_path.data());

}

void SemExists::TearDown(){
    sem_close(sem_coord);
    SetupCoord::TearDown();
}