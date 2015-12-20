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

void printErrorAndExit(const std::string &msg);
void debug(const std::string &msg);
std::string getServerKey(const std::string &host, unsigned short port);

#endif /* Utils_hpp */
