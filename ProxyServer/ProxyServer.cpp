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
        /// no timeout so if less than 0 then there is error.
        if (ret <= 0) printErrorAndExit("error occurred when waiting.");
        /// handles event one by one.
        for (int eventIndex = 0; eventIndex < ret; ++eventIndex) {
            KEVENT *pCurrentEvent = &eventList[eventIndex];
            if (pCurrentEvent->ident == listenEvent.ident) {
                int cfd = acceptClient((int)listenEvent.ident, pCurrentEvent);
                registerEventsForNewClient(kfd, cfd);
            }
            else if (pCurrentEvent->flags & EV_EOF) {
                debug("connection closed with peer: " + clientsMap[(int)pCurrentEvent->ident]);
                clearClientData(kfd, (int)pCurrentEvent->ident);
            }
            else if (pCurrentEvent->filter == EVFILT_READ) {
            }
            else if (pCurrentEvent->filter == EVFILT_WRITE) {
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
    
    ret = listen(fd, 80);
    if (ret < 0) printErrorAndExit("listen failed.");
    debug("listening on " + host + ":" + std::to_string(port));

    EV_SET(ev, fd, EVFILT_READ, EV_ADD, 0, 80, nullptr);
}

int ProxyServer::acceptClient(int listenFd, const KEVENT *ev) {
    struct sockaddr addr;
    socklen_t socklen;
    int fd = accept(listenFd, &addr, &socklen);
    if (fd < 0) printErrorAndExit("error when accepting connection.");
    struct sockaddr_in *inAddr = (struct sockaddr_in *)&addr;
    std::string clientHost = inet_ntoa(inAddr->sin_addr);
    unsigned short clientPort = ntohs(inAddr->sin_port);
    clientsMap[fd] = getServerKey(clientHost, clientPort);
    debug("added client: " + clientsMap[fd]);
    return fd;
}

void ProxyServer::registerEventsForNewClient(int kfd, int cfd) {
    static KEVENT tmpEvent;
    
    /// register read and write events
    memset(&tmpEvent, 0, sizeof(tmpEvent));
    EV_SET(&tmpEvent, cfd, EVFILT_READ, EV_ADD, 0, 0, nullptr);
    int ret = kevent(kfd, &tmpEvent, 1, nullptr, 0, nullptr);
    if (ret < 0) printErrorAndExit("set event for read failed.");
    
    memset(&tmpEvent, 0, sizeof(tmpEvent));
    EV_SET(&tmpEvent, cfd, EVFILT_WRITE, EV_ADD, 0, 0, nullptr);
    ret = kevent(kfd, &tmpEvent, 1, nullptr, 0, nullptr);
    if (ret < 0) printErrorAndExit("set event for write failed.");
}

void ProxyServer::clearClientData(int kfd, int cfd) {
    clientsMap.erase(cfd);
    /// close will trigger EV_DELETE on the events
    /// related to the fd.
    if (cfd >= 0) close(cfd);
}