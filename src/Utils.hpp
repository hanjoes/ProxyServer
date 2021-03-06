//
//  Utils.hpp
//  ProxyServer
//
//  Created by Hanzhou Shi on 12/20/15.
//  Copyright © 2015 USF. All rights reserved.
//

#ifndef Utils_hpp
#define Utils_hpp

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>

using LSS = std::list<std::pair<std::string, std::string>>;
using PSS = std::pair<std::string, std::string>;

void debug(const std::string &msg);
void error(const std::string &msg);

void printErrorAndExit(const std::string &msg);
std::string getServerKey(const std::string &host, unsigned short port);
size_t getLines(const char *buf, size_t len, std::vector<std::string> &lines);
PSS pairByColon(const std::string &s);
std::string trim(const std::string &s);
int connectToHost(const std::string &host);
std::vector<std::string> getPartsFromCmd(const std::string &cmd);
std::string getHostFromUrl(const std::string &url);
std::string toUpper(std::string &&s);

#endif /* Utils_hpp */
