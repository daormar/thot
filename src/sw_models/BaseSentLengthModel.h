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
/* Module: BaseSentLengthModel                                      */
/*                                                                  */
/* Prototype file: BaseSentLengthModel.h                            */
/*                                                                  */
/* Description: The BaseSentLengthModel class implements the        */
/*              interface of sentence length models                 */
/*                                                                  */
/********************************************************************/

#ifndef _BaseSentLengthModel
#define _BaseSentLengthModel

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "ErrorDefs.h"
#include "SwDefs.h"
#include "Prob.h"

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- BaseSentLengthModel class

class BaseSentLengthModel
{
 public:
  
      // Load model parameters
  virtual bool load(const char* filename)=0;

      // Print model parameters
  virtual bool print(const char* filename)=0;

      // Sentence length model functions
  virtual Prob sentLenProb(unsigned int slen,unsigned int tlen)=0;
      // returns p(tl=tlen|sl=slen)
  virtual LgProb sentLenLgProb(unsigned int slen,unsigned int tlen)=0;

      // Sum sentence length model functions
  virtual Prob sumSentLenProb(unsigned int slen,unsigned int tlen)=0;
      // returns p(tl<=tlen|sl=slen)
  virtual LgProb sumSentLenLgProb(unsigned int slen,unsigned int tlen)=0;

      // Functions to train the sentence length model
  virtual void trainSentPairRange(std::pair<unsigned int,unsigned int> sentPairRange,
                                  int verbosity=0)=0;
  virtual void trainSentPair(std::vector<std::string> srcSentVec,
                             std::vector<std::string> trgSentVec,
                             Count c=1)=0;
  
      // clear function
  virtual void clear(void)=0;

      // Destructor
  virtual ~BaseSentLengthModel(void){}

};

#endif
