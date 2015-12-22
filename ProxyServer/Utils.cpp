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

#include <assert.h>

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <err.h>


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

// get a line from buffer, divided by CRLF.
// returns the number of bytes consumed.
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

PSS getHeaderPair(const std::string &s) {
    size_t colon = s.find(':');
    PSS p;
    if (colon != std::string::npos) {
        std::string l = toUpper(trim(s.substr(0, colon)));
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

std::string getIpFromHost(const std::string &host) {
    struct addrinfo hints, *res, *res0;
    const char *cause = NULL;
    std::string ret = "";
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    int s = getaddrinfo(host.c_str(), NULL, &hints, &res0);
    if (s) printErrorAndExit(gai_strerror(s));
    
    for (res = res0; res; res = res->ai_next) {
        s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (s < 0) {
            cause = "socket";
            continue;
        }
        struct sockaddr_in *sockAddr = (struct sockaddr_in *)res->ai_addr;
        ret = inet_ntoa(sockAddr->sin_addr);
        /// got one
        break;
    }
    
    if (s < 0) {
        err(1, "%s", cause);
    }
    freeaddrinfo(res0);
    return ret;
}

std::vector<std::string> getPartsFromCmd(const std::string &cmd) {
    std::stringstream ss(cmd);
    std::vector<std::string> parts;
    std::string part;
    while (ss >> part) parts.push_back(part);
    return parts;
}

std::string getHostFromUrl(const std::string &url) {
    assert(url.find("http") == 0);
    size_t lo = url.find_first_of("//");
    assert(lo != std::string::npos);
    lo += 2;/// skip the protocol part ( http:// or https:// )
    size_t hi = url.find_first_of("/", lo);
    if (hi == std::string::npos) hi = url.size();
    return url.substr(lo, hi-lo);
}

std::string toUpper(std::string &&s) {
    for (auto i = 0; i < s.size(); ++i) {
        s[i] = toupper(s[i]);
    }
    return s;
}
