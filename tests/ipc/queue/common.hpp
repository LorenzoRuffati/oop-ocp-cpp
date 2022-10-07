#ifndef TEST_HEADER_QUEUE
#define TEST_HEADER_QUEUE
#include <gtest/gtest.h>
#include <algorithm>
#include "src/ipc/ipc.hpp"


class QueuePreexist: public ::testing::Test {
    protected:
        std::string path;
        mqd_t descr;

        void SetUp() override;
        void TearDown() override;
};


class QueueHasMessages: public QueuePreexist {
    protected:
        void SetUp() override;
};

std::vector<byte> write_random(size_t num);

void send_bytes_to_queue(std::vector<byte>, mqd_t);

#endif