#include "common.hpp"
#include "src/utils/utils.hpp"

using namespace SHM;

void delay_post(std::string path, int delay){
    sleep(delay);
    sem_t* sem = sem_open((char*)path.data(), O_RDWR);
    sem_post(sem);
}

TEST_F(SetupCoord, creationNoSemaphore){
    // Check it joins holding the locks and waking up the writer
    std::thread t_c(&delay_post, sem_path, 2);
    //std::cout<< args.passwd << std::endl;
    ShmReceiver rec(Method::shm, Role::receiver, args);
    t_c.join();

    LockType i = test_rwlock(&(coord->lcs_leaving[1]));

    EXPECT_EQ(i, LockType::Read);
    EXPECT_EQ(coord->rdr_cnt.var, 1);
}


TEST_F(SemUnlocked, creationUnlckSemaphore){
    // Check it joins holding the locks and waking up the writer
    ShmReceiver rec(Method::shm, Role::receiver, args);
    /*
    std::promise<LockType> p;
    auto f = p.get_future();
    std::thread t(&test_rwlock, std::move(p), &(coord->lcs_leaving[1]));
    t.join();
    LockType i = f.get();
    */

    EXPECT_EQ(test_rwlock(&(coord->lcs_leaving[1])), LockType::Read);
    EXPECT_EQ(coord->rdr_cnt.var, 1);
}

TEST_F(ReaderJoined, OneChunkTransfer){
    // Reads one chunk and the second is 0-length
    //std::cout << std::endl << "Starting test" << std::endl;
    ASSERT_EQ(recv->buff_size(), 1024);
    auto src = std::vector<byte>{'a','b','c'};
    UNSAFE_write_to_chunk(coord, 0, src, buff);
    //std::cout << "Written 1" << std::endl;
    UNSAFE_write_to_chunk(coord, 1, std::vector<byte>{}, buff);
    //std::cout << "Written 2" << std::endl;
    int r = pthread_rwlock_unlock(&(coord->lcs_active[0]));


    ASSERT_TRUE(recv->ready());
    //std::cout << "Ready" << std::endl;
    ASSERT_EQ(test_rwlock(&(coord->lcs_leaving[1])), LockType::Free);
    ASSERT_EQ(test_rwlock(&(coord->lcs_active[0])), LockType::Read);
    //std::cout << "Locks ok" << std::endl;
    auto res = recv->receive(recv->buff_size());
    //std::cout << "Received" << std::endl;
    ASSERT_EQ(res, src);
    ASSERT_EQ(test_rwlock(&(coord->lcs_leaving[0])), LockType::Read);
    ASSERT_EQ(test_rwlock(&(coord->lcs_active[0])), LockType::Free);
    //std::cout << "Receive ok" << std::endl;

    ASSERT_FALSE(recv->ready());
    ASSERT_EQ(test_rwlock(&(coord->lcs_leaving[0])), LockType::Free);
    ASSERT_EQ(test_rwlock(&(coord->lcs_active[1])), LockType::Free);
    ASSERT_THROW(recv->receive(recv->buff_size()), OwnError);

    //std::cout << "Finished test" << std::endl;
    delete recv;
}

void write_parallel(struct SHM::Coord* coord, std::vector<byte> vec, int idx, char* buff, int delay, int pipe_sender){
    pthread_rwlock_wrlock(&(coord->lcs_active[idx]));
    write(pipe_sender, "Ok", 1);
    sleep(delay);
    UNSAFE_write_to_chunk(coord, idx, vec, buff);
    pthread_rwlock_unlock(&(coord->lcs_active[idx]));
}

TEST_F(ReaderJoined, Wraparound){
    // Fourth chunk is 0 length    
    auto src = std::vector<byte>{'a','b','c'};
    UNSAFE_write_to_chunk(coord, 0, src, buff);
    UNSAFE_write_to_chunk(coord, 1, src, buff);
    pthread_rwlock_unlock(&(coord->lcs_active[0]));

    ASSERT_TRUE(recv->ready());
    ASSERT_EQ(test_rwlock(&(coord->lcs_leaving[1])), LockType::Free);
    ASSERT_EQ(test_rwlock(&(coord->lcs_active[0])), LockType::Read);
    auto res = recv->receive(recv->buff_size());
    EXPECT_EQ(res, src);
    ASSERT_EQ(test_rwlock(&(coord->lcs_leaving[0])), LockType::Read);
    ASSERT_EQ(test_rwlock(&(coord->lcs_active[0])), LockType::Free);

    ASSERT_TRUE(recv->ready());
    ASSERT_EQ(test_rwlock(&(coord->lcs_leaving[0])), LockType::Free);
    ASSERT_EQ(test_rwlock(&(coord->lcs_active[1])), LockType::Read);
    res = recv->receive(recv->buff_size());
    EXPECT_EQ(res, src);
    ASSERT_EQ(test_rwlock(&(coord->lcs_leaving[1])), LockType::Read);
    ASSERT_EQ(test_rwlock(&(coord->lcs_active[1])), LockType::Free);

    src.push_back('d');
    UNSAFE_write_to_chunk(coord, 0, src, buff);

    ASSERT_TRUE(recv->ready());
    ASSERT_EQ(test_rwlock(&(coord->lcs_leaving[1])), LockType::Free);
    ASSERT_EQ(test_rwlock(&(coord->lcs_active[0])), LockType::Read);
    res = recv->receive(recv->buff_size()); // locks 
    EXPECT_EQ(res, src);
    ASSERT_EQ(test_rwlock(&(coord->lcs_leaving[0])), LockType::Read);
    ASSERT_EQ(test_rwlock(&(coord->lcs_active[0])), LockType::Free);
    
    int pipes[2];
    char b_p[1];
    pipe(pipes);
    std::thread t(write_parallel, coord, std::vector<byte>{}, 1, buff, 3, pipes[1]);

    read(pipes[0], b_p, 1);
    ASSERT_EQ(test_rwlock(&(coord->lcs_active[1])), LockType::Write);
    ASSERT_FALSE(recv->ready());
    t.join();
    ASSERT_EQ(test_rwlock(&(coord->lcs_leaving[0])), LockType::Free);
    ASSERT_EQ(test_rwlock(&(coord->lcs_active[1])), LockType::Free);

    delete recv;
    ASSERT_EQ(0, coord->rdr_cnt.var);
}

TEST_F(ReaderJoined, DoubleTrouble){
    // Double check and double read
    // Test locks and asserts
    auto src = std::vector<byte>{'a','b','c'};
    UNSAFE_write_to_chunk(coord, 0, src, buff);
    UNSAFE_write_to_chunk(coord, 1, src, buff);
    pthread_rwlock_unlock(&(coord->lcs_active[0]));

    ASSERT_TRUE(recv->ready());
    ASSERT_EQ(test_rwlock(&(coord->lcs_leaving[1])), LockType::Free);
    ASSERT_EQ(test_rwlock(&(coord->lcs_active[0])), LockType::Read);
    ASSERT_TRUE(recv->ready());
    ASSERT_EQ(test_rwlock(&(coord->lcs_leaving[1])), LockType::Free);
    ASSERT_EQ(test_rwlock(&(coord->lcs_active[0])), LockType::Read);
    auto res = recv->receive(recv->buff_size());
    EXPECT_EQ(res, src);
    ASSERT_EQ(test_rwlock(&(coord->lcs_leaving[0])), LockType::Read);
    ASSERT_EQ(test_rwlock(&(coord->lcs_active[0])), LockType::Free);
    ASSERT_THROW(recv->receive(recv->buff_size()), OwnError);

    delete recv;
}

TEST_F(ReaderJoined, AnomalousDestroyRead){
    auto src = std::vector<byte>{'a','b','c'};
    UNSAFE_write_to_chunk(coord, 0, src, buff);
    UNSAFE_write_to_chunk(coord, 1, src, buff);
    pthread_rwlock_unlock(&(coord->lcs_active[0]));

    ASSERT_TRUE(recv->ready());
    auto res = recv->receive(recv->buff_size());
    EXPECT_EQ(res, src);
    ASSERT_EQ(test_rwlock(&(coord->lcs_leaving[0])), LockType::Read);
    ASSERT_EQ(test_rwlock(&(coord->lcs_active[0])), LockType::Free);
    ASSERT_THROW(recv->receive(recv->buff_size()), OwnError);
    delete recv;
    ASSERT_EQ(test_rwlock(&(coord->lcs_leaving[0])), LockType::Free);
    ASSERT_EQ(0, coord->rdr_cnt.var);

}

TEST_F(ReaderJoined, AnomalousDestroyCheck){
    auto src = std::vector<byte>{'a','b','c'};
    UNSAFE_write_to_chunk(coord, 0, src, buff);
    pthread_rwlock_unlock(&(coord->lcs_active[0]));

    ASSERT_TRUE(recv->ready());
    delete recv;
    ASSERT_EQ(test_rwlock(&(coord->lcs_active[0])), LockType::Free);
    ASSERT_EQ(0, coord->rdr_cnt.var);

}
