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
 * @file ThotDecoderClient.h
 * 
 * @brief Client for thot_server tool.
 */

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
    void connectToTransServer(const char *dirServ,
                             unsigned int _port);
    void sendSentPairForOlTrain(int user_id,
                                const char *srcSent,
                                const char *refSent);
    void sendStrPairForTrainEcm(int user_id,
                                const char *strx,
                                const char *stry);
    void sendSentToTranslate(int user_id,
                             const char *sentenceToTranslate,
                             std::string& translatedSentence,
                             std::string& bestHypInfo);
    void sendSentPairVerCov(int user_id,
                            const char *srcSent,
                            const char *refSent,
                            std::string &translatedSentence);
    void startCat(int user_id,
                  const char *sentenceToTranslate,
                  std::string &translatedSentence);
    void addStrToPref(int user_id,
                      const char* strToAddToPref,
                      std::string &translatedSentence);
    void resetPref(int user_id);
    void sendPrintRequest(int user_id);
    void sendEndServerRequest(int user_id);
    void disconnect(int user_id);
    
 private:

    // Connection information
    bool connected;
    unsigned int port;
    std::string serverName;

    // Data members
    int fileDesc;
};

#endif
