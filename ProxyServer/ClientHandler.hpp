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

class ClientHandler {
public:
    ClientHandler(const std::string &host, unsigned short port);
    
    ~ClientHandler();
    
    std::string dump();
    
private:
    std::string host;
    unsigned short port;
};

#endif /* ClientHandler_hpp */
