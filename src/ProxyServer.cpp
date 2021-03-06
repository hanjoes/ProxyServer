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

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

ProxyServer::ProxyServer(const std::string &host, int port, bool mul)
: host(host), port(port), multiThreaded(mul)
{
    eventList = new KEVENT[MAX_EVENT_NUM];
    pthread_attr_init(&threadAttr);
    pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED);
}

ProxyServer::~ProxyServer() {
    pthread_attr_destroy(&threadAttr);
    delete[] eventList;
}

void ProxyServer::start() {
    /// initialize kqueue
    int kfd = kqueue();
    
    KEVENT listenEvent;
    initListenEvent(&listenEvent);
    int ret = kevent(kfd, &listenEvent, 1, nullptr, 0, nullptr);
    if (ret < 0) printErrorAndExit("add event failed.");
    
    debug(">> entering main loop...");
    while (true) {
        ret = kevent(kfd, nullptr, 0, eventList, MAX_EVENT_NUM, nullptr);
        /// no timeout so if less than 0 then there is error.
        if (ret <= 0) printErrorAndExit("error occurred when waiting.");
        /// handles event one by one.
        for (int eventIndex = 0; eventIndex < ret; ++eventIndex) {
            KEVENT *pCurrentEvent = &eventList[eventIndex];
            int eventIdent = (int)pCurrentEvent->ident;
            
            if (eventIdent == listenEvent.ident) {
                int cfd = acceptClient((int)listenEvent.ident);
                registerEventsForNewClient(kfd, cfd);
            }
            else if (pCurrentEvent->flags & EV_EOF) {
                clearClientData(kfd, eventIdent);
            }
            else if (pCurrentEvent->filter == EVFILT_READ) {
                auto iter = clientsMap.find(eventIdent);
                if (iter != clientsMap.end()) {
                    iter->second->processRequest(eventIdent);
                }
            }
            else if (pCurrentEvent->filter == EVFILT_WRITE) {
                /// a client socket can only be written after all data
                /// has been read.
                
                /// iterate through all client handler and find dispatchable
                /// clients and launch threads
                auto iter = clientsMap.begin();
                for (; iter != clientsMap.end(); ++iter) {
                    if (iter->second->canDispatch()) {
                        dispatch(iter->second.get(), eventIdent);
                    }
                }
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
    debug(">> listening on " + host + ":" + std::to_string(port));

    EV_SET(ev, fd, EVFILT_READ, EV_ADD, 0, 80, nullptr);
}

int ProxyServer::acceptClient(int listenFd) {
    struct sockaddr addr;
    socklen_t socklen;
    int fd = accept(listenFd, &addr, &socklen);
    if (fd < 0) printErrorAndExit("error when accepting connection.");
    struct sockaddr_in *inAddr = (struct sockaddr_in *)&addr;
    std::string clientHost = inet_ntoa(inAddr->sin_addr);
    unsigned short clientPort = ntohs(inAddr->sin_port);
    clientsMap[fd] = UPTRCH(new ClientHandler(clientHost, clientPort));
    debug(">> added client: " + clientHost + ":" + std::to_string(clientPort));
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

void ProxyServer::dispatch(ClientHandler *client, int fd) {
    if (client != nullptr) client->setDispatched(true);
    
    auto args = new ClientHandlerArgs();
    args->fd = fd;
    args->clientHandler = client;
    
    if (!multiThreaded) {
        debug("single thread.");
        ClientHandler::run(args);
        return;
    }
    pthread_t pid;
    pthread_create(&pid, &threadAttr, ClientHandler::run, args);
}
