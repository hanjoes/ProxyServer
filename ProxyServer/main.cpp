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
        std::cout << "Try: " << " ./main [host] [port]" << std::endl;
        return 0;
    }
    std::string host = argv[1];
    int port = atoi(argv[2]);
    auto server = std::unique_ptr<ProxyServer>(new ProxyServer(host, port, false));
    server->start();
    return 0;
}
