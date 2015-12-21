//
//  ClientHandler.cpp
//  ProxyServer
//
//  Created by Hanzhou Shi on 12/16/15.
//  Copyright © 2015 USF. All rights reserved.
//

#include "ClientHandler.hpp"
#include "Utils.hpp"

#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

ClientHandler::ClientHandler(const std::string &host, unsigned short port)
: host(host), port(port), dataLen(MAX_BUFFER_LEN)
{
    memset(buffer, 0, MAX_BUFFER_LEN);
}

ClientHandler::~ClientHandler() {
}

std::string ClientHandler::dump() {
    return host + ":" + std::to_string(port);
}

void ClientHandler::processRequest(int fd, size_t dataLen) {
    char *bufferStart = ((char *)buffer) + dataLen;
    ssize_t ret = read(fd, bufferStart, MAX_BUFFER_LEN - dataLen);
    if (ret < 0) printErrorAndExit("read failed.");
    debug(bufferStart);
}

