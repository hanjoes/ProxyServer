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

using namespace std;

class ClientHandler {
public:
    ClientHandler(const string &host, int port);

private:
    const string& host;
    int port;
};

#endif /* ClientHandler_hpp */
