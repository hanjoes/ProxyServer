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
    
    void setDispatched(bool status);
    
    static void *run(void *args);
    
private:
    void forwardResponseHeader(int fd);
    
    int fillReadBuffer(int fd);
    
    void resetBuffer();
    
    void leftShiftData(size_t lo, size_t len);
    
    LSS getHeaderList();
    
    std::string writeHeader(const LSS &headers, const std::string &cmd);
    
    void process(const std::string &req, int us, int ds);
    
    void getResponseAndForward(int us, int ds);
    
    int flushBuffer(const char *buffer, ssize_t len, int fd);

private:
    std::string host;
    unsigned short port;
    bool dispatched;
    
    std::vector<std::string> lines;

    size_t dataLen;
    char buffer[MAX_BUFFER_LEN];
};

#endif /* ClientHandler_hpp */
