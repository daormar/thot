/*
thot package for statistical machine translation
Copyright (C) 2013-2017 Daniel Ortiz-Mart\'inez, Adam Harasimowicz
 
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

#include "StdCerrThreadSafePrint.h"
#include "StdCerrThreadSafeTidPrint.h"
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
#include <set>
#include "options.h"
#include "ctimer.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>

//--------------- Constants ------------------------------------------

#define BACKLOG                    10     // Maximum number of pending
                                          // connections that can be
                                          // queued

#define DEFAULT_USER_ID             0

//--------------- Type definitions ------------------------------------

struct request_data
{
  int sockd;
  struct in_addr sin_addr;
};

//--------------- Function Declarations -------------------------------

void* process_request(void* rdata_ptr);
int process_request_switch(int sockd,
                           int user_id,
                           int server_request_code,
                           int verbose);
int processParameters(void);
int start_server(void);
bool read_end_server_var(void);
void set_end_server_var(void);
void increase_num_threads_var(void);
void decrease_num_threads_var(void);
void wait_on_num_threads_var_cond(void);
void sigchld_handler(int s);
int handleParameters(int argc,
                     char *argv[]);
int takeParameters(int argc,
                   const std::vector<std::string>& argv_stl);
int checkParameters(void);
void printParameters(void);
void printUsage(void);
void version(void);

//--------------- Global variables ------------------------------------

thot_server_pars ts_pars;
ThotDecoderUserPars tdu_pars;

ThotDecoder* thotDecoderPtr;
    // NOTE: a pointer is used to avoid early call to class constructor
    // (it is a costly process that otherwise would be executed even if
    // only the help message is to be printed)

    // Mutexes and conditions
pthread_mutex_t num_threads_var_mut;
pthread_cond_t num_threads_var_cond;
int num_threads;
pthread_mutex_t end_server_var_mut;
bool end;

//--------------- Function Definitions --------------------------------


//---------------
int main(int argc,char *argv[])
{ 
  // Ignore broken pipe signal to do not close server
  signal(SIGPIPE, SIG_IGN);
     
  if(handleParameters(argc,argv)==THOT_ERROR)
  {
    return THOT_ERROR;
  }
  else
  {
    return processParameters();
  }
}

//---------------
int processParameters(void)
{
      // Process configuration file
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
    delete thotDecoderPtr;
    return THOT_OK;
  }
  else
  {
        // Start server
    int retCode=start_server();
    delete thotDecoderPtr;
    return retCode;
  }
}

//---------------
int start_server(void)
{
  int sockfd, new_fd;  // Use sockfd to listen to new connections
                       // through new_fd
  struct sockaddr_in my_addr;    // infomation about server address
  struct sockaddr_in their_addr; // information about client addresses
  int sin_size;
  struct sigaction sa;
  int yes=1;
  
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    std::cerr<<"socket error"<<std::endl;
    exit(1);
  }

  if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1)
  {
    std::cerr<<"setsockopt error"<<std::endl;
    exit(1);
  }
  my_addr.sin_family = AF_INET;          // byte ordering used by the machine
  my_addr.sin_port = htons(ts_pars.server_port); // byte ordering used by the network
  my_addr.sin_addr.s_addr = INADDR_ANY;  // fill with IP address
  memset(&(my_addr.sin_zero), '\0', 8);  // reset the rest of the data structure

  if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr))== -1)
  {
    std::cerr<<"bind error"<<std::endl;
    exit(1);
  }

  if (listen(sockfd, BACKLOG) == -1)
  {
    std::cerr<<"listen error"<<std::endl;
    exit(1);
  }

  sa.sa_handler = sigchld_handler; // kill inactive processes
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1)
  {
    std::cerr<<"sigaction error"<<std::endl;
    exit(1);
  }

      // Initialize mutexes and conditions
  num_threads=0;
  pthread_mutex_init(&num_threads_var_mut,NULL);
  pthread_cond_init(&num_threads_var_cond,NULL);
  pthread_mutex_init(&end_server_var_mut,NULL);

  std::cerr<<"Listening to port "<< ts_pars.server_port <<"..."<<std::endl;
  
      // main accept() loop
  end=false;
  while(!read_end_server_var())
  {  
    sin_size = sizeof(struct sockaddr_in);
    if ((new_fd = accept(sockfd,(struct sockaddr *)&their_addr,(socklen_t *)&sin_size)) == -1)
    {
      std::cerr<<"accept error"<<std::endl;
      continue;
    }

        // Prepare request data
    request_data rdata;
    rdata.sockd=new_fd;
    rdata.sin_addr=their_addr.sin_addr;

        // Process request
    pthread_t      tid;  // thread ID
    pthread_attr_t attr; // thread attribute

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&tid, &attr, process_request, (void*) &rdata);
  }

  if(ts_pars.v_given)
    StdCerrThreadSafe<<"Server: shutting down"<<std::endl;

      // Wait for threads to finish
  wait_on_num_threads_var_cond();
  
      // Destroy mutexes and conditions
  pthread_mutex_destroy(&num_threads_var_mut);
  pthread_cond_destroy(&num_threads_var_cond);
  pthread_mutex_destroy(&end_server_var_mut);
  pthread_mutex_destroy(&user_set_mut);

      // Wait for threads to finish
  wait_on_num_threads_var_cond();
  
      // Destroy mutexes and conditions
  pthread_mutex_destroy(&num_threads_var_mut);
  pthread_cond_destroy(&num_threads_var_cond);
  pthread_mutex_destroy(&end_server_var_mut);

  return THOT_OK;
}

//---------------
void sigchld_handler(int /*s*/)
{
  while(wait(NULL) > 0){};
}

//---------------
bool read_end_server_var(void)
{
  pthread_mutex_lock(&end_server_var_mut);
  /////////// begin of mutex
  bool result=end;
  /////////// end of mutex 
  pthread_mutex_unlock(&end_server_var_mut);

  return result;
}

//---------------
void set_end_server_var(void)
{
  pthread_mutex_lock(&end_server_var_mut);
  /////////// begin of mutex
  end=true;
  /////////// end of mutex 
  pthread_mutex_unlock(&end_server_var_mut);
}

//---------------
void increase_num_threads_var(void)
{
  pthread_mutex_lock(&num_threads_var_mut);
  /////////// begin of mutex
  ++num_threads;
  /////////// end of mutex 
  pthread_mutex_unlock(&num_threads_var_mut);
}

//---------------
void decrease_num_threads_var(void)
{
  pthread_mutex_lock(&num_threads_var_mut);
  /////////// begin of mutex
  --num_threads;

      // Restart threads waiting on condition
  if(num_threads==0)
    pthread_cond_broadcast(&num_threads_var_cond);
      // The pthread_cond_broadcast() function shall unblock all threads
      // currently blocked on the specified condition variable.

  /////////// end of mutex 
  pthread_mutex_unlock(&num_threads_var_mut);
}

//---------------
void wait_on_num_threads_var_cond(void)
{
  pthread_mutex_lock(&num_threads_var_mut);
  /////////// begin of mutex
  while(num_threads>0)
  {
    pthread_cond_wait(&num_threads_var_cond,&num_threads_var_mut);
        // The pthread_cond_wait() function is used to block on a
        // condition variable. It is called with mutex locked by the
        // calling thread or undefined behaviour will result.

        // This function atomically release mutex and cause the calling
        // thread to block on the condition variable

        // Upon successful return, the mutex has been locked and is
        // owned by the calling thread.
  }

      // NOTE: num_threads_var_mut mutex is intentionally kept locked at
      // the end of this function, preventing execution of new server
      // request threads
}

//---------------
void* process_request(void* rdata_ptr)
{
  increase_num_threads_var();
    
      // Initialize variables
  request_data rdata=*(request_data*) rdata_ptr;
  int verbose=ts_pars.v_given;

  if(verbose)
  {
        // Current date/time based on current system
    time_t now = time(0);
        // Convert now to tm struct for local timezone
    tm* localtm = localtime(&now);
    std::cerr<<"----------------------------------------------------"<<std::endl;
    std::cerr<<"Processing new request..."<<std::endl;
    std::cerr<<"Current time: "<<asctime(localtm);
    std::cerr<<"Origin: "<<inet_ntoa(rdata.sin_addr)<<std::endl;
  }

      // Get request code
  int server_request_code=BasicSocketUtils::recvInt(rdata.sockd);
  if(verbose) std::cerr<<"Request code: "<<server_request_code<<std::endl;

      // Get user id
  int user_id=BasicSocketUtils::recvInt(rdata.sockd);
  if(verbose) std::cerr<<"User identifier: "<<user_id<<std::endl;
  
      // Init user parameters
  int ret=thotDecoderPtr->initUserPars(user_id,tdu_pars,verbose);
  if(ret==THOT_ERROR)
  {
    // end=true;
    if(verbose) std::cerr<<"Error while initializing server parameters"<<std::endl;
    close(rdata.sockd);
    decrease_num_threads_var();
    pthread_exit(NULL);
  }

      // Process request measuring time
  double elapsed_prev,elapsed,ucpu,scpu;
  ctimer(&elapsed_prev,&ucpu,&scpu);

  ret=process_request_switch(rdata.sockd,user_id,server_request_code,verbose);
  
  ctimer(&elapsed,&ucpu,&scpu);
  
  if(verbose)
  {
    std::cerr<<"Elapsed time: " << elapsed-elapsed_prev << " secs\n";
  }
  if(ret==THOT_ERROR)
  {
    if(verbose) std::cerr<<"Error while processing client request"<<std::endl;
    close(rdata.sockd);
    decrease_num_threads_var();
    pthread_exit(NULL);
  }

  close(rdata.sockd);
  decrease_num_threads_var();
  pthread_exit(NULL);
}

//---------------
int process_request_switch(int sockd,
                           int user_id,
                           int server_request_code,
                           int verbose)
{
  std::string stlStr;
  std::string stlStr1;
  std::string stlStr2;
  std::string stlStrSrc;
  std::string stlStrRef;
  std::string result;
  std::string bestHypInfo;
  std::string catResult;
  std::vector<float> floatVec;
  RejectedWordsSet emptyRejWordsSet;
  int ret=THOT_OK;
  
  switch(server_request_code)
  {
    case OL_TRAIN_PAIR:
      BasicSocketUtils::recvStlStr(sockd,stlStrSrc);
      BasicSocketUtils::recvStlStr(sockd,stlStrRef);
      ret=thotDecoderPtr->onlineTrainSentPair(user_id,stlStrSrc.c_str(),stlStrRef.c_str(),verbose);
      BasicSocketUtils::writeInt(sockd,ret);
      break;

    case TRAIN_ECM:
      BasicSocketUtils::recvStlStr(sockd,stlStr1);
      BasicSocketUtils::recvStlStr(sockd,stlStr2);
      ret=thotDecoderPtr->trainEcm(user_id,stlStr1.c_str(),stlStr2.c_str(),verbose);
      BasicSocketUtils::writeInt(sockd,ret);
      break;

    case TRANSLATE_SENT:
      BasicSocketUtils::recvStlStr(sockd,stlStr);
      thotDecoderPtr->translateSentence(user_id,stlStr.c_str(),result,bestHypInfo,verbose);
      BasicSocketUtils::writeStr(sockd,result.c_str());
      BasicSocketUtils::writeStr(sockd,bestHypInfo.c_str());
      break;

    case TRANSLATE_SENT_HYPINFO:
      BasicSocketUtils::recvStlStr(sockd,stlStr);
      thotDecoderPtr->translateSentence(user_id,stlStr.c_str(),result,bestHypInfo,verbose);
      BasicSocketUtils::writeStr(sockd,result.c_str());
      BasicSocketUtils::writeStr(sockd,bestHypInfo.c_str());
      break;

    case VERIFY_COV:
      BasicSocketUtils::recvStlStr(sockd,stlStrSrc);
      BasicSocketUtils::recvStlStr(sockd,stlStrRef);
      ret=thotDecoderPtr->sentPairVerCov(user_id,stlStrSrc.c_str(),stlStrRef.c_str(),result,verbose);
      BasicSocketUtils::writeStr(sockd,result.c_str());
      break;

    case START_CAT:
      BasicSocketUtils::recvStlStr(sockd,stlStr);
      thotDecoderPtr->startCat(user_id,stlStr.c_str(),catResult,verbose);
      BasicSocketUtils::writeStr(sockd,catResult.c_str());
      break;

    case ADD_STR_TO_PREF:
      BasicSocketUtils::recvStlStr(sockd,stlStr);
      thotDecoderPtr->addStrToPref(user_id,stlStr.c_str(),emptyRejWordsSet,catResult,verbose);
      BasicSocketUtils::writeStr(sockd,catResult.c_str());
      break;

    case RESET_PREF:
      thotDecoderPtr->resetPrefix(user_id);
      BasicSocketUtils::writeInt(sockd,THOT_OK);
      break;
      
    case CLEAR_TRANS:
      thotDecoderPtr->clearTrans(verbose);
      BasicSocketUtils::writeInt(sockd,THOT_OK);
      break;

    case PRINT_MODELS:
      ret=thotDecoderPtr->printModels(verbose);
      BasicSocketUtils::writeInt(sockd,ret);
      break;

    case END_SERVER: set_end_server_var();
      BasicSocketUtils::writeInt(sockd,1);
      thotDecoderPtr->clearTrans(verbose);
      break;
  }

  return ret;
}

//---------------
int handleParameters(int argc,
                     char *argv[])
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
  
  std::vector<std::string> argv_stl=argv2argv_stl(argc,argv);
  if(takeParameters(argc,argv_stl)==THOT_ERROR)
  {
    return THOT_ERROR;
  }
  else
  {
    if(checkParameters()==THOT_OK)
    {
      printParameters();
      return THOT_OK;
    }
    else
    {
      return THOT_ERROR;
    }
  }
}

//---------------
int takeParameters(int argc,
                   const std::vector<std::string>& argv_stl)
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
        std::cerr<<"Error: no value for -c parameter."<<std::endl;
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
        std::cerr<<"Error: no value for -h parameter."<<std::endl;
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
      std::cerr<<"Error: parameter "<<argv_stl[i]<<" not valid."<<std::endl;
      return THOT_ERROR;
    }
    ++i;
  }
  return THOT_OK;
}

//---------------
int checkParameters(void)
{
  if(!ts_pars.i_given && !ts_pars.c_given)
  {
    std::cerr<<"Error: either -c or -i parameter should be given!"<<std::endl;
    return THOT_ERROR;
  }

  if(ts_pars.i_given && ts_pars.w_given)
  {
    std::cerr<<"Error: -i and -w parameters cannot be given simultaneously!"<<std::endl;
    return THOT_ERROR;
  }
  
  return THOT_OK;
}

//---------------
void printParameters(void)
{
  std::cerr<<"Server port: "<<ts_pars.server_port<<std::endl;
  std::cerr<<"-w: "<<ts_pars.w_given<<std::endl;
  std::cerr<<"-v: "<<ts_pars.v_given<<std::endl;
}

//---------------
void printUsage(void)
{
  std::cerr<<"Usage: thot_server    -i | -c <string>"<<std::endl;
  std::cerr<<"                      [-p <int>] [ -w ] [ -v ] [--help] [--version]"<<std::endl;
  std::cerr<<std::endl;
  std::cerr<<"-i <string>    Test server initialization and exit"<<std::endl<<std::endl;
  std::cerr<<"-c <string>    Configuration file"<<std::endl<<std::endl;
  std::cerr<<"-p <int>       Port used by the server"<<std::endl<<std::endl;
  std::cerr<<"-w             Print model weights and exit"<<std::endl<<std::endl;
  std::cerr<<"-v             Verbose mode"<<std::endl<<std::endl;
  std::cerr<<"--help         Print this help and exit"<<std::endl<<std::endl;
  std::cerr<<"--version      Output version information and exit"<<std::endl<<std::endl;
}

//---------------
void version(void)
{
  std::cerr<<"thot_server is part of the thot package "<<std::endl;
  std::cerr<<"thot version "<<THOT_VERSION<<std::endl;
  std::cerr<<"thot is GNU software written by Daniel Ortiz"<<std::endl;
}
