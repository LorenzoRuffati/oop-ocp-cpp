#include "common.hpp"

TEST(QueueNotExist, CreateSender){
    try
    {
        OptArgs args;
        args.filename = std::string{"/test_queue"};
        auto mq = MQWrite(Method::queue, Role::sender, args);
        ASSERT_TRUE(mq.ready()) << "Queue created but ready fails";
    }
    catch(const std::exception& e)
    {
        FAIL() << "Queue failed creating";
    }
}

TEST_F(QueuePreexist, CreateSender){
    try
    {
        OptArgs args;
        args.filename = path;
        auto mq = MQWrite(Method::queue, Role::sender, args);
        ASSERT_TRUE(mq.ready()) << "Queue created but ready fails";
    }
    catch(const std::exception& e)
    {
        FAIL() << "Queue failed creating";
    }
}

TEST_F(QueueHasMessages, CreateSender){
    try
    {
        OptArgs args;
        args.filename = path;
        MQWrite(Method::queue, Role::sender, args);
        FAIL() << "Accessing as a writer a queue with messages must be an error" ;
    }
    catch(const std::exception& e)
    {
        SUCCEED();
    }
}

// Use QueuePreexist for the next tests so I can check the queue already

TEST_F(QueuePreexist, sendfullbuff){
    OptArgs args;
    args.filename = path;
    auto mq = MQWrite(Method::queue, Role::sender, args);
    ASSERT_TRUE(mq.ready());
    size_t buff_s = mq.buff_size();

    std::vector<byte> source, dest, buff;
    source = write_random(buff_s);
    auto r = mq.send(source);
    ASSERT_TRUE(r);

    buff.reserve(buff_s*2);

    unsigned int prio = 0;
    size_t nr = mq_receive(descr, (char*)buff.data(), buff_s, &prio);
    EXPECT_EQ(nr, buff_s);
    EXPECT_EQ(prio, 0);
    
    std::copy_n(buff.data(), nr, std::back_inserter(dest));
    ASSERT_EQ(source.size(), dest.size());
    ASSERT_TRUE(source == dest);
}

TEST_F(QueuePreexist, sendpartialbuff){
    OptArgs args;
    args.filename = path;
    auto mq = MQWrite(Method::queue, Role::sender, args);
    ASSERT_TRUE(mq.ready());
    size_t buff_s = mq.buff_size();

    std::vector<byte> source, dest, buff;
    source = write_random(buff_s - 1);
    auto r = mq.send(source);
    ASSERT_TRUE(r);

    buff.reserve(buff_s*2);

    unsigned int prio = 0;
    size_t nr = mq_receive(descr, (char*)buff.data(), buff_s, &prio);
    EXPECT_EQ(nr, buff_s-1);
    EXPECT_EQ(prio, 0);
    
    std::copy_n(buff.data(), nr, std::back_inserter(dest));
    ASSERT_EQ(source.size(), dest.size());
    ASSERT_TRUE(source == dest);
}

TEST_F(QueuePreexist, sendemptybuff){
    OptArgs args;
    args.filename = path;
    auto mq = MQWrite(Method::queue, Role::sender, args);
    ASSERT_TRUE(mq.ready());
    size_t buff_s = mq.buff_size();

    std::vector<byte> source(0), dest, buff;
    auto r = mq.send(source);
    ASSERT_TRUE(r);
    buff.reserve(buff_s*2);
    unsigned int prio = 0;
    mq_receive(descr, (char*)buff.data(), buff_s, &prio);
    EXPECT_GT(prio, 0);
}

TEST_F(QueuePreexist, sendafterempty){
    OptArgs args;
    args.filename = path;
    auto mq = MQWrite(Method::queue, Role::sender, args);
    ASSERT_TRUE(mq.ready());
    size_t buff_s = mq.buff_size();

    std::vector<byte> source(0), dest, buff;
    auto r = mq.send(source);
    ASSERT_TRUE(r);
    source = write_random(buff_s);
    r = mq.send(source);
    ASSERT_FALSE(r);
}