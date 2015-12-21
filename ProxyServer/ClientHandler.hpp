//
//  ClientHandler.hpp
//  ProxyServer
//
//  Created by Hanzhou Shi on 12/16/15.
//  Copyright © 2015 USF. All rights reserved.
//

#ifndef ClientHandler_hpp
#define ClientHandler_hpp

#include <cstdio>
#include <string>
#include <iostream>

#include "Utils.hpp"

const static int MAX_BUFFER_LEN = 4*1024;

class ClientHandler {
public:
    ClientHandler(const std::string &host, unsigned short port);
    
    ~ClientHandler();
    
    std::string dump();
    
    void processRequest(int fd, size_t dataLen);

private:
    MSS generateHeaderMap(int fd);

private:
    std::string host;
    unsigned short port;
    
    MSS headerMap;

    int dataLen;
    char buffer[MAX_BUFFER_LEN];
};

#endif /* ClientHandler_hpp */
