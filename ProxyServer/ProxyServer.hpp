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

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

using KEVENT = struct kevent;

const static int MAX_EVENT_NUM = 128;

class ProxyServer {
public:
    ProxyServer(const std::string &host, int port);
    
    ~ProxyServer();
    
    void start();
    
protected:
    void initListenEvent(KEVENT *ev);
    int acceptClient(int listenFd, const KEVENT *ev);
    void registerEventsForNewClient(int kfd, int cfd);
    void clearClientData(int kfd, int cfd);
    
protected:
    KEVENT *eventList;
    std::unordered_map<int, std::string> clientsMap;
    const std::string &host;
    int port;
};

#endif /* ProxyServer_hpp */
