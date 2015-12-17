//
//  main.cpp
//  ProxyServer
//
//  Created by Hanzhou Shi on 12/16/15.
//  Copyright © 2015 USF. All rights reserved.
//

#include <iostream>
#include <memory>

#include "ProxyServer.hpp"

int main(int argc, const char * argv[]) {
    if (argc != 3) {
        cout << "Try: " << " ./main [host] [port]" << endl;
        return 0;
    }
    string host = argv[1];
    int port = atoi(argv[2]);
    auto server = unique_ptr<ProxyServer>(new ProxyServer(host, port));
    server->start();
    return 0;
}
