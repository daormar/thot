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

/**
 * @file thot_client.cc
 * 
 * @brief Implements a client for thot_server tool.
 */

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "ThotDecoderClient.h"
#include "ErrorDefs.h"
#include <SmtPreprocDefs.h>
#include "options.h"
#include "ctimer.h"
#include "client_server_defs.h"
#include "thot_client_pars.h"
#include <math.h>
#include "AwkInputStream.h"
#include <iostream>
#include <fstream>
#include <iomanip>

//--------------- Constants ------------------------------------------


//--------------- Function Declarations ------------------------------

void process_request(const thot_client_pars& tdcPars);
int extractJsonFileContent(std::string jsonFileName,
                           std::string& jsonFileContent);
int TakeParameters(int argc,
                   char *argv[],
                   thot_client_pars& tdcPars);
void printUsage(void);
void version(void);
void printDesc(void);

//--------------- Global variables -----------------------------------


//--------------- Function Definitions -------------------------------

//---------------
int main(int argc,char *argv[])
{
  thot_client_pars tdcPars;
 
  if(TakeParameters(argc,argv,tdcPars)==THOT_OK)
  {
        // Parameters ok

        // Process json file if it was given
    if(!tdcPars.jsonFileName.empty())
    {
      int ret=extractJsonFileContent(tdcPars.jsonFileName,tdcPars.sentenceToTranslate);
      if(ret==THOT_ERROR)
        return THOT_ERROR;
    }
    
        // Process request
    try
    {
      process_request(tdcPars);
    }
    catch(const std::exception& e)
    {
      std::cerr << e.what() << std::endl;
      return THOT_ERROR;
    }
    
    return THOT_OK;
  } 
  else return THOT_ERROR;  
}

//---------------
void process_request(const thot_client_pars& tdcPars)
{
  std::string s;
  std::vector<std::string> v;
  std::string translatedSentence;
  std::string bestHypInfo;
  ThotDecoderClient thotDecoderClient;
  double elapsed_ant,elapsed,ucpu,scpu;
  double connection_latency=0;

      // Connect to translation server
  if(tdcPars.verbose)
  {
    std::cerr<<"----------------------------------------------------"<<std::endl;
    std::cerr<<"User id: "<<tdcPars.user_id<<std::endl;
    std::cerr<<"Connecting to server..."<<std::endl;
    ctimer(&elapsed_ant,&ucpu,&scpu);
  }

  thotDecoderClient.connectToTransServer(tdcPars.serverIP.c_str(),
                                         tdcPars.server_port);

      // Print connection latency information
  if(tdcPars.verbose)
  {
    ctimer(&elapsed,&ucpu,&scpu);
    connection_latency=elapsed-elapsed_ant;
    std::cerr<<"Connection latency: " << connection_latency << " secs\n";
  }

      // Send request to the translation server
  if(tdcPars.verbose)
  {
    std::cerr<<"Client: sending request to the server, request type "<<tdcPars.server_request_code<<std::endl;
  }
      // Get time
  if(tdcPars.verbose)
    ctimer(&elapsed_ant,&ucpu,&scpu);

  switch(tdcPars.server_request_code)
  {
    case OL_TRAIN_PAIR: thotDecoderClient.sendSentPairForOlTrain(tdcPars.user_id,tdcPars.stlStringSrc.c_str(),tdcPars.stlStringRef.c_str());
      break;
    case TRAIN_ECM: thotDecoderClient.sendStrPairForTrainEcm(tdcPars.user_id,tdcPars.stlString1.c_str(),tdcPars.stlString2.c_str());
      break;
    case TRANSLATE_SENT: thotDecoderClient.sendSentToTranslate(tdcPars.user_id,tdcPars.sentenceToTranslate.c_str(),translatedSentence,bestHypInfo);
      std::cout<<translatedSentence<<std::endl;
      break;
    case TRANSLATE_SENT_HYPINFO: thotDecoderClient.sendSentToTranslate(tdcPars.user_id,tdcPars.sentenceToTranslate.c_str(),translatedSentence,bestHypInfo);
      std::cout<<bestHypInfo<<std::endl;
      std::cout<<translatedSentence<<std::endl;
      break;
    case VERIFY_COV: thotDecoderClient.sendSentPairVerCov(tdcPars.user_id,tdcPars.stlStringSrc.c_str(),tdcPars.stlStringRef.c_str(),translatedSentence);
      std::cout<<translatedSentence<<std::endl;
      break;
    case START_CAT: thotDecoderClient.startCat(tdcPars.user_id,tdcPars.sentenceToTranslate.c_str(),translatedSentence);
      std::cout<<translatedSentence<<std::endl;
      break;
    case ADD_STR_TO_PREF: thotDecoderClient.addStrToPref(tdcPars.user_id,tdcPars.strToAddToPref.c_str(),translatedSentence);
      std::cout<<translatedSentence<<std::endl;
      break;
    case RESET_PREF: thotDecoderClient.resetPref(tdcPars.user_id);
      break;
    case PRINT_MODELS: thotDecoderClient.sendPrintRequest(tdcPars.user_id);
      break;
    case END_SERVER: thotDecoderClient.sendEndServerRequest(tdcPars.user_id);
      break;
    default:
      break;
  }
  if(tdcPars.verbose)
  {
    ctimer(&elapsed,&ucpu,&scpu);
    double request_latency=elapsed-elapsed_ant;
    std::cerr<<"Request latency: " << request_latency << " secs\n";
    std::cerr<<"Elapsed time (connection + request latencies): " << connection_latency+request_latency << " secs\n";
  }

      //thotDecoderClient.disconnect(); // (disconnect is not required since the server only
      //                                   dispatch one request per client execution)
}

//---------------
int extractJsonFileContent(std::string jsonFileName,
                           std::string& jsonFileContent)
{
  AwkInputStream awk;
  if(awk.open(jsonFileName.c_str())==THOT_ERROR)
  {
    std::cerr<<"Error while opening json file "<<jsonFileName<<std::endl;
    return THOT_ERROR;
  }
  else
  {
    jsonFileContent.clear();
    while(awk.getln())
    {
      jsonFileContent+=awk.dollar(0);
    }
    return THOT_OK;
  }
}

//---------------
int TakeParameters(int argc,
                   char *argv[],
                   thot_client_pars& tdcPars)
{
 int err;

 if(argc==1)
 {
   printDesc();
   return THOT_ERROR;   
 }

 err=readOption(argc,argv,"--help");
 if(err!=-1)
 {
   printUsage();
   return THOT_ERROR;   
 }      

     /* Verify --version option */
 err=readOption(argc,argv, "--version");
 if(err!=-1)
 {
   version();
   return THOT_ERROR;
 }

     /* Take the server IP*/
 err=readSTLstring(argc,argv, "-i", &tdcPars.serverIP);
 if(err==-1)
 {
   printUsage();
   return THOT_ERROR;   
 }

     /* Take the -p parameter */
 err=readUnsignedInt(argc,argv, "-p", &tdcPars.server_port);
 if(err!=0)
 {
   tdcPars.server_port=DEFAULT_SERVER_PORT;
 }

     /* Take the -uid parameter */
 err=readInt(argc,argv, "-uid", &tdcPars.user_id);
 if(err!=0)
 {
   tdcPars.user_id=DEFAULT_USER_ID;
 }

     /* Verify verbose option */
 tdcPars.verbose=0;
 err=readOption(argc,argv, "-v");
 if(err==0) tdcPars.verbose=1;

     /* Take the sentence pair to be trained */
 err=readTwoSTLstrings(argc,argv, "-tr", &tdcPars.stlStringSrc,&tdcPars.stlStringRef);
 if(err==0)
 {
   tdcPars.server_request_code=OL_TRAIN_PAIR;
   return THOT_OK;
 }

 //     /* Take the string pair for error correcting model training */
 // err=readTwoSTLstrings(argc,argv, "-tre", &tdcPars.stlString1,&tdcPars.stlString2);
 // if(err==0)
 // {
 //   tdcPars.server_request_code=TRAIN_ECM;
 //   return THOT_OK;
 // }

     /* Take the sentence to be translated */
 err=readSTLstring(argc,argv, "-t", &tdcPars.sentenceToTranslate);
 if(err==0)
 {
   tdcPars.server_request_code=TRANSLATE_SENT;
   return THOT_OK;
 }

     /* Take the sentence to be translated */
 err=readSTLstring(argc,argv, "-th", &tdcPars.sentenceToTranslate);
 if(err==0)
 {
   tdcPars.server_request_code=TRANSLATE_SENT_HYPINFO;
   return THOT_OK;
 }

     /* Take the name of the json file */
 err=readSTLstring(argc,argv, "-j", &tdcPars.jsonFileName);
 if(err==0)
 {
   tdcPars.server_request_code=TRANSLATE_SENT;
   return THOT_OK;
 }

     /* Take the sentence pair for coverage verifying */
 err=readTwoSTLstrings(argc,argv, "-c", &tdcPars.stlStringSrc,&tdcPars.stlStringRef);
 if(err==0)
 {
   tdcPars.server_request_code=VERIFY_COV;
   return THOT_OK;
 }

     /* Take the sentence to be translated in a CAT scenario using the
      * n-best technique*/
 err=readSTLstring(argc,argv, "-sc", &tdcPars.sentenceToTranslate);
 if(err==0)
 {
   tdcPars.server_request_code=START_CAT;
   return THOT_OK;
 }

     /* Take the sentence to be translated */
 err=readSTLstring(argc,argv, "-ap",&tdcPars.strToAddToPref);
 if(err==0)
 {
   tdcPars.server_request_code=ADD_STR_TO_PREF;
   return THOT_OK;
 }

     /* Check -rp option */
 err=readOption(argc,argv,"-rp");
 if(err!=-1)
 {
   tdcPars.server_request_code=RESET_PREF;
   return THOT_OK;
 }

      /* Verify print option */
 err=readOption(argc,argv, "-pr");
 if(err==0)
 {
   tdcPars.server_request_code=PRINT_MODELS;
   return THOT_OK;
 }

     /* Verify -e option */
 err=readOption(argc,argv, "-e");
 if(err==0)
 {
   tdcPars.server_request_code=END_SERVER;
   return THOT_OK;
 }

 return THOT_ERROR;
}

//---------------
void printDesc(void)
{
  std::cerr<<"thot_client is part of the thot package "<<std::endl;
  std::cerr<<"thot version "<<THOT_VERSION<<std::endl;
  std::cerr<<"thot is GNU software written by Daniel Ortiz"<<std::endl;
}

//---------------
void printUsage(void)
{
  std::cerr<<"Usage: thot_client           -i <string> [-p <int>] [-uid <int>]\n";
  std::cerr<<"                             { -tr <srcstring> <refstring> |\n";
  // std::cerr<<"                          | -tre <srcstring> <refstring> |\n";
  std::cerr<<"                             | -t <string> | -th <string> | -j <string> |\n";
  std::cerr<<"                             | -c <srcstring> <refstring> |\n";
  std::cerr<<"                             | -sc <string> | -ap <string> | -rp |\n";
  std::cerr<<"                             | -o <string> | -e } [ -v ]\n";
  std::cerr<<"                             [--help] [--version]\n\n";
  std::cerr<<"-i <string>                  Set IP address of the server.\n";
  std::cerr<<"-p <int>                     Server port.\n";
  std::cerr<<"-uid <int>                   Set user id ("<<DEFAULT_USER_ID<<" by default).\n";
  std::cerr<<"-tr <srcstring> <refstring>  Train server models given a sentence pair.\n";
  // std::cerr<<"-tre <srcstring> <refstring>  Train error correcting model given a string pair.\n";
  std::cerr<<"-t <string>                  Translate sentence.\n";
  std::cerr<<"-th <string>                 Translate sentence (returns hypothesis\n";
  std::cerr<<"                             information).\n";
  std::cerr<<"-j <string>                  Translate sentence given in a file in json format.\n";  
  std::cerr<<"                             The file contains the source sentence plus\n";
  std::cerr<<"                             metadata.\n";
  std::cerr<<"                             NOTE: only servers loading modules that support\n";
  std::cerr<<"                             json format will work with this option\n";
  std::cerr<<"-c <srcstring> <refstring>   Verify model coverage for reference sentence.\n";
  std::cerr<<"-sc <string>                 Start CAT system for the given sentence, using\n";
  std::cerr<<"                             the null string as prefix.\n";
  std::cerr<<"-ap <string>                 Add string to prefix.\n";
  std::cerr<<"-rp <string>                 Reset prefix.\n";
  std::cerr<<"-pr                          Print models.\n";
  std::cerr<<"-e                           End server.\n";
  std::cerr<<"-v                           Verbose mode.\n";
  std::cerr<<"--help                       Display this help and exit.\n";
  std::cerr<<"--version                    Output version information and exit.\n";
}

//---------------
void version(void)
{
  std::cerr<<"thot_client is part of the thot package\n";
  std::cerr<<"thot version "<<THOT_VERSION<<std::endl;
  std::cerr<<"thot is GNU software written by Daniel Ortiz\n";
}
