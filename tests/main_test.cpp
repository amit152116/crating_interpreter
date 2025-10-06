#include <gtest/gtest.h>

#include "Thor/Thor.hpp"

TEST(ThorTest, Tokenize) {
    EXPECT_TRUE(true);
}

TEST(ThorTest, LexerTest) {
    std::string output = "Hello from ProjectClass!";
    EXPECT_EQ(output, "Hello from ProjectClass!");
}
