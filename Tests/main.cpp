//
//  main.cpp
//  Tests
//
//  Created by Hanzhou Shi on 12/21/15.
//  Copyright © 2015 USF. All rights reserved.
//

#include <iostream>

#include "gtest/gtest.h"
#include "Utils.hpp"

TEST(Utils, SplitByColon) {
    const char *input1 = "User-Agent: Wget/1.17 (darwin15.0.0)";
    PSS pss = splitByColon(input1);
    EXPECT_STREQ("User-Agent", pss.first.c_str());
    EXPECT_STREQ("Wget/1.17 (darwin15.0.0)", pss.second.c_str());
    
    const char *input2 = "  a : b  ";
    pss = splitByColon(input2);
    EXPECT_STREQ("a", pss.first.c_str());
    EXPECT_STREQ("b", pss.second.c_str());
    
    const char *input3 = "   :   ";
    pss = splitByColon(input3);
    EXPECT_STREQ("", pss.first.c_str());
    EXPECT_STREQ("", pss.second.c_str());
}

TEST(Utils, GetLines) {
    const std::string line1 = "User-Agent: Wget/1.17 (darwin15.0.0)\r\n";
    const std::string line2 = "Wget/1.17 (darwin15.0.0)";
    const std::string input1 = line1 + line2;
    std::vector<std::string> lines;
    
    int consumed = getLines(input1.c_str(), (int)input1.size(), lines);
    EXPECT_EQ(line1.size(), consumed);
    EXPECT_EQ("User-Agent: Wget/1.17 (darwin15.0.0)", lines.front());
    EXPECT_EQ("User-Agent: Wget/1.17 (darwin15.0.0)", lines.back());
    
    const std::string input2 = "User-Agent: Wget/1.17 (darwin15.0.0)\r\n\r\n";
    
    consumed = getLines(input2.c_str(), (int)input2.size(), lines);
    EXPECT_EQ(input2.size(), consumed);
    EXPECT_EQ("User-Agent: Wget/1.17 (darwin15.0.0)", lines.front());
    EXPECT_EQ("", lines.back());
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
