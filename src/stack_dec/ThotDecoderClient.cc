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
void ThotDecoderClient::connectToTransServer(const char *dirServ,
                                             unsigned int _port)
{
 if(!connected)
 {
   int fd;
   BasicSocketUtils::connect(dirServ,_port,fd);

       // Set data member values    
   connected=true;
   port=_port;
   serverName=dirServ;   
   fileDesc=fd;
 }
 else
 {
   throw std::runtime_error("Client already connected");        
 }
}

//--------------------------
void ThotDecoderClient::sendSentPairForOlTrain(int user_id,
                                               const char *srcSent,
                                               const char *refSent)
{
  if(connected)
  {
    BasicSocketUtils::writeInt(fileDesc,OL_TRAIN_PAIR);
    BasicSocketUtils::writeInt(fileDesc,user_id);
    BasicSocketUtils::writeStr(fileDesc,srcSent);
    BasicSocketUtils::writeStr(fileDesc,refSent);
    int ret=BasicSocketUtils::recvInt(fileDesc);
    if(ret==THOT_ERROR)
      throw std::runtime_error("Online training request failed");
  }
  else
  {
    throw std::runtime_error("ThotDecoderClient not connected");        
  }
}

//--------------------------
void ThotDecoderClient::sendStrPairForTrainEcm(int user_id,
                                               const char *strx,
                                               const char *stry)
{
  if(connected)
  {
    BasicSocketUtils::writeInt(fileDesc,TRAIN_ECM);
    BasicSocketUtils::writeInt(fileDesc,user_id);
    BasicSocketUtils::writeStr(fileDesc,strx);
    BasicSocketUtils::writeStr(fileDesc,stry);
    int ret=BasicSocketUtils::recvInt(fileDesc);
    if(ret==THOT_ERROR)
      throw std::runtime_error("Error correction model training request failed");
  }
  else
  {
    throw std::runtime_error("ThotDecoderClient not connected");        
  }
}

//--------------------------
void ThotDecoderClient::sendSentToTranslate(int user_id,
                                            const char *sentenceToTranslate,
                                            std::string &translatedSentence,
                                            std::string& bestHypInfo)
{
  if(connected)
  {
    BasicSocketUtils::writeInt(fileDesc,TRANSLATE_SENT);
    BasicSocketUtils::writeInt(fileDesc,user_id);
    BasicSocketUtils::writeStr(fileDesc,sentenceToTranslate);
    BasicSocketUtils::recvStlStr(fileDesc,translatedSentence);
    BasicSocketUtils::recvStlStr(fileDesc,bestHypInfo);
  }
  else
  {
    throw std::runtime_error("ThotDecoderClient not connected");        
  }    
}

//--------------------------
void ThotDecoderClient::sendSentPairVerCov(int user_id,
                                           const char *srcSent,
                                           const char *refSent,
                                           std::string &translatedSentence)
{
  if(connected)
  {
    BasicSocketUtils::writeInt(fileDesc,VERIFY_COV);
    BasicSocketUtils::writeInt(fileDesc,user_id);
    BasicSocketUtils::writeStr(fileDesc,srcSent);
    BasicSocketUtils::writeStr(fileDesc,refSent);
    BasicSocketUtils::recvStlStr(fileDesc,translatedSentence);
  }
  else
  {
    throw std::runtime_error("ThotDecoderClient not connected");        
  }
}

//--------------------------
void ThotDecoderClient::startCat(int user_id,
                                 const char *sentenceToTranslate,
                                 std::string &translatedSentence)
{
  if(connected)
  {
    BasicSocketUtils::writeInt(fileDesc,START_CAT);
    BasicSocketUtils::writeInt(fileDesc,user_id);
    BasicSocketUtils::writeStr(fileDesc,sentenceToTranslate);    
    BasicSocketUtils::recvStlStr(fileDesc,translatedSentence);
  }
  else
  {
    throw std::runtime_error("ThotDecoderClient not connected");        
  }
}

//--------------------------
void ThotDecoderClient::addStrToPref(int user_id,
                                     const char* strToAddToPref,
                                     std::string &translatedSentence)
{
  if(connected)
  {
    BasicSocketUtils::writeInt(fileDesc,ADD_STR_TO_PREF);
    BasicSocketUtils::writeInt(fileDesc,user_id);
    BasicSocketUtils::writeStr(fileDesc,strToAddToPref);    
    BasicSocketUtils::recvStlStr(fileDesc,translatedSentence);
  }
  else
  {
    throw std::runtime_error("ThotDecoderClient not connected");        
  }
}

//--------------------------
void ThotDecoderClient::resetPref(int user_id)
{    
  if(connected)
  {
    BasicSocketUtils::writeInt(fileDesc,RESET_PREF);
    BasicSocketUtils::writeInt(fileDesc,user_id);
    BasicSocketUtils::recvInt(fileDesc);
  }
  else
  {
    throw std::runtime_error("ThotDecoderClient not connected");        
  }        
}

//--------------------------
void ThotDecoderClient::sendPrintRequest(int user_id)
{
  if(connected)
  {
    BasicSocketUtils::writeInt(fileDesc,PRINT_MODELS);
    BasicSocketUtils::writeInt(fileDesc,user_id);
    int ret=BasicSocketUtils::recvInt(fileDesc);
    if(ret==THOT_ERROR)
      throw std::runtime_error("Print request failed");
  }
  else
  {
    throw std::runtime_error("ThotDecoderClient not connected");        
  }    
}

//--------------------------
void ThotDecoderClient::sendEndServerRequest(int user_id)
{
  if(connected)
  {
    BasicSocketUtils::writeInt(fileDesc,END_SERVER);
    BasicSocketUtils::writeInt(fileDesc,user_id);
    BasicSocketUtils::recvInt(fileDesc);
  }
  else
  {
    throw std::runtime_error("ThotDecoderClient not connected");        
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
