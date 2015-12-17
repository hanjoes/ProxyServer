//
//  ProxyServer.cpp
//  ProxyServer
//
//  Created by Hanzhou Shi on 12/16/15.
//  Copyright © 2015 USF. All rights reserved.
//

#include <iostream>
#include "ProxyServer.hpp"

ProxyServer::ProxyServer(const string &host, int port)
: host(host), port(port)
{
}

void ProxyServer::start() {
    while (true) {
        cout << "host: " << host << " port: " << port << endl;
        sleep(2);
    }
}