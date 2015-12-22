//
//  ClientHandler.cpp
//  ProxyServer
//
//  Created by Hanzhou Shi on 12/16/15.
//  Copyright © 2015 USF. All rights reserved.
//

#include "ClientHandler.hpp"
#include "Utils.hpp"

#include <sstream>

#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

ClientHandler::ClientHandler(const std::string &host, unsigned short port)
: host(host), port(port), dataLen(0)
{
    memset(buffer, 0, MAX_BUFFER_LEN);
}

ClientHandler::~ClientHandler() {
}

std::string ClientHandler::dump() {
    return host + ":" + std::to_string(port);
}

void ClientHandler::processRequest(int fd) {
    char *bufferStart = ((char *)buffer) + dataLen;
    ssize_t ret = read(fd, bufferStart, MAX_BUFFER_LEN - dataLen);
    if (ret < 0) printErrorAndExit("read failed.");
    dataLen += ret;
    
    debug(bufferStart);
    
    /// read all lines from current buffer.
    int usedDataLen = getLines(buffer, dataLen, lines);
    int remainDataLen = dataLen - usedDataLen;
    /// update buffer
    if (usedDataLen < MAX_BUFFER_LEN) {
        /// move unused data to beginning.
        memmove(buffer, buffer+usedDataLen, remainDataLen);
    }
    memset(buffer+remainDataLen, 0, MAX_BUFFER_LEN-remainDataLen);
}

/// launch a thread to communicate with upstream and
/// forward response to the client using fd.
void ClientHandler::requestUpstreamAndForward(int fd) {
    std::string cmd = lines.front();
    std::vector<std::string> parts = getPartsFromCmd(cmd);
    std::string host = getHostFromUrl(parts[1]);
    std::string ip = getIpFromHost(host);
    debug("host: " + host + " ip: " + ip);
    
    LSS headers = generateHeaders();
}

bool ClientHandler::canDispatch() {
    return lines.back().size() == 0;
}

LSS ClientHandler::generateHeaders() {
    /// split lines into pairs, and strip "User-Agent" and
    /// "referer" browser headers.
    LSS headers;
    for (int i = 1; i < lines.size()-1; ++i) {
        PSS p = splitByColon(lines[i]);
        debug(p.first + " -> " + p.second);
        headers.push_back(p);
    }
    return headers;
}
