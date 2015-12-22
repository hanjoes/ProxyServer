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
    memset(buffer, 0, MAX_BUFFER_LEN);
    dataLen = 0;
    
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
    debug("CMD::" + cmd);
    std::string host = getHostFromUrl(parts[1]);
    int upstream = connectToHost(host);
    if (upstream < 0) {
        std::cerr
        << "connect to upstream failed.. "
        << strerror(errno)
        << std::endl;
        return;
    }
    LSS headers = getClientHeaderList();
    std::string request = getRequest(headers, cmd);
    
    process(request, upstream, fd);
}

bool ClientHandler::canDispatch() {
    return lines.back().size() == 0;
}

LSS ClientHandler::getClientHeaderList() {
    /// split lines into pairs, and strip "User-Agent" and
    /// "referer" browser headers.
    LSS headers;
    for (int i = 1; i < lines.size()-1; ++i) {
        PSS p = pairByColon(lines[i]);
        const std::string &name = p.first;
        if (!name.compare("REFERER") || !name.compare("USER-AGENT"))
            continue;
        headers.push_back(p);
    }
    lines.clear();
    return headers;
}

std::string ClientHandler::getRequest(const LSS &headers,
                                      const std::string &cmd) {
    std::string req = cmd + "\r\n";
    for (auto p : headers) req += p.first + ": " + p.second + "\r\n";
    req += "\r\n"; /// an extra CRLF for termination
    return req;
}

void ClientHandler::process(const std::string &req, int us, int ds) {
    debug("request: ");
    debug(req);

    memset(buffer, 0, MAX_BUFFER_LEN);
    memcpy(buffer, req.c_str(), req.size());
    
    ssize_t ret = flushBuffer(buffer, req.size(), us);
    if (ret < 0) return;
    
    getResponseAndForward(us, ds);
}

void ClientHandler::getResponseAndForward(int us, int ds) {
    memset(buffer, 0, MAX_BUFFER_LEN);
    
    ssize_t ret;
    while ((ret = read(us, buffer, MAX_BUFFER_LEN)) > 0) {
        ret = flushBuffer(buffer, ret, ds);
        if (ret < 0) break;
    }
}

int ClientHandler::flushBuffer(char *buffer, ssize_t len, int fd) {
    ssize_t ret;
    ssize_t remainDataLen = len;
    while (remainDataLen > 0) {
        ret = write(fd, buffer, remainDataLen);
        if (ret < 0) {
            error("flush error.");
            return -1;
        }
        remainDataLen -= ret;
        memmove(buffer, buffer+ret, remainDataLen);
    }
    return 0;
}


// get a line from buffer, divided by CRLF.
// returns the number of bytes consumed.
int ClientHandler::getLines(const char *buf,
                            int len,
                            std::vector<std::string> &lines) {
    int lo = 0;
    int hi = 0;
    while (hi < len) {
        if (hi > 1 && buf[hi] == '\n' && buf[hi-1] == '\r') {
            lines.push_back(std::string(buf+lo, hi-lo-1));
            lo = hi+1;
        }
        ++hi;
        if (lines.size() > 0 && lines.back().size() == 0) break;
    }
    return lo;
}