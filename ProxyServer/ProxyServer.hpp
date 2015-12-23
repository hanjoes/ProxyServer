//
//  ProxyServer.hpp
//  ProxyServer
//
//  Created by Hanzhou Shi on 12/16/15.
//  Copyright © 2015 USF. All rights reserved.
//

#ifndef ProxyServer_hpp
#define ProxyServer_hpp

#include <cstdio>
#include <string>
#include <unordered_map>
#include <memory>

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <pthread.h> 

#include "ClientHandler.hpp"

using KEVENT = struct kevent;
using UPTRCH = std::unique_ptr<ClientHandler>;

const static int MAX_EVENT_NUM = 128;

class ProxyServer {
public:
    ProxyServer(const std::string &host, int port, bool mul = true);
    
    ~ProxyServer();
    
    void start();
    
protected:
    void initListenEvent(KEVENT *ev);
    
    int acceptClient(int listenFd);
    
    void registerEventsForNewClient(int kfd, int cfd);
    
    void clearClientData(int kfd, int cfd);
    
    void dispatch(ClientHandler *client, int fd);
    
protected:
    KEVENT *eventList;
    std::unordered_map<int, UPTRCH> clientsMap;
    const std::string &host;
    int port;
    bool multiThreaded;
    pthread_attr_t threadAttr;
    void *args[2];
};

#endif /* ProxyServer_hpp */
