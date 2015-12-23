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
#include <assert.h>

ClientHandler::ClientHandler(const std::string &host, unsigned short port)
: host(host), port(port), dataLen(0), dispatched(false)
{
    resetBuffer();
}

ClientHandler::~ClientHandler() {
}

std::string ClientHandler::dump() {
    return host + ":" + std::to_string(port);
}

void ClientHandler::processRequest(int fd) {
    /// don't reset buffer here, since we could
    /// get partial header.
    if (fillReadBuffer(fd) < 0) return;
    
    /// get header from current buffer.
    size_t usedDataLen = getLines(buffer, dataLen, lines);
    size_t remainDataLen = dataLen - usedDataLen;
    /// update buffer
    if (usedDataLen < MAX_BUFFER_LEN) {
        /// move unused data to beginning.
        memmove(buffer, buffer+usedDataLen, remainDataLen);
    }
    memset(buffer+remainDataLen, 0, MAX_BUFFER_LEN-remainDataLen);
}

int ClientHandler::fillReadBuffer(int fd) {
    char *bufferStart = ((char *)buffer) + dataLen;
    ssize_t ret = read(fd, bufferStart, MAX_BUFFER_LEN - dataLen);
    if (ret >= 0) dataLen += ret;
    return (int)ret;
}

/// launch a thread to communicate with upstream and
/// forward response to the client using fd.
void ClientHandler::requestUpstreamAndForward(int fd) {
    /// process command.
    std::string cmd = lines.front();
    std::vector<std::string> parts = getPartsFromCmd(cmd);
    if (!parts[2].compare("HTTP/1.1")) parts[2] = "HTTP/1.0";
    cmd = parts[0] + " " + parts[1] + " " + parts[2];
    
    std::string host = getHostFromUrl(parts[1]);
    int upstream = connectToHost(host);
    if (upstream < 0) {
        std::cerr
        << "connect to upstream failed.. "
        << strerror(errno)
        << std::endl;
        return;
    }
    LSS headers = getHeaderList();
    
    std::string request = writeHeader(headers, cmd);
    
    process(request, upstream, fd);
    close(upstream);
}

bool ClientHandler::canDispatch() {
    return lines.size() > 0 && lines.back().size() == 0 && !dispatched;
}

void ClientHandler::setDispatched(bool status) {
    dispatched = status;
}

void *ClientHandler::run(void *args) {
    ClientHandlerArgs *runArgs = (ClientHandlerArgs *)args;
    ClientHandler *client = runArgs->clientHandler;
    client->requestUpstreamAndForward(runArgs->fd);
    delete runArgs;
    return NULL;
}

LSS ClientHandler::getHeaderList() {
    /// split lines into pairs, and strip "User-Agent" and
    /// "referer" browser headers.
    LSS headers;
    for (int i = 1; i < lines.size()-1; ++i) {
        PSS p = pairByColon(lines[i]);
        const std::string &name = p.first;
        const std::string &value = p.second;
        if (!name.compare("REFERER") || !name.compare("USER-AGENT") ||
            !value.compare("Keep-Alive"))
            continue;
        headers.push_back(p);
    }
    lines.clear();
    return headers;
}

std::string ClientHandler::writeHeader(const LSS &headers,
                                       const std::string &cmd) {
    std::string header = cmd + "\r\n";
    for (auto p : headers) header += p.first + ": " + p.second + "\r\n";
    header += "\r\n"; /// an extra CRLF for termination
    return header;
}

void ClientHandler::process(const std::string &req, int us, int ds) {
    debug("request: ");
    debug(req);
    debug("request ended.");
    
    ssize_t ret = flushBuffer(req.c_str(), req.size(), us);
    if (ret < 0) return;
    
    getResponseAndForward(us, ds);
}

void ClientHandler::getResponseAndForward(int us, int ds) {
    lines.clear();
    resetBuffer();
    
    /// process response header first.
    size_t usedDataLen;
    while (fillReadBuffer(us) > 0) {
        usedDataLen = getLines(buffer, dataLen, lines);
        dataLen -= usedDataLen;
        leftShiftData(usedDataLen, dataLen);
        if (lines.size() > 0 && lines.back().size() == 0) break;
    }
    forwardResponseHeader(ds);
    
    /// forward all data.
    size_t total = 0;
    while (fillReadBuffer(us) > 0) {
        total += dataLen;
        if (flushBuffer(buffer, dataLen, ds) <0) break;
        resetBuffer();
    }
}

void ClientHandler::forwardResponseHeader(int fd) {
    std::string cmd = lines.front();
    std::vector<std::string> parts = getPartsFromCmd(cmd);
    if (!parts[0].compare("HTTP/1.1")) parts[0] = "HTTP/1.0";
    cmd = parts[0] + " " + parts[1] + " " + parts[2];
    
    LSS headers = getHeaderList();
    std::string header = writeHeader(headers, cmd);
    debug("response: ");
    debug(header);
    debug("response ended.");

    flushBuffer(header.c_str(), header.size(), fd);
}

void ClientHandler::resetBuffer() {
    memset(buffer, 0, MAX_BUFFER_LEN);
    dataLen = 0;
}

void ClientHandler::leftShiftData(size_t lo, size_t len) {
    memmove(buffer, buffer + lo, len);
    memset(buffer+len, 0, MAX_BUFFER_LEN-len);
    dataLen = len;
}

int ClientHandler::flushBuffer(const char *buffer, ssize_t len, int fd) {
    debug(">>> flushing to fd: " + std::to_string(fd) + " <<<");
    ssize_t ret;
    ssize_t consumedLen = 0;
    while (consumedLen < len) {
        ret = write(fd, buffer + consumedLen, len - consumedLen);
        if (ret < 0) {
            error("flush buffer error.");
            return -1;
        }
        consumedLen += ret;
    }
    assert(len == consumedLen);
    return 0;
}
