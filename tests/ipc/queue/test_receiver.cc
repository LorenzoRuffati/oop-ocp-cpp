#include "common.hpp"

TEST(QueueNotExist, CreateReceiver){
    try
    {
        OptArgs args;
        args.filename = std::string{"/test_queue"};
        auto mq = MQWrite(Method::queue, Role::receiver, args);
        ASSERT_TRUE(mq.ready()) << "Queue created but ready fails";
    }
    catch(const std::exception& e)
    {
        FAIL() << "Queue failed creating";
    }
}

TEST_F(QueuePreexist, CreateReceiver){
    try
    {
        OptArgs args;
        args.filename = path;
        auto mq = MQWrite(Method::queue, Role::receiver, args);
        ASSERT_TRUE(mq.ready()) << "Queue created but ready fails";
    }
    catch(const std::exception& e)
    {
        FAIL() << "Queue failed creating";
    }
}

TEST_F(QueueHasMessages, CreateReceiver){
    try
    {
        OptArgs args;
        args.filename = path;
        auto mq = MQWrite(Method::queue, Role::receiver, args);
        ASSERT_TRUE(mq.ready()) << "Queue created but ready fails";
    }
    catch(const std::exception& e)
    {
        FAIL() << "Queue failed";
    }
}

TEST_F(QueuePreexist, readywriterfinmsg){
    std::vector<char> rando_high_prio{1,2,3};
    mq_send(descr, &rando_high_prio[0], rando_high_prio.size(), 1);
    OptArgs args;
    args.filename = path;
    auto mq = MQWrite(Method::queue, Role::receiver, args);
    ASSERT_FALSE(mq.ready()) << "Expected false upon having only one message which has high priority";
}

TEST_F(QueueHasMessages, readymultiplemsgs){
    std::vector<char> rando_high_prio{1,2,3};
    mq_send(descr, &rando_high_prio[0], rando_high_prio.size(), 1);
    OptArgs args;
    args.filename = path;
    auto mq = MQWrite(Method::queue, Role::receiver, args);
    ASSERT_TRUE(mq.ready()) << "Expected true when having more than one message";
}

TEST_F(QueuePreexist, readynomsgfin){
    std::vector<char> rando_high_prio{1,2,3};
    mq_send(descr, &rando_high_prio[0], rando_high_prio.size(), 1);
    OptArgs args;
    args.filename = path;
    auto mq = MQWrite(Method::queue, Role::receiver, args);
    ASSERT_FALSE(mq.ready());
    ASSERT_FALSE(mq.ready()) << "Expected false";
}

TEST_F(QueueHasMessages, readymsgfin){
    std::vector<char> rando_high_prio{1,2,3};
    mq_send(descr, &rando_high_prio[0], rando_high_prio.size(), 0);
    mq_send(descr, &rando_high_prio[0], rando_high_prio.size(), 1);
    OptArgs args;
    args.filename = path;
    auto mq = MQWrite(Method::queue, Role::receiver, args);
    auto r = mq.ready();
    ASSERT_TRUE(r);
    mq.receive(mq.buff_size());
    ASSERT_TRUE(mq.ready()) << "Expected true";
}

TEST_F(QueueHasMessages, updtmsgfinfrmrcv){
    std::vector<char> rando_high_prio{1,2,3};
    OptArgs args;
    args.filename = path;

    auto mq = MQWrite(Method::queue, Role::receiver, args);
    auto r = mq.ready();
    ASSERT_TRUE(r);
    
    mq_send(descr, &rando_high_prio[0], rando_high_prio.size(), 1);
    mq.receive(mq.buff_size());
    
    ASSERT_FALSE(mq.ready());
}

TEST_F(QueuePreexist, frombuff){
    std::vector<byte> src, buff, dest;

    OptArgs args;
    args.filename = path;

    auto mq = MQWrite(Method::queue, Role::receiver, args);
    size_t buffs = mq.buff_size();

    src = write_random(buffs-1);
    send_bytes_to_queue(src, descr);
    ASSERT_TRUE(mq.ready()); // Puts into the buffer
    dest = mq.receive(buffs);
    ASSERT_TRUE(dest == src);
}

TEST_F(QueuePreexist, firstisendmsg){
    std::vector<byte> src, buff, dest;
    std::vector<char> rando_high_prio{1,2,3};

    OptArgs args;
    args.filename = path;

    auto mq = MQWrite(Method::queue, Role::receiver, args);
    size_t buffs = mq.buff_size();

    src = write_random(buffs);
    send_bytes_to_queue(src, descr);
    mq_send(descr, &rando_high_prio[0], rando_high_prio.size(), 1);
    ASSERT_TRUE(mq.ready());
    dest = mq.receive(buffs);
    ASSERT_TRUE(dest == src);
}
