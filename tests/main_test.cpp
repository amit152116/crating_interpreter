#include <gtest/gtest.h>

#include "Krypton/Krypton.hpp"

TEST(KryptonTest, Tokenize) {
    EXPECT_TRUE(true);
}

TEST(KryptonTest, LexerTest) {
    std::string output = "Hello from ProjectClass!";
    EXPECT_EQ(output, "Hello from ProjectClass!");
}
