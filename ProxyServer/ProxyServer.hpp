//
//  ProxyServer.hpp
//  ProxyServer
//
//  Created by Hanzhou Shi on 12/16/15.
//  Copyright © 2015 USF. All rights reserved.
//

#ifndef ProxyServer_hpp
#define ProxyServer_hpp

#include <cstdio>
#include <string>

using namespace std;

class ProxyServer {

public:
    ProxyServer(const string &host, int port);
    
    void start();
    
private:
    const string &host;
    int port;
};

#endif /* ProxyServer_hpp */
