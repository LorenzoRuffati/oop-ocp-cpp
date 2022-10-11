#include "common.hpp"
#include "src/utils/utils.hpp"

void delay_post(std::string path, int delay){
    sleep(delay);
    sem_t* sem = sem_open((char*)path.data(), O_RDWR);
    sem_post(sem);
}

using namespace SHM;
TEST_F(SetupCoord, creationNoSemaphore){
    // Check it joins holding the locks and waking up the writer
    std::thread t_c(&delay_post, sem_path, 3);
    ShmReceiver rec(Method::shm, Role::receiver, args);

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
    ASSERT_EQ(recv->buff_size(), 1024);
    auto src = std::vector<byte>{'a','b','c'};
    write_to_chunk(coord, 0, src, buff);
    write_to_chunk(coord, 1, std::vector<byte>{}, buff);
    pthread_rwlock_unlock(&(coord->lcs_active[0]));

    ASSERT_TRUE(recv->ready());
    ASSERT_EQ(test_rwlock(&(coord->lcs_leaving[1])), LockType::Free);
    ASSERT_EQ(test_rwlock(&(coord->lcs_active[0])), LockType::Read);
    auto res = recv->receive(recv->buff_size());
    ASSERT_EQ(res, src);
    ASSERT_EQ(test_rwlock(&(coord->lcs_leaving[0])), LockType::Read);
    ASSERT_EQ(test_rwlock(&(coord->lcs_active[0])), LockType::Free);

    ASSERT_FALSE(recv->ready());
    ASSERT_EQ(test_rwlock(&(coord->lcs_leaving[0])), LockType::Free);
    ASSERT_EQ(test_rwlock(&(coord->lcs_active[1])), LockType::Free);
    ASSERT_THROW(recv->receive(recv->buff_size()), OwnError);

    delete recv;
}

void write_parallel(struct SHM::Coord* coord, std::vector<byte> vec, int idx, char* buff, int delay){
    sleep(delay);
    write_to_chunk(coord, idx, vec, buff);
}

TEST_F(ReaderJoined, Wraparound){
    // Fourth chunk is 0 length    
    auto src = std::vector<byte>{'a','b','c'};
    write_to_chunk(coord, 0, src, buff);
    write_to_chunk(coord, 1, src, buff);
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
    write_to_chunk(coord, 0, src, buff);

    ASSERT_TRUE(recv->ready());
    ASSERT_EQ(test_rwlock(&(coord->lcs_leaving[1])), LockType::Free);
    ASSERT_EQ(test_rwlock(&(coord->lcs_active[0])), LockType::Read);
    res = recv->receive(recv->buff_size());
    EXPECT_EQ(res, src);
    ASSERT_EQ(test_rwlock(&(coord->lcs_leaving[0])), LockType::Read);
    ASSERT_EQ(test_rwlock(&(coord->lcs_active[0])), LockType::Free);
    
    std::thread t(write_parallel, coord, std::vector<byte>{}, 1, buff, 3);
    ASSERT_FALSE(recv->ready());
    ASSERT_EQ(test_rwlock(&(coord->lcs_leaving[0])), LockType::Free);
    ASSERT_EQ(test_rwlock(&(coord->lcs_active[1])), LockType::Free);

    delete recv;
    ASSERT_EQ(0, coord->rdr_cnt.var);
}

TEST_F(ReaderJoined, DoubleTrouble){
    // Double check and double read
    // Test locks and asserts
    auto src = std::vector<byte>{'a','b','c'};
    write_to_chunk(coord, 0, src, buff);
    write_to_chunk(coord, 1, src, buff);
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
    write_to_chunk(coord, 0, src, buff);
    write_to_chunk(coord, 1, src, buff);
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
    write_to_chunk(coord, 0, src, buff);
    pthread_rwlock_unlock(&(coord->lcs_active[0]));

    ASSERT_TRUE(recv->ready());
    delete recv;
    ASSERT_EQ(test_rwlock(&(coord->lcs_active[0])), LockType::Free);
    ASSERT_EQ(0, coord->rdr_cnt.var);

}
