//
//  ProxyServer.cpp
//  ProxyServer
//
//  Created by Hanzhou Shi on 12/16/15.
//  Copyright © 2015 USF. All rights reserved.
//

#include <iostream>
#include <vector>

#include "ProxyServer.hpp"
#include "Utils.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

ProxyServer::ProxyServer(const std::string &host, int port)
: host(host), port(port)
{
    eventList = new KEVENT[MAX_EVENT_NUM];
}

ProxyServer::~ProxyServer() {
    delete[] eventList;
}

void ProxyServer::start() {
    /// initialize kqueue
    int kfd = kqueue();
    
    KEVENT listenEvent;
    initListenEvent(&listenEvent);
    int ret = kevent(kfd, &listenEvent, 1, nullptr, 0, nullptr);
    if (ret < 0) printErrorAndExit("add event failed.");
    
    debug("entering main loop...");
    while (true) {
        ret = kevent(kfd, nullptr, 0, eventList, MAX_EVENT_NUM, nullptr);
        debug("getting an event.");
        /// no timeout so if less than 0 then there is error.
        if (ret <= 0) printErrorAndExit("error occurred when waiting.");
        /// handles event one by one.
        for (int eventIndex = 0; eventIndex < ret; ++eventIndex) {
            KEVENT *pCurrentEvent = &eventList[eventIndex];
            if (pCurrentEvent->ident == listenEvent.ident) {
                acceptClient((int)listenEvent.ident, pCurrentEvent);
            }
            else if (pCurrentEvent->filter == EVFILT_READ) {
            }
            else if (pCurrentEvent->filter == EVFILT_WRITE) {
                debug("event write " + clientsMap[(int)pCurrentEvent->ident]);
            }
        }
    }
}

void ProxyServer::initListenEvent(KEVENT *ev) {
    int fd = socket(PF_INET, SOCK_STREAM, 0);
    
    struct sockaddr_in sin;
    sin.sin_len = sizeof(struct sockaddr_in);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = inet_addr(host.c_str());
    int ret = bind(fd, (struct sockaddr *)&sin, sizeof(sin));
    if (ret < 0) printErrorAndExit("bind failed.");
    /// according to documentation, the backlog is silently limited
    /// to 128.
    ret = listen(fd, 80);
    debug("listening on " + host + ":" + std::to_string(port));
    if (ret < 0) printErrorAndExit("listen failed.");
    EV_SET(ev, fd, EVFILT_READ, EV_ADD, 0, 80, nullptr);
}

void ProxyServer::acceptClient(int listenFd, const KEVENT *ev) {
    debug("accepting connection");
    static struct sockaddr addr;
    static socklen_t socklen;
    int fd = accept(listenFd, &addr, &socklen);
    if (fd < 0) printErrorAndExit("error when accepting connection.");
    struct sockaddr_in *inAddr = (struct sockaddr_in *)&addr;
    std::string clientHost = inet_ntoa(inAddr->sin_addr);
    clientsMap[fd] = getServerKey(clientHost, inAddr->sin_port);
}
