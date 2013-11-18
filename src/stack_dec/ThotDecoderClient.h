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
/* Module: ThotDecoderClient.h                                      */
/*                                                                  */
/* Prototype file: ThotDecoderClient.h                              */
/*                                                                  */
/* Description: Client of the stack_trans_server.                   */
/*                                                                  */
/********************************************************************/

#ifndef _ThotDecoderClient_h
#define _ThotDecoderClient_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "client_server_defs.h"
#include <ErrorDefs.h>
#include <BasicSocketUtils.h>
#include <StrProcUtils.h>
#include <string>
#include <iostream>

using namespace std;

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------

//--------------- ThotDecoderClient class

class ThotDecoderClient
{
 public:
    
    // Basic functions
    ThotDecoderClient(void);
    int connectToTransServer(const char *dirServ,
                             unsigned int _port);
    bool sendSentPairForOlTrain(int user_id,
                                const char *srcSent,
                                const char *refSent);
    bool sendStrPairForTrainEcm(int user_id,
                                const char *strx,
                                const char *stry);
    bool sendSentToTranslate(int user_id,
                             const char *sentenceToTranslate,
                             std::string &translatedSentence);
    bool sendSentPairVerCov(int user_id,
                            const char *srcSent,
                            const char *refSent,
                            std::string &translatedSentence);
    bool startCat(int user_id,
                  const char *sentenceToTranslate,
                  std::string &translatedSentence);
    bool addStrToPref(int user_id,
                      const char* strToAddToPref,
                      std::string &translatedSentence);
    bool resetPref(int user_id);
    bool sendClearRequest(int user_id);
    bool sendPrintRequest(int user_id,
                          const char *printPrefix);
    void disconnect(int user_id);
    bool sendEndServerRequest(int user_id);
    
 private:

    // Connection information
    bool connected;
    unsigned int port;
    std::string serverName;

    // Data members
    int fileDesc;
};

#endif
