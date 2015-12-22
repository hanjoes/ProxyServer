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
    EXPECT_STREQ("User-Agent: Wget/1.17 (darwin15.0.0)", lines.front().c_str());
    EXPECT_STREQ("User-Agent: Wget/1.17 (darwin15.0.0)", lines.back().c_str());
    lines.clear();
    
    const std::string input2 = "User-Agent: Wget/1.17 (darwin15.0.0)\r\n\r\n";
    
    consumed = getLines(input2.c_str(), (int)input2.size(), lines);
    EXPECT_EQ(input2.size(), consumed);
    EXPECT_STREQ("User-Agent: Wget/1.17 (darwin15.0.0)", lines.front().c_str());
    EXPECT_STREQ("", lines.back().c_str());
    lines.clear();
    
    const std::string input3 = "User-Agent: Wget/1.17 (darwin15.0.0)";
    
    consumed = getLines(input3.c_str(), (int)input3.size(), lines);
    EXPECT_EQ(0, consumed);
    EXPECT_EQ(0, lines.size());
    lines.clear();
}

TEST(Utils, GetPartsFromCmd) {
    const std::string input1 =
    "GET "
    "http://docs.oracle.com/javase/7/docs/api/java/net/Socket.html "
    "HTTP/1.1";
    
    std::vector<std::string> parts = getPartsFromCmd(input1);
    EXPECT_STREQ("GET", parts[0].c_str());
    EXPECT_STREQ("http://docs.oracle.com/javase/7/docs/api/java/net/"
                 "Socket.html", parts[1].c_str());
    EXPECT_STREQ("HTTP/1.1", parts[2].c_str());
}

TEST(Utils, GetHostFromUrl) {
    const std::string input1 =
    "http://docs.oracle.com/javase/7/docs/api/java/net"
    "/Socket.html";
    
    std::string host = getHostFromUrl(input1);
    EXPECT_STREQ("docs.oracle.com", host.c_str());
}


int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
