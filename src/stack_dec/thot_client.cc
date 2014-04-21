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
/* Module: thot_client.cc                                           */
/*                                                                  */
/* Definitions file: thot_client.cc                                 */
/*                                                                  */
/* Description: Implements a translator client                      */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "ThotDecoderClient.h"
#include "ErrorDefs.h"
#include <SmtPreprocDefs.h>
#include "options.h"
#include "ctimer.h"
#include "client_server_defs.h"
#include "thot_client_pars.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

//--------------- Constants ------------------------------------------


//--------------- Function Declarations ------------------------------

int process_request(const thot_client_pars& tdcPars);
int TakeParameters(int argc,
                   char *argv[],
                   thot_client_pars& tdcPars);
void printUsage(void);
void version(void);
void printDesc(void);

//--------------- Global variables -----------------------------------


//--------------- Function Definitions -------------------------------

//--------------- main function
int main(int argc,char *argv[])
{
  thot_client_pars tdcPars;
 
  if(TakeParameters(argc,argv,tdcPars)==OK)
  {// Parameters ok
   double elapsed_ant,elapsed,ucpu,scpu;
   bool retVal;

   if(tdcPars.verbose)
   {
     ctimer(&elapsed_ant,&ucpu,&scpu);
   }
   retVal=process_request(tdcPars);
   if(tdcPars.verbose)
   {
     ctimer(&elapsed,&ucpu,&scpu);
     cerr<<"Elapsed time: " << elapsed-elapsed_ant << " secs\n";
   }
   return retVal;
 } 
 else return ERROR;  
}


//--------------- main function
int process_request(const thot_client_pars& tdcPars)
{
  string s;
  vector<string> v;
  std::string translatedSentence;
  ThotDecoderClient thotDecoderClient;
  int retVal=OK;
  double elapsed_ant,elapsed,ucpu,scpu;

      // Connect to translation server
  if(tdcPars.verbose)
  {
    cerr<<"----------------------------------------------------"<<endl;
    cerr<<"User ID: "<<tdcPars.user_id<<endl;
    cerr<<"Connecting to server..."<<endl;
    ctimer(&elapsed_ant,&ucpu,&scpu);
  }

  retVal=thotDecoderClient.connectToTransServer(tdcPars.serverIP.c_str(),
                                                tdcPars.server_port);
  if(retVal==ERROR) return ERROR;

      // Print connection latency information
  if(tdcPars.verbose)
  {
    ctimer(&elapsed,&ucpu,&scpu);
    cerr<<"Connection latency: " << elapsed-elapsed_ant << " secs\n";
  }

      // Send request to the translation server
  if(tdcPars.verbose)
  {
    cerr<<"Client: sending request to the server, request code "<<tdcPars.server_request_code<<endl;
  }
      // Get time
  ctimer(&elapsed_ant,&ucpu,&scpu);

  switch(tdcPars.server_request_code)
  {
    case OL_TRAIN_PAIR: retVal=thotDecoderClient.sendSentPairForOlTrain(tdcPars.user_id,tdcPars.stlStringSrc.c_str(),tdcPars.stlStringRef.c_str());
      ctimer(&elapsed,&ucpu,&scpu);
      if(tdcPars.verbose) cerr<<"Client: return value= "<<retVal<<endl;
      break;
    case TRAIN_ECM: retVal=thotDecoderClient.sendStrPairForTrainEcm(tdcPars.user_id,tdcPars.stlString1.c_str(),tdcPars.stlString2.c_str());
      ctimer(&elapsed,&ucpu,&scpu);
      if(tdcPars.verbose) cerr<<"Client: return value= "<<retVal<<endl;
      break;
    case TRANSLATE_SENT: retVal=thotDecoderClient.sendSentToTranslate(tdcPars.user_id,tdcPars.sentenceToTranslate.c_str(),translatedSentence);
      if(tdcPars.verbose) cerr<<"Client: translation received"<<endl;
      cout<<translatedSentence<<endl;
      break;
    case VERIFY_COV: retVal=thotDecoderClient.sendSentPairVerCov(tdcPars.user_id,tdcPars.stlStringSrc.c_str(),tdcPars.stlStringRef.c_str(),translatedSentence);
      ctimer(&elapsed,&ucpu,&scpu);
      if(tdcPars.verbose) cerr<<"Client: forced translation received"<<endl;
      cout<<translatedSentence<<endl;
      break;
    case START_CAT: retVal=thotDecoderClient.startCat(tdcPars.user_id,tdcPars.sentenceToTranslate.c_str(),translatedSentence);
      ctimer(&elapsed,&ucpu,&scpu);
      if(tdcPars.verbose) cerr<<"Client: return value= "<<retVal<<endl;
      cout<<translatedSentence<<endl;
      break;
    case ADD_STR_TO_PREF: retVal=thotDecoderClient.addStrToPref(tdcPars.user_id,tdcPars.strToAddToPref.c_str(),translatedSentence);
      ctimer(&elapsed,&ucpu,&scpu);
      if(tdcPars.verbose) cerr<<"Client: return value= "<<retVal<<endl;
      cout<<translatedSentence<<endl;
      break;
    case RESET_PREF: retVal=thotDecoderClient.resetPref(tdcPars.user_id);
      ctimer(&elapsed,&ucpu,&scpu);
      if(tdcPars.verbose) cerr<<"Client: return value= "<<retVal<<endl;
      break;
    case CLEAR_TRANS: retVal=thotDecoderClient.sendClearRequest(tdcPars.user_id);
      ctimer(&elapsed,&ucpu,&scpu);
      if(tdcPars.verbose) cerr<<"Client: return value= "<<retVal<<endl;
      break;
    case PRINT_MODELS: retVal=thotDecoderClient.sendPrintRequest(tdcPars.user_id,tdcPars.stlString.c_str());
      ctimer(&elapsed,&ucpu,&scpu);
      if(tdcPars.verbose) cerr<<"Client: return value= "<<retVal<<endl;
      break;
    case END_SERVER: retVal=thotDecoderClient.sendEndServerRequest(tdcPars.user_id);
      ctimer(&elapsed,&ucpu,&scpu);
      if(tdcPars.verbose) cerr<<"Client: return value= "<<retVal<<endl;
      break;
    default:
      ctimer(&elapsed,&ucpu,&scpu);
      break;
  }
  if(tdcPars.verbose)
  {
    cerr<<"Request latency: " << elapsed-elapsed_ant << " secs\n";
  }

      //thotDecoderClient.disconnect(); // (disconnect is not required since the server only
      //                                   dispatch one request per client execution)
  return retVal;
}

//--------------- TakeParameters function

int TakeParameters(int argc,
                   char *argv[],
                   thot_client_pars& tdcPars)
{
 int err;

 if(argc==1)
 {
   printDesc();
   return ERROR;   
 }

 err=readOption(argc,argv,"--help");
 if(err!=-1)
 {
   printUsage();
   return ERROR;   
 }      

     /* Verify --version option */
 err=readOption(argc,argv, "--version");
 if(err!=-1)
 {
   version();
   return ERROR;
 }

     /* Take the server IP*/
 err=readSTLstring(argc,argv, "-i", &tdcPars.serverIP);
 if(err==-1)
 {
   printUsage();
   return ERROR;   
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
   return OK;
 }

 //     /* Take the string pair for error correcting model training */
 // err=readTwoSTLstrings(argc,argv, "-tre", &tdcPars.stlString1,&tdcPars.stlString2);
 // if(err==0)
 // {
 //   tdcPars.server_request_code=TRAIN_ECM;
 //   return OK;
 // }

     /* Take the sentence to be translated */
 err=readSTLstring(argc,argv, "-t", &tdcPars.sentenceToTranslate);
 if(err==0)
 {
   tdcPars.server_request_code=TRANSLATE_SENT;
   return OK;
 }

     /* Take the sentence pair for coverage verifying */
 err=readTwoSTLstrings(argc,argv, "-c", &tdcPars.stlStringSrc,&tdcPars.stlStringRef);
 if(err==0)
 {
   tdcPars.server_request_code=VERIFY_COV;
   return OK;
 }

     /* Take the sentence to be translated in a CAT scenario using the
      * n-best technique*/
 err=readSTLstring(argc,argv, "-sc", &tdcPars.sentenceToTranslate);
 if(err==0)
 {
   tdcPars.server_request_code=START_CAT;
   return OK;
 }

     /* Take the sentence to be translated */
 err=readSTLstring(argc,argv, "-ap",&tdcPars.strToAddToPref);
 if(err==0)
 {
   tdcPars.server_request_code=ADD_STR_TO_PREF;
   return OK;
 }

     /* Check -rp option */
 err=readOption(argc,argv,"-rp");
 if(err!=-1)
 {
   tdcPars.server_request_code=RESET_PREF;
   return OK;
 }

     /* Verify clear option */
 err=readOption(argc,argv, "-clear");
 if(err==0)
 {
   tdcPars.server_request_code=CLEAR_TRANS;
   return OK;
 }

     /* Take the prefix to be used when printing models */
 err=readSTLstring(argc,argv, "-o", &tdcPars.stlString);
 if(err==0)
 {
   tdcPars.server_request_code=PRINT_MODELS;
   return OK;
 }

     /* Verify verbose option */
 err=readOption(argc,argv, "-e");
 if(err==0)
 {
   tdcPars.server_request_code=END_SERVER;
   return OK;
 }

 return ERROR;
}

//--------------- printDesc() function
void printDesc(void)
{
  cerr<<"thot_client written by Daniel Ortiz\n";
  cerr<<"type \"thot_client --help\" to get usage information.\n";
}

//--------------------------------
void printUsage(void)
{
  cerr<<"Usage: thot_client        -i <string> [-p <int>] [-uid <int>]\n";
  cerr<<"                          { -tr <srcsent> <refsent> | \n";
  // cerr<<"                          | -tre <srcsent> <refsent> | \n";
  cerr<<"                          | -t <string> | -c <srcsent> <refsent> |\n";
  cerr<<"                          | -sc <string> | -ap <string> | -rp |\n";
  cerr<<"                          | -clear | -o <string> | -e } [ -v ]\n";
  cerr<<"                          [--help] [--version]\n\n";
  cerr<<"-i <string>               Set IP address of the server.\n\n";
  cerr<<"-p <int>                  Server port.\n\n";
  cerr<<"-uid <int>                Set user id ("<<DEFAULT_USER_ID<<" by default).\n\n";
  cerr<<"-tr <srcsent> <refsent>   Train server models given a sentence pair.\n\n";
  // cerr<<"-tre <srcsent> <refsent>  Train error correcting model given a string pair.\n\n";
  cerr<<"-t <string>               Translate sentence.\n\n";
  cerr<<"-c <srcsent> <refsent>    Verify model coverage for reference sentence.\n\n";
  cerr<<"-sc <string>              Start CAT system for the given sentence, using the\n";
  cerr<<"                          null string as prefix.\n\n";
  cerr<<"-ap <string>              Add string to prefix.\n\n";
  cerr<<"-rp <string>              Reset prefix.\n\n";
  cerr<<"-clear                    Clear loaded models.\n\n";
  cerr<<"-o <string>               Print models.\n\n";
  cerr<<"-e                        End server.\n\n";
  cerr<<"-v                        Verbose mode.\n\n";
  cerr<<"--help                    Display this help and exit.\n\n";
  cerr<<"--version                 Output version information and exit.\n\n";
}
//--------------------------------
void version(void)
{
  cerr<<"thot_client is part of the thot package\n";
  cerr<<"thot version "<<THOT_VERSION<<endl;
  cerr<<"thot is GNU software written by Daniel Ortiz\n";
}
