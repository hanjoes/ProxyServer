//
//  Utils.cpp
//  ProxyServer
//
//  Created by Hanzhou Shi on 12/20/15.
//  Copyright © 2015 USF. All rights reserved.
//

#include "Utils.hpp"

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
