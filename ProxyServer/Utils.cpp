//
//  Utils.cpp
//  ProxyServer
//
//  Created by Hanzhou Shi on 12/20/15.
//  Copyright © 2015 USF. All rights reserved.
//

#include "Utils.hpp"

#include <sstream>
#include <iterator>
#include <vector>

void printErrorAndExit(const std::string &msg) {
    std::cout << msg << " Reason: " << strerror(errno) << std::endl;
    exit(-1);
}

void debug(const std::string &msg) {
#if DEBUG
    std::cout << msg << std::endl;
#endif
}

std::string getServerKey(const std::string &host, unsigned short port) {
    return host + ":" + std::to_string(port);
}

int parseHttpRequest(const char *buffer, int dataLen, MSS &headerMap) {
    int lo = 0;
    int hi = 0;
    std::vector<std::string> lines;
    while (hi < dataLen) {
        if (hi > 1 && buffer[hi] == '\n' && buffer[hi-1] == '\r') {
            lines.push_back(std::string(buffer+lo, hi-lo-2));
            lo = hi+1;
        }
        ++hi;
    }
    return lo;
}

PSS splitByColon(const std::string &s) {
    size_t colon = s.find(':');
    PSS p;
    if (colon != std::string::npos) {
        std::string left = trim(s.substr(0, colon));
        std::string right = trim(s.substr(colon+1, s.size()));
    }
    return p;
}

std::string trim(const std::string &s) {
    size_t lo = s.find_first_not_of(' ');
    size_t hi = s.find_last_not_of(' ');
    return s.substr(lo, hi-lo+1);
}

