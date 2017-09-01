/*
thot package for statistical machine translation
Copyright (C) 2013 Daniel Ortiz-Mart\'inez
 
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public License
as published by the Free Software Foundation; either version 3
of the License, or (at your option) any later version.
 
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.
 
You should have received a copy of the GNU Lesser General Public License
along with this program; If not, see <http://www.gnu.org/licenses/>.
*/
 
/**
 * @file BasicSocketUtils.h
 * @brief Defines network utilities
 */

#ifndef _BasicSocketUtils_h
#define _BasicSocketUtils_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <iostream>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <string>
#include <vector>
#include <ErrorDefs.h>
#include <mem_alloc_utils.h>

#ifdef THOT_MINGW
# include <winsock2.h>
#else
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
/* netbd.h contains the declaration of the hostent struct */

//--------------- Constants ------------------------------------------

#define NETWORK_BUFF_SIZE 512

//---------------

namespace BasicSocketUtils
{
      // Basic socket functions
  int init(void);
  int recvStr(int s,char *str);
  int recvStlStr(int s,std::string& stlstr);
  int recvInt(int s);
  int writeInt(int fd,int i);
  int writeStr(int fd,const char* s);
  int connect(const char *dirServ,
              unsigned int port,
              int& fileDesc);
  void clean(void);
}

#endif
