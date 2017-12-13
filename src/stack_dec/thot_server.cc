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

#define BACKLOG                   100     // Maximum number of pending
                                          // connections that can be
                                          // queued

#define DEFAULT_USER_ID             0

//--------------- Type definitions ------------------------------------

struct request_data
{
  int sockd;
  struct in_addr sin_addr;
  int request_type;
  int user_id;
};

//--------------- Function Declarations -------------------------------

int processParameters(void);
int start_server(void);
int get_request_type(int sockd,
                   int& request_type);
int get_user_id(int sockd,
                int& user_id);
void* process_request(void* rdata_ptr);
void process_request_switch(int sockd,
                            int user_id,
                            int server_request_type,
                            int verbose);
int init_user_pars_if_required(int user_id);
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
std::set<int> user_set;

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
  
      // Parameters ok
  if(ts_pars.w_given)
  {
    int ret=thotDecoderPtr->initUsingCfgFile(ts_pars.c_str,tdu_pars,ts_pars.v_given);
    if(ret==THOT_ERROR)
    {
      delete thotDecoderPtr;
      return THOT_ERROR;
    }
        // NOTE: user pars are initialized so as to correctly set
        // computer assisted translation weights
    ret=thotDecoderPtr->initUserPars(DEFAULT_USER_ID,tdu_pars,ts_pars.v_given);
    if(ret==THOT_ERROR)
    {
      delete thotDecoderPtr;
      return THOT_ERROR;
    }
        // Print weights
    thotDecoderPtr->printModelWeights();
    delete thotDecoderPtr;
    return THOT_OK;
  }
  else
  {
    if(ts_pars.t_given)
    {
      int ret=thotDecoderPtr->testSoftwareModulesInModelDescriptors(ts_pars.c_str,ts_pars.v_given);
      if(ret==THOT_ERROR)
      {
        delete thotDecoderPtr;
        return THOT_ERROR;
      }
      delete thotDecoderPtr;
      return THOT_OK;
    }
    else
    {
      int ret=thotDecoderPtr->initUsingCfgFile(ts_pars.c_str,tdu_pars,ts_pars.v_given);
      if(ret==THOT_ERROR)
      {
        delete thotDecoderPtr;
        return THOT_ERROR;
      }
          // Start server
      int retCode=start_server();
      delete thotDecoderPtr;
      return retCode;
    }
  }
}

//---------------
int start_server(void)
{
  int sockfd;  // Use sockfd to listen to new connections
  struct sockaddr_in my_addr;    // infomation about server address
  struct sigaction sa;
  int yes=1;

      // Create socket
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    StdCerrThreadSafe<<"socket error"<<std::endl;
    exit(1);
  }

      // Set socket options
  if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1)
  {
    StdCerrThreadSafe<<"setsockopt error"<<std::endl;
    exit(1);
  }
  my_addr.sin_family = AF_INET;          // byte ordering used by the machine
  my_addr.sin_port = htons(ts_pars.server_port); // byte ordering used by the network
  my_addr.sin_addr.s_addr = INADDR_ANY;  // fill with IP address
  memset(&(my_addr.sin_zero), '\0', 8);  // reset the rest of the data structure

      // Assign address to socket
  if (::bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr))== -1)
  {
    StdCerrThreadSafe<<"bind error"<<std::endl;
    exit(1);
  }

      // Start listening
  if (listen(sockfd, BACKLOG) == -1)
  {
    StdCerrThreadSafe<<"listen error"<<std::endl;
    exit(1);
  }

  sa.sa_handler = sigchld_handler; // kill inactive processes
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1)
  {
    StdCerrThreadSafe<<"sigaction error"<<std::endl;
    exit(1);
  }

      // Initialize mutexes and conditions
  num_threads=0;
  pthread_mutex_init(&num_threads_var_mut,NULL);
  pthread_cond_init(&num_threads_var_cond,NULL);

  StdCerrThreadSafe<<"Listening to port "<< ts_pars.server_port <<"..."<<std::endl;
  
      // main accept() loop
  bool end_server=false;
  while(!end_server)
  {
        // accept connection
    struct sockaddr_in their_addr; // information about client addresses
    int sin_size = sizeof(struct sockaddr_in);
    int new_fd;
    if ((new_fd = accept(sockfd,(struct sockaddr *)&their_addr,(socklen_t *)&sin_size)) == -1)
    {
      StdCerrThreadSafe<<"accept error"<<std::endl;
      continue;
    }

        // Obtain request type
    int request_type;
    int ret=get_request_type(new_fd,request_type);
    if(ret==THOT_ERROR)
    {
      StdCerrThreadSafe<<"Error while obtaining request type"<<std::endl;
      close(new_fd);
      continue;
    }

        // Check if server should be finished
    if(request_type==END_SERVER)
      end_server=true;
      
        // Obtain user identifier
    int user_id;
    ret=get_user_id(new_fd,user_id);
    if(ret==THOT_ERROR)
    {
      StdCerrThreadSafe<<"Error while obtaining user identifier"<<std::endl;
      close(new_fd);
      continue;
    }

        // Init user parameters if required
    ret=init_user_pars_if_required(user_id);
    if(ret==THOT_ERROR)
    {
      StdCerrThreadSafe<<"Error while initializing server parameters"<<std::endl;
      close(new_fd);
      continue;
    }
    
        // Prepare request data (memory is released by thread provided
        // it is sucessfully created)
    request_data* rdata_ptr=new request_data;
    rdata_ptr->sockd=new_fd;
    rdata_ptr->sin_addr=their_addr.sin_addr;
    rdata_ptr->request_type=request_type;
    rdata_ptr->user_id=user_id;
    
        // Process request
    pthread_t      tid;  // thread ID
    pthread_attr_t attr; // thread attribute

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    int thread_err=pthread_create(&tid, &attr, process_request, (void*) rdata_ptr);
    if(thread_err>0)
    {
          // Release memory since thread was not created
      delete rdata_ptr;
      
      StdCerrThreadSafe<<"Warning: call to pthread_create failed"<<std::endl;
    }
    else
    {
          // Thread created successfully

          // Increase variable with number of threads being executed
          // NOTE: this cannot be done inside process_request function.
          // Otherwise, when waiting for threads to finish during shutting
          // down, some threads may become blocked
      increase_num_threads_var();
    }
  }

      // Wait for threads to finish
  wait_on_num_threads_var_cond();

  if(ts_pars.v_given || ts_pars.vd_given)
    StdCerrThreadSafe<<"Server: shutting down"<<std::endl;

      // Destroy mutexes and conditions
  pthread_mutex_destroy(&num_threads_var_mut);
  pthread_cond_destroy(&num_threads_var_cond);

  return THOT_OK;
}

//---------------
void sigchld_handler(int /*s*/)
{
  while(wait(NULL) > 0){};
}

//---------------
int get_request_type(int sockd,
                   int& request_type)
{
  try
  {
    request_type=BasicSocketUtils::recvInt(sockd);
  }
  catch(const std::exception& e)
  {
    return THOT_ERROR;
  }
  
  return THOT_OK;
}

//---------------
int get_user_id(int sockd,
                int& user_id)
{
  try
  {
    user_id=BasicSocketUtils::recvInt(sockd);
  }
  catch(const std::exception& e)
  {
    return THOT_ERROR;
  }
  
  return THOT_OK;
}

//---------------
void* process_request(void* void_ptr)
{
      // Read request data and release memory
  request_data rdata=*(request_data*) void_ptr;
  delete (request_data*) void_ptr;
    
      // Initialize variables
  int verbose=THOTDEC_NON_VERBOSE_MODE;
  if(ts_pars.v_given)
    verbose=THOTDEC_NORMAL_VERBOSE_MODE;
  if(ts_pars.vd_given)
    verbose=THOTDEC_DEBUG_VERBOSE_MODE;
  
  bool printTid=true;
  if(verbose==THOTDEC_DEBUG_VERBOSE_MODE)
    printTid=false;
  
  if(verbose)
  {
        // Current date/time based on current system
    time_t now = time(0);
        // Convert now to tm struct for local timezone
    tm* localtm = localtime(&now);
    StdCerrThreadSafeCond(printTid)<<"----------------------------------------------------"<<std::endl;
    StdCerrThreadSafeCond(printTid)<<"Processing new request..."<<std::endl;
    StdCerrThreadSafeCond(printTid)<<"Current time: "<<asctime(localtm);
    StdCerrThreadSafeCond(printTid)<<"Origin: "<<inet_ntoa(rdata.sin_addr)<<std::endl;
    StdCerrThreadSafeCond(printTid)<<"Request type: "<<rdata.request_type<<std::endl;
  }

  try
  {
        // Process request measuring time
    double elapsed_prev,elapsed,ucpu,scpu;
    ctimer(&elapsed_prev,&ucpu,&scpu);

    process_request_switch(rdata.sockd,rdata.user_id,rdata.request_type,verbose);

    ctimer(&elapsed,&ucpu,&scpu);

    if(verbose)
    {
      StdCerrThreadSafeCond(printTid)<<"Elapsed time: " << elapsed-elapsed_prev << " secs\n";
    }
  }
  catch(const std::exception& e)
  {
        // Clean after failure
    if(verbose) StdCerrThreadSafeCond(printTid) << e.what() << std::endl;
    close(rdata.sockd);
    decrease_num_threads_var();
    pthread_exit(NULL);
  }

  close(rdata.sockd);
  decrease_num_threads_var();
  pthread_exit(NULL);
}

//---------------
void process_request_switch(int sockd,
                            int user_id,
                            int server_request_type,
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
  int ret;
  
  switch(server_request_type)
  {
    case OL_TRAIN_PAIR:
      BasicSocketUtils::recvStlStr(sockd,stlStrSrc);
      BasicSocketUtils::recvStlStr(sockd,stlStrRef);
      ret=thotDecoderPtr->onlineTrainSentPair(user_id,stlStrSrc.c_str(),stlStrRef.c_str(),verbose);
      BasicSocketUtils::writeInt(sockd,ret);
      if(ret==THOT_ERROR)
        throw std::runtime_error("Online training request failed");
      break;

    case TRAIN_ECM:
      BasicSocketUtils::recvStlStr(sockd,stlStr1);
      BasicSocketUtils::recvStlStr(sockd,stlStr2);
      ret=thotDecoderPtr->trainEcm(user_id,stlStr1.c_str(),stlStr2.c_str(),verbose);
      BasicSocketUtils::writeInt(sockd,ret);
      if(ret==THOT_ERROR)
        throw std::runtime_error("Error correction model training request failed");
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
      thotDecoderPtr->sentPairVerCov(user_id,stlStrSrc.c_str(),stlStrRef.c_str(),result,verbose);
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
      
    case PRINT_MODELS:
      ret=thotDecoderPtr->printModels(verbose);
      BasicSocketUtils::writeInt(sockd,ret);
      if(ret==THOT_ERROR)
        throw std::runtime_error("Printing request failed");
      break;

    case END_SERVER: // NOTE: this request only involves sending
                     // acknowledgement message to client and clearing
                     // data structures, end_server variable is not
                     // modified from here
      thotDecoderPtr->clearTrans(verbose);
      BasicSocketUtils::writeInt(sockd,THOT_OK);
      break;
  }
}

//---------------
int init_user_pars_if_required(int user_id)
{
  int ret=THOT_OK;
  std::set<int>::const_iterator user_set_iter=user_set.find(user_id);
  if(user_set_iter==user_set.end())
  {
        // User is new

        // Store user
    user_set.insert(user_id);

        // Initialize parameters
    ret=thotDecoderPtr->initUserPars(user_id,tdu_pars,ts_pars.v_given);
  }
  return ret;
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
      // request threads (however, with current implementation this is
      // no longer possible)
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

        // -t parameter
    if(argv_stl[i]=="-t" && !matched)
    {
      ts_pars.t_given=true;
      ++matched;
    }

        // -v parameter
    if(argv_stl[i]=="-v" && !matched)
    {
      ts_pars.v_given=true;
      ++matched;
    }

        // -vd parameter
    if(argv_stl[i]=="-vd" && !matched)
    {
      ts_pars.vd_given=true;
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

  if(ts_pars.i_given && ts_pars.t_given)
  {
    std::cerr<<"Error: -i and -t parameters cannot be given simultaneously!"<<std::endl;
    return THOT_ERROR;
  }

  if(ts_pars.w_given && ts_pars.t_given)
  {
    std::cerr<<"Error: -w and -t parameters cannot be given simultaneously!"<<std::endl;
    return THOT_ERROR;
  }

  if(ts_pars.v_given && ts_pars.vd_given)
  {
    std::cerr<<"Error: -v and -vd parameters cannot be given simultaneously!"<<std::endl;
    return THOT_ERROR;
  }

  return THOT_OK;
}

//---------------
void printParameters(void)
{
  std::cerr<<"-i: "<<ts_pars.i_given<<std::endl;
  std::cerr<<"-c: "<<ts_pars.c_given<<std::endl;
  std::cerr<<"-p: "<<ts_pars.server_port<<std::endl;
  std::cerr<<"-w: "<<ts_pars.w_given<<std::endl;
  std::cerr<<"-v: "<<ts_pars.v_given<<std::endl;
  std::cerr<<"-vd: "<<ts_pars.vd_given<<std::endl;
}

//---------------
void printUsage(void)
{
  std::cerr<<"Usage: thot_server    -i | -c <string>"<<std::endl;
  std::cerr<<"                      [-p <int>] [ -w | -t ] [ -v | -vd ] [--help] [--version]"<<std::endl;
  std::cerr<<std::endl;
  std::cerr<<"-i             Test server initialization using master.ini and exit"<<std::endl<<std::endl;
  std::cerr<<"-c <string>    Configuration file"<<std::endl<<std::endl;
  std::cerr<<"-p <int>       Port used by the server"<<std::endl<<std::endl;
  std::cerr<<"-w             Print model weights and exit"<<std::endl<<std::endl;
  std::cerr<<"-t             Test software modules incorporated in model descriptors and exit"<<std::endl<<std::endl;
  std::cerr<<"-v             Verbose mode"<<std::endl<<std::endl;
  std::cerr<<"-vd            Verbose mode for debugging. This mode displays more information"<<std::endl;
  std::cerr<<"               than -v option but it is not designed to work for concurrent"<<std::endl;
  std::cerr<<"               server queries"<<std::endl<<std::endl;
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
