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
#include <map>

using VSS = std::vector<std::pair<std::string, std::string>>;
using PSS = std::pair<std::string, std::string>;

void printErrorAndExit(const std::string &msg);
void debug(const std::string &msg);
std::string getServerKey(const std::string &host, unsigned short port);
int getLines(const char *buf, int len, std::vector<std::string> &lines);
PSS splitByColon(const std::string &s);
std::string trim(const std::string &s);

#endif /* Utils_hpp */
