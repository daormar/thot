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
/* Module: ThotDecoderClient                                        */
/*                                                                  */
/* Definitions file: ThotDecoderClient.cc                           */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "ThotDecoderClient.h"

//--------------- Global variables -----------------------------------

//--------------- Function declarations 

//--------------- Constants


//--------------- Classes --------------------------------------------


//--------------------------
ThotDecoderClient::ThotDecoderClient(void)
{
 connected=false;
}

//--------------------------
int ThotDecoderClient::connectToTransServer(const char *dirServ,
                                            unsigned int _port)
{
 if(!connected)
 {
   int fd;
   int retVal=BasicSocketUtils::connect(dirServ,_port,fd);
   if(retVal==OK)
   {
         // Set data member values    
     connected=true;
     port=_port;
     serverName=dirServ;   
     fileDesc=fd;

     return OK;
   }
   else
     return ERROR;
 }
 else
 {
   cerr<<"Client already connected!"<<endl;
   return ERROR;
 }
}

//--------------------------
bool ThotDecoderClient::sendSentPairForOlTrain(int user_id,
                                               const char *srcSent,
                                               const char *refSent)
{
  if(connected)
  {// Send request
    BasicSocketUtils::writeInt(fileDesc,OL_TRAIN_PAIR);
    BasicSocketUtils::writeInt(fileDesc,user_id);
    BasicSocketUtils::writeStr(fileDesc,srcSent);
    BasicSocketUtils::writeStr(fileDesc,refSent);

    int retVal=BasicSocketUtils::recvInt(fileDesc);
    return retVal;
  }
  else
  {
    cerr<<"ThotDecoderClient not connected!"<<endl;
    return ERROR;
  }
}

//--------------------------
bool ThotDecoderClient::sendStrPairForTrainEcm(int user_id,
                                               const char *strx,
                                               const char *stry)
{
  if(connected)
  {// Send request
    BasicSocketUtils::writeInt(fileDesc,TRAIN_ECM);
    BasicSocketUtils::writeInt(fileDesc,user_id);
    BasicSocketUtils::writeStr(fileDesc,strx);
    BasicSocketUtils::writeStr(fileDesc,stry);

    int retVal=BasicSocketUtils::recvInt(fileDesc);
    return retVal;
  }
  else
  {
    cerr<<"ThotDecoderClient not connected!"<<endl;
    return ERROR;
  }
}

//--------------------------
bool ThotDecoderClient::sendSentToTranslate(int user_id,
                                            const char *sentenceToTranslate,
                                            std::string &translatedSentence)
{
  if(connected)
  {// Send request
    BasicSocketUtils::writeInt(fileDesc,TRANSLATE_SENT);
    BasicSocketUtils::writeInt(fileDesc,user_id);
    BasicSocketUtils::writeStr(fileDesc,sentenceToTranslate);

    int retVal=BasicSocketUtils::recvStlStr(fileDesc,translatedSentence);
    return OK;
  }
  else
  {
    cerr<<"ThotDecoderClient not connected!"<<endl;
    return ERROR;
  }    
}

//--------------------------
bool ThotDecoderClient::sendSentPairVerCov(int user_id,
                                           const char *srcSent,
                                           const char *refSent,
                                           std::string &translatedSentence)
{
  if(connected)
  {// Send request
    BasicSocketUtils::writeInt(fileDesc,VERIFY_COV);
    BasicSocketUtils::writeInt(fileDesc,user_id);
    BasicSocketUtils::writeStr(fileDesc,srcSent);
    BasicSocketUtils::writeStr(fileDesc,refSent);

    int retVal=BasicSocketUtils::recvStlStr(fileDesc,translatedSentence);
    return OK;
  }
  else
  {
    cerr<<"ThotDecoderClient not connected!"<<endl;
    return ERROR;
  }
}

//--------------------------
bool ThotDecoderClient::startCat(int user_id,
                                 const char *sentenceToTranslate,
                                 std::string &translatedSentence)
{
  if(connected)
  {// Send request
    BasicSocketUtils::writeInt(fileDesc,START_CAT);
    BasicSocketUtils::writeInt(fileDesc,user_id);
    BasicSocketUtils::writeStr(fileDesc,sentenceToTranslate);
    
    int retVal=BasicSocketUtils::recvStlStr(fileDesc,translatedSentence);
    return OK;
  }
  else
  {
    cerr<<"ThotDecoderClient not connected!"<<endl;
    return ERROR;
  }
}

//--------------------------
bool ThotDecoderClient::addStrToPref(int user_id,
                                     const char* strToAddToPref,
                                     std::string &translatedSentence)
{
  if(connected)
  {// Send request
    BasicSocketUtils::writeInt(fileDesc,ADD_STR_TO_PREF);
    BasicSocketUtils::writeInt(fileDesc,user_id);
    BasicSocketUtils::writeStr(fileDesc,strToAddToPref);
    
    int retVal=BasicSocketUtils::recvStlStr(fileDesc,translatedSentence);
    return OK;
  }
  else
  {
    cerr<<"ThotDecoderClient not connected!"<<endl;
    return ERROR;
  }
}

//--------------------------
bool ThotDecoderClient::resetPref(int user_id)
{    
  if(connected)
  {// Send request
    int retVal;

    BasicSocketUtils::writeInt(fileDesc,RESET_PREF);
    BasicSocketUtils::writeInt(fileDesc,user_id);
    retVal=BasicSocketUtils::recvInt(fileDesc);
    return retVal;
  }
  else
  {
    cerr<<"ThotDecoderClient not connected!"<<endl;
    return ERROR;
  }        
}

//--------------------------
bool ThotDecoderClient::sendClearRequest(int user_id)
{
  if(connected)
  {// Send request
    BasicSocketUtils::writeInt(fileDesc,CLEAR_TRANS);
    BasicSocketUtils::writeInt(fileDesc,user_id);

    int retVal=BasicSocketUtils::recvInt(fileDesc);
    return retVal;
  }
  else
  {
    cerr<<"ThotDecoderClient not connected!"<<endl;
    return ERROR;
  }      
}

//--------------------------
bool ThotDecoderClient::sendPrintRequest(int user_id)
{
  if(connected)
  {// Send request
    BasicSocketUtils::writeInt(fileDesc,PRINT_MODELS);
    BasicSocketUtils::writeInt(fileDesc,user_id);

    int retVal=BasicSocketUtils::recvInt(fileDesc);
    return retVal;
  }
  else
  {
    cerr<<"ThotDecoderClient not connected!"<<endl;
    return ERROR;
  }    
}

//--------------------------
void ThotDecoderClient::disconnect(int user_id)
{
 if(connected)
 {
  BasicSocketUtils::writeInt(fileDesc,END_CLIENT_DIALOG);
  BasicSocketUtils::writeInt(fileDesc,user_id);
  close(fileDesc); 
  connected=false;   
 }
}

//--------------------------
bool ThotDecoderClient::sendEndServerRequest(int user_id)
{
  if(connected)
  {// Send request
    BasicSocketUtils::writeInt(fileDesc,END_SERVER);
    BasicSocketUtils::writeInt(fileDesc,user_id);
  
    int retVal=BasicSocketUtils::recvInt(fileDesc);
    close(fileDesc); 
    connected=false;
    return retVal;
  }
  else
  {
    cerr<<"ThotDecoderClient not connected!"<<endl;
    return ERROR;
  }
}
