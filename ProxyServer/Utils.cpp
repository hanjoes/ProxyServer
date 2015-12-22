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

int getLines(const char *buf, int len, std::vector<std::string> &lines) {
    int lo = 0;
    int hi = 0;
    while (hi < len) {
        if (hi > 1 && buf[hi] == '\n' && buf[hi-1] == '\r') {
            lines.push_back(std::string(buf+lo, hi-lo-1));
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
        std::string l = trim(s.substr(0, colon));
        std::string r = trim(s.substr(colon+1, s.size()));
        p = std::make_pair(l, r);
    }
    return p;
}

std::string trim(const std::string &s) {
    size_t lo = s.find_first_not_of(' ');
    if (lo == std::string::npos) return "";
    size_t hi = s.find_last_not_of(' ');
    return s.substr(lo, hi-lo+1);
}

