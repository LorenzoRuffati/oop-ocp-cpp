#include "common.hpp"

#include <random>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <vector>

void QueuePreexist::SetUp(){
    struct mq_attr attr{0};
    attr.mq_msgsize = 100;
    attr.mq_maxmsg = 10;
    attr.mq_flags = 0;

    path = std::string("/test_queue");
    descr = mq_open(&path[0], O_CREAT | O_RDWR, 0660, &attr);
}

void QueuePreexist::TearDown(){
    mq_close(descr);
    mq_unlink(&path[0]);
    //std::cout << "Destroying queue " << path << std::endl;
}

void QueueHasMessages::SetUp(){
    QueuePreexist::SetUp();
    std::string test = "Hello world";
    mq_send(descr, &test[0], test.length(), 0);
}

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