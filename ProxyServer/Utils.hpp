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

void printErrorAndExit(const std::string &msg);
void debug(const std::string &msg);
std::string getServerKey(const std::string &host, unsigned short port);
int getLines(const char *buf, int len, std::vector<std::string> &lines);
PSS splitByColon(const std::string &s);
std::string trim(const std::string &s);
std::string getIpFromHost(const std::string &host);
std::vector<std::string> getPartsFromCmd(const std::string &cmd);
std::string getHostFromUrl(const std::string &url);

#endif /* Utils_hpp */
