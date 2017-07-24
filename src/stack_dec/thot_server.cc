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
 
/********************************************************************/
/*                                                                  */
/* Module: thot_server.cc                                           */
/*                                                                  */
/* Definitions file: thot_server.cc                                 */
/*                                                                  */
/* Description: Implements a translation server                     */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "ThotDecoder.h"
#include <ErrorDefs.h>
#include <StrProcUtils.h>
#include <BasicSocketUtils.h>
#include "thot_server_pars.h"
#include "client_server_defs.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "options.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>

using namespace std;

//--------------- Constants ------------------------------------------

#define BACKLOG                    10     // Maximum number of pending
                                          // connections that can be
                                          // queued

#define DEFAULT_USER_ID             0

//--------------- Function Declarations -------------------------------

int process_request(int s,
                    const thot_server_pars& ts_pars,
                    const ThotDecoderUserPars& tdu_pars,
                    bool &end);
int processParameters(thot_server_pars ts_pars);
int start_server(thot_server_pars ts_pars,
                 ThotDecoderUserPars tdu_pars);
void sigchld_handler(int s);
int handleParameters(int argc,
                     char *argv[],
                     thot_server_pars& pars);
int takeParameters(int argc,
                   const Vector<std::string>& argv_stl,
                   thot_server_pars& ts_pars);
int checkParameters(thot_server_pars& ts_pars);
void printParameters(thot_server_pars ts_pars);
void printUsage(void);
void version(void);

//--------------- Type definitions ------------------------------------


//--------------- Global variables ------------------------------------

ThotDecoder* thotDecoderPtr;
    // NOTE: a pointer is used to avoid early call to class constructor
    // (it is a costly process that otherwise would be executed even if
    // only the help message is to be printed)

//--------------- Function Definitions --------------------------------


//--------------- main function
int main(int argc,char *argv[])
{
  thot_server_pars ts_pars;
    
  if(handleParameters(argc,argv,ts_pars)==THOT_ERROR)
  {
    return THOT_ERROR;
  }
  else
  {
    return processParameters(ts_pars);
  }
}

//--------------- processParameters function
int processParameters(thot_server_pars ts_pars)
{
      // Process configuration file
  ThotDecoderUserPars tdu_pars;
  thotDecoderPtr=new ThotDecoder;
  if(ts_pars.i_given)
  {
    delete thotDecoderPtr;
    return THOT_OK;
  }
  
  int ret=thotDecoderPtr->initUsingCfgFile(ts_pars.c_str,tdu_pars,ts_pars.v_given);
  if(ret==THOT_ERROR)
  {
    delete thotDecoderPtr;
    return THOT_ERROR;
  }

      // Parameters ok
  if(ts_pars.w_given)
  {
        // Print weights
    thotDecoderPtr->printModelWeights();
    thotDecoderPtr->clearTrans();
    delete thotDecoderPtr;
    return THOT_OK;
  }
  else
  {
        // Start server
    int retCode=start_server(ts_pars,tdu_pars);
    delete thotDecoderPtr;
    return retCode;
  }
}

//--------------- start_server function
int start_server(thot_server_pars ts_pars,
                 ThotDecoderUserPars tdu_pars)
{
  int sockfd, new_fd;  // Use sockfd to listen to new connections
                       // through new_fd
  struct sockaddr_in my_addr;    // infomation about server address
  struct sockaddr_in their_addr; // information about client addresses
  int sin_size;
  struct sigaction sa;
  int yes=1;
  bool end=false;
  
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("socket");
    exit(1);
  }

  if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1)
  {
    perror("setsockopt");
    exit(1);
  }
  my_addr.sin_family = AF_INET;          // byte ordering used by the machine
  my_addr.sin_port = htons(ts_pars.server_port); // byte ordering used by the network
  my_addr.sin_addr.s_addr = INADDR_ANY;  // fill with IP address
  memset(&(my_addr.sin_zero), '\0', 8);  // reset the rest of the data structure

  if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr))== -1)
  {
    perror("bind");
    exit(1);
  }

  if (listen(sockfd, BACKLOG) == -1)
  {
    perror("listen");
    exit(1);
  }

  sa.sa_handler = sigchld_handler; // kill inactive processes
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1)
  {
    perror("sigaction");
    exit(1);
  }

  cerr<<"Listening to port "<< ts_pars.server_port <<"..."<<endl;
  
      // main accept() loop
  
  while(!end)
  {  
    sin_size = sizeof(struct sockaddr_in);
    if ((new_fd = accept(sockfd,(struct sockaddr *)&their_addr,(socklen_t *)&sin_size)) == -1)
    {
      perror("accept");
      continue;
    }
    if(ts_pars.v_given)
    {
      cerr<<"----------------------------------------------------"<<endl;
      cerr<<"Server: got connection from "<<inet_ntoa(their_addr.sin_addr)<<endl;
    }

    process_request(new_fd,ts_pars,tdu_pars,end); 
  
//    if (!fork())
//    { // this is the child process
//    close(sockfd); // The child does not need the descriptor
//    if (send(new_fd, "Hello, world!\n", 14, 0) == -1)
//      perror("send");
//      close(new_fd);
//      exit(0);
//    }
    close(new_fd);  
  }
  
  return THOT_OK;
}

//--------------- sigchld_handler function
void sigchld_handler(int /*s*/)
{
  while(wait(NULL) > 0){};
}

//--------------- process_requests function
int process_request(int s,
                    const thot_server_pars& ts_pars,
                    const ThotDecoderUserPars& tdu_pars,
                    bool &end)
{
      // Variable declarations
  std::string stlStr;
  std::string stlStr1;
  std::string stlStr2;
  std::string stlStrSrc;
  std::string stlStrRef;
  bool retVal=false;
  std::string result;
  std::string bestHypInfo;
  std::string catResult;
  Vector<float> floatVec;
  RejectedWordsSet emptyRejWordsSet;
  int verbose=ts_pars.v_given;

      // Get request code
  int server_request_code=BasicSocketUtils::recvInt(s);
  if(verbose) cerr<<"Server: request code "<<server_request_code<<endl;

      // Get user id
  int user_id=BasicSocketUtils::recvInt(s);
  if(verbose) cerr<<"Server: received request from user "<<user_id<<endl;
  
      // Init user parameters
  retVal=thotDecoderPtr->initUserPars(user_id,tdu_pars,verbose);
  if(retVal==THOT_ERROR)
  {
    end=true;
    if(verbose) cerr<<"Server: shutting down"<<endl;
    return THOT_ERROR;
  }

      // Process request
  switch(server_request_code)
  {
    case OL_TRAIN_PAIR:
      BasicSocketUtils::recvStlStr(s,stlStrSrc);
      BasicSocketUtils::recvStlStr(s,stlStrRef);
      retVal=thotDecoderPtr->onlineTrainSentPair(user_id,stlStrSrc.c_str(),stlStrRef.c_str(),verbose);
      BasicSocketUtils::writeInt(s,(int)retVal);
      break;

    case TRAIN_ECM:
      BasicSocketUtils::recvStlStr(s,stlStr1);
      BasicSocketUtils::recvStlStr(s,stlStr2);
      retVal=thotDecoderPtr->trainEcm(user_id,stlStr1.c_str(),stlStr2.c_str(),verbose);
      BasicSocketUtils::writeInt(s,(int)retVal);
      break;

    case TRANSLATE_SENT:
      BasicSocketUtils::recvStlStr(s,stlStr);
      thotDecoderPtr->translateSentence(user_id,stlStr.c_str(),result,bestHypInfo,verbose);
      BasicSocketUtils::writeStr(s,result.c_str());
      BasicSocketUtils::writeStr(s,bestHypInfo.c_str());
      break;

    case TRANSLATE_SENT_HYPINFO:
      BasicSocketUtils::recvStlStr(s,stlStr);
      thotDecoderPtr->translateSentence(user_id,stlStr.c_str(),result,bestHypInfo,verbose);
      BasicSocketUtils::writeStr(s,result.c_str());
      BasicSocketUtils::writeStr(s,bestHypInfo.c_str());
      break;

    case VERIFY_COV:
      BasicSocketUtils::recvStlStr(s,stlStrSrc);
      BasicSocketUtils::recvStlStr(s,stlStrRef);
      retVal=thotDecoderPtr->sentPairVerCov(user_id,stlStrSrc.c_str(),stlStrRef.c_str(),result,verbose);
      BasicSocketUtils::writeStr(s,result.c_str());
      break;

    case START_CAT:
      BasicSocketUtils::recvStlStr(s,stlStr);
      thotDecoderPtr->startCat(user_id,stlStr.c_str(),catResult,verbose);
      BasicSocketUtils::writeStr(s,catResult.c_str());
      break;

    case ADD_STR_TO_PREF:
      BasicSocketUtils::recvStlStr(s,stlStr);
      thotDecoderPtr->addStrToPref(user_id,stlStr.c_str(),emptyRejWordsSet,catResult,verbose);
      BasicSocketUtils::writeStr(s,catResult.c_str());
      break;

    case RESET_PREF:
      thotDecoderPtr->resetPrefix(user_id);
      BasicSocketUtils::writeInt(s,THOT_OK);
      break;
      
    case CLEAR_TRANS:
      thotDecoderPtr->clearTrans(verbose);
      BasicSocketUtils::writeInt(s,(int)retVal);
      break;

    case PRINT_MODELS:
      retVal=thotDecoderPtr->printModels(verbose);
      BasicSocketUtils::writeInt(s,(int)retVal);
      break;

    case END_SERVER: end=true;
      if(verbose) cerr<<"Server: shutting down"<<endl;
      BasicSocketUtils::writeInt(s,1);
      thotDecoderPtr->clearTrans(verbose);
      break;
  }
  return retVal;
}

//--------------- handleParameters function
int handleParameters(int argc,
                     char *argv[],
                     thot_server_pars& ts_pars)
{
  if(argc==1 || readOption(argc,argv,"--version")!=-1)
  {
    version();
    return THOT_ERROR;
  }
  if(readOption(argc,argv,"--help")!=-1)
  {
    printUsage();
    return THOT_ERROR;   
  }
  
  Vector<std::string> argv_stl=argv2argv_stl(argc,argv);
  if(takeParameters(argc,argv_stl,ts_pars)==THOT_ERROR)
  {
    return THOT_ERROR;
  }
  else
  {
    if(checkParameters(ts_pars)==THOT_OK)
    {
      printParameters(ts_pars);
      return THOT_OK;
    }
    else
    {
      return THOT_ERROR;
    }
  }
}

//--------------- takeparameters function
int takeParameters(int argc,
                   const Vector<std::string>& argv_stl,
                   thot_server_pars& ts_pars)
{
  int i=1;
  unsigned int matched;
  
  while(i<argc)
  {
    matched=0;   

        // -i parameter
    if(argv_stl[i]=="-i" && !matched)
    {
      ts_pars.i_given=true;
      ++matched;
    }

        // -c parameter
    if(argv_stl[i]=="-c" && !matched)
    {
      ts_pars.c_given=true;
      if(i==argc-1)
      {
        cerr<<"Error: no value for -c parameter."<<endl;
        return THOT_ERROR;
      }
      else
      {
        ts_pars.c_str=argv_stl[i+1];
        ++matched;
        ++i;
      }
    }

        // -p parameter
    if(argv_stl[i]=="-p" && !matched)
    {
      ts_pars.p_given=true;
      if(i==argc-1)
      {
        cerr<<"Error: no value for -h parameter."<<endl;
        return THOT_ERROR;
      }
      else
      {
        ts_pars.server_port=atoi(argv_stl[i+1].c_str());
        ++matched;
        ++i;
      }
    }

        // -w parameter
    if(argv_stl[i]=="-w" && !matched)
    {
      ts_pars.w_given=true;
      ++matched;
    }

        // -v parameter
    if(argv_stl[i]=="-v" && !matched)
    {
      ts_pars.v_given=true;
      ++matched;
    }

        // Check if current parameter is not valid
    if(matched==0)
    {
      cerr<<"Error: parameter "<<argv_stl[i]<<" not valid."<<endl;
      return THOT_ERROR;
    }
    ++i;
  }
  return THOT_OK;
}

//--------------- checkParameters function
int checkParameters(thot_server_pars& ts_pars)
{
  if(!ts_pars.i_given && !ts_pars.c_given)
  {
    cerr<<"Error: either -c or -i parameter should be given!"<<endl;
    return THOT_ERROR;
  }

  if(ts_pars.i_given && ts_pars.w_given)
  {
    cerr<<"Error: -i and -w parameters cannot be given simultaneously!"<<endl;
    return THOT_ERROR;
  }
  
  return THOT_OK;
}

//--------------- printParameters function
void printParameters(thot_server_pars ts_pars)
{
  cerr<<"Server port: "<<ts_pars.server_port<<endl;
  cerr<<"-w: "<<ts_pars.w_given<<endl;
  cerr<<"-v: "<<ts_pars.v_given<<endl;
}

//--------------- printUsage function
void printUsage(void)
{
  cerr<<"thot_server written by Daniel Ortiz"<<endl;
  cerr<<"Usage: thot_server    -i | -c <string>"<<endl;
  cerr<<"                      [-p <int>] [ -w ] [ -v ] [--help] [--version]"<<endl;
  cerr<<endl;  
  cerr<<"-i <string>    Test server initialization and exit"<<endl<<endl;  
  cerr<<"-c <string>    Configuration file"<<endl<<endl;  
  cerr<<"-p <int>       Port used by the server"<<endl<<endl;  
  cerr<<"-w             Print model weights and exit"<<endl<<endl;  
  cerr<<"-v             Verbose mode"<<endl<<endl;
  cerr<<"--help         Print this help and exit"<<endl<<endl;
  cerr<<"--version      Output version information and exit"<<endl<<endl;
}

//--------------- version function
void version(void)
{
  cerr<<"thot_server is part of the thot package "<<endl;
  cerr<<"thot version "<<THOT_VERSION<<endl;
  cerr<<"thot is GNU software written by Daniel Ortiz"<<endl;
}
