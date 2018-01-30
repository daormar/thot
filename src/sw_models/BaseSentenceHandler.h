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
 * @file BaseSentenceHandler.h
 * 
 * @brief Defines the BaseSentenceHandler class.  BaseSentenceHandler
 * class provides a general interface for sentence handlers.
 */

#ifndef _BaseSentenceHandler_h
#define _BaseSentenceHandler_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <ErrorDefs.h>
#include <string>
#include <vector>
#include "SwDefs.h"

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------

//--------------- function declarations ------------------------------

//--------------- Classes --------------------------------------------

//--------------- BaseSentenceHandler class

class BaseSentenceHandler
{
  public:

       // Functions to read and add sentence pairs
   virtual bool readSentencePairs(const char *srcFileName,
                                  const char *trgFileName,
                                  const char *sentCountsFile,
                                  std::pair<unsigned int,unsigned int>& sentRange)=0;
       // NOTE: when function readSentencePairs() is invoked, previously
       //       seen sentence pairs are removed
   
   virtual void addSentPair(std::vector<std::string> srcSentStr,
                            std::vector<std::string> trgSentStr,
                            Count c,
                            std::pair<unsigned int,unsigned int>& sentRange)=0;
   virtual unsigned int numSentPairs(void)=0;
       // NOTE: the whole valid range in a given moment is
       // [ 0 , numSentPairs() )
   virtual int nthSentPair(unsigned int n,
                           std::vector<std::string>& srcSentStr,
                           std::vector<std::string>& trgSentStr,
                           Count& c)=0;
   virtual int getSrcSent(unsigned int n,
                          std::vector<std::string>& srcSentStr)=0;
   virtual int getTrgSent(unsigned int n,
                          std::vector<std::string>& trgSentStr)=0;
   virtual int getCount(unsigned int n,
                        Count& c)=0;

       // Functions to print sentence pairs
   virtual bool printSentPairs(const char *srcSentFile,
                               const char *trgSentFile,
                               const char *sentCountsFile)=0;

       // Clear function
   virtual void clear(void)=0;

       // Destructor
   virtual ~BaseSentenceHandler(){}
};

#endif
