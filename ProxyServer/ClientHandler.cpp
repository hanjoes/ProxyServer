//
//  ClientHandler.cpp
//  ProxyServer
//
//  Created by Hanzhou Shi on 12/16/15.
//  Copyright © 2015 USF. All rights reserved.
//

#include "ClientHandler.hpp"

ClientHandler::ClientHandler(const std::string &host, unsigned short port)
: host(host), port(port)
{
}

ClientHandler::~ClientHandler() {
    std::cout << "destructor called..." << std::endl;
}

std::string ClientHandler::dump() {
    return host + ":" + std::to_string(port);
}

