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
    
    void processRequest(int fd);
    
    void requestUpstreamAndForward(int fd);
    
    bool canDispatch();
    
private:
    LSS generateHeaders();

private:
    std::string host;
    unsigned short port;
    
    std::vector<std::string> lines;

    int dataLen;
    char buffer[MAX_BUFFER_LEN];
};

#endif /* ClientHandler_hpp */
